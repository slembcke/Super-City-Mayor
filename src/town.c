#include <string.h>

#include "pixler.h"
#include "common.h"
#include "main.h"

static u8 CITY_BLOCKS[LEVEL_SIZE];
static u8 ATTRIB_TABLE[64];

static void load_metatile(u8 x, u8 y, u8 tile){
	static const u8 META_MASK[] = {0x03, 0x0C, 0x30, 0xC0};
	static const u8 PAL[] = {0x00, 0x55, 0xAA, 0xFF};
	static const u16 ROW_ADDR[] = {
		NT_ADDR(0, 0,  0),
		NT_ADDR(0, 0,  2),
		NT_ADDR(0, 0,  4),
		NT_ADDR(0, 0,  6),
		NT_ADDR(0, 0,  8),
		NT_ADDR(0, 0, 10),
		NT_ADDR(0, 0, 12),
		NT_ADDR(0, 0, 14),
		NT_ADDR(0, 0, 16),
		NT_ADDR(0, 0, 18),
		NT_ADDR(0, 0, 20),
		NT_ADDR(0, 0, 22),
		NT_ADDR(0, 0, 24),
		NT_ADDR(0, 0, 26),
		NT_ADDR(0, 0, 28),
	};

	static u16 addr;
	static u8 mask, pal;
	
	tile *= 5;
	
	// Load the attribute quadrant mask and palette.
	idx = 2*(y & 1) + (x & 1);
	mask = META_MASK[idx];
	idx = (LEVEL_META_TILES + 4)[tile];
	pal = PAL[idx];
	
	// Calculate atrrib table byte index.
	idx = 4*(y & 0xE) + x/2;
	tmp = ATTRIB_TABLE[idx];
	tmp = (tmp & ~mask) | (pal & mask);
	ATTRIB_TABLE[idx] = tmp;
	
	px_buffer_data(1, AT_ADDR(0) + idx);
	PX.buffer[0] = tmp;
	
	// Load metatile.
	addr = ROW_ADDR[y] + 2*x;
	px_buffer_data(2, addr);
	PX.buffer[0] = (LEVEL_META_TILES + 0)[tile];
	PX.buffer[1] = (LEVEL_META_TILES + 1)[tile];
	px_buffer_data(2, addr + 32);
	PX.buffer[0] = (LEVEL_META_TILES + 2)[tile];
	PX.buffer[1] = (LEVEL_META_TILES + 3)[tile];
}

struct {
	u8 gameplay_coro[32];

	u16 countdown;
	u16 count_rate;
	u8 break_timeout;
	
	u8 score;
} TOWN;

#define RATE_PER_BUILDING 3

static void break_building(bool should_yield){
	while(true){
		idx = rand8();
		
		if(idx >= sizeof(CITY_BLOCKS)) goto yield;
		
		tmp = CITY_BLOCKS[idx];
		if(tmp & LEVEL_BITS_BUILDING){
			sound_play(SOUND_DROP);
			TOWN.count_rate += RATE_PER_BUILDING;
			
			tmp |= (LEVEL_BITS_DESTROYED | LEVEL_BITS_ACTION_ALLOWED);
			CITY_BLOCKS[idx] = tmp;
			load_metatile(idx & 0xF, idx >> 4, tmp & LEVEL_BITS_METATILE);
			return;
		}
		
		yield: if(should_yield) px_coro_yield(0);
	}
}

static void fix_building(u8 idx){
	sound_play(SOUND_MATCH);
	TOWN.count_rate -= RATE_PER_BUILDING;
	
	tmp = CITY_BLOCKS[idx];
	tmp &= ~(LEVEL_BITS_DESTROYED | LEVEL_BITS_ACTION_ALLOWED);
	
	//update the building
	CITY_BLOCKS[idx] = tmp;
	load_metatile(idx & 0xF, idx >> 4, tmp & LEVEL_BITS_METATILE);
	Score++;
   if ( Score == 0xff ) ultimate_win_screen();
}

//#define BUILDING_BREAK_TIMEOUT 240

static uintptr_t gameplay_coro_body(uintptr_t unused){
	//static u8 timeout = BUILDING_BREAK_TIMEOUT;
	static u8 timeout;
	timeout = TOWN.break_timeout;
	while(true){
		if((px_ticks & 1) && (--timeout == 0)){
			break_building(true);
			timeout = TOWN.break_timeout;
		}
		px_coro_yield(0);
	}
	
	while(true) px_coro_yield(0);
	
	(void)unused;
	return 0;
}

void paint_score()
{
   u8 temp = Score;

   px_spr(128,  16, 1, '0'+((u8)temp%10));
   temp /= 10;
   px_spr(120,  16, 1, '0'+(u8)temp%10);
   temp /= 10;
   px_spr(112,  16, 1, '0'+(u8)temp);
   px_spr(136,  16, 1, '0');
   px_spr(144,  16, 1, '0');
}

u8 collision_check(u8 x, u8 y) {

	//check requested move location
	iz = LEVEL_TILE_AT_PIXEL(x,y);
	return CITY_BLOCKS[iz];// & LEVEL_BITS_NON_WALKABLE;
}

u8 count_broken(void){
	iz = 0;
	
	// TODO this loop is super expensive.
	for(idx = 0; idx < 16*15; ++idx){
		tmp = CITY_BLOCKS[idx];
		if(tmp & LEVEL_BITS_DESTROYED) iz++;
	}
	
	return iz;
}

enum FACE {FACE_R, FACE_L, FACE_D, FACE_U};

typedef struct {
	Gamepad pad;
	u8 x, y, dir;
} Player;

// Player player1, player2;

static void player_update(Player* _player){
	static Player player;
	player = *_player;
	
	// Repairs.
	if(JOY_BTN_A(player.pad.press)){
		//map player position to city grid
		ix = player.x/16;
		iy = player.y/16;

		//change to cell player is facing
		switch(player.dir){
			case FACE_L: ix--; break;
			case FACE_R: ix++; break;
			case FACE_D: iy++; break;
			case FACE_U: iy--; break;
		}

		//is the building actionable?
		idx = LEVEL_TILE_AT_GRID(ix,iy);
		tmp = CITY_BLOCKS[idx];
		if(tmp & LEVEL_BITS_ACTION_ALLOWED){
			fix_building(idx);
		} else {
			if ( player.dir == FACE_R ) {
				idx = LEVEL_TILE_AT_GRID(ix,iy-8);
				tmp = CITY_BLOCKS[idx];
				if (tmp & LEVEL_BITS_ACTION_ALLOWED) {
					fix_building(idx);
				}
			}
			else if ( player.dir == FACE_L ) {
				idx = LEVEL_TILE_AT_GRID(ix,iy-8);
				tmp = CITY_BLOCKS[idx];
				if (tmp & LEVEL_BITS_ACTION_ALLOWED) {
					fix_building(idx);
				}
			}
		}
	}	
		
	if(JOY_LEFT (player.pad.value)) {
		player.dir = FACE_L;

		idx = collision_check(player.x - 1, player.y);
		if(!(idx & LEVEL_BITS_NON_WALKABLE)) player.x--;
	} else if(JOY_RIGHT (player.pad.value)){
		player.dir = FACE_R;

		idx = collision_check(player.x + 1, player.y);
		if(!(idx & LEVEL_BITS_NON_WALKABLE)) player.x++;
	}

	if(JOY_UP(player.pad.value)) {
		player.dir = FACE_U;

		idx = collision_check(player.x, player.y - 1);
		if(!(idx & LEVEL_BITS_NON_WALKABLE)) player.y--;
	} else if(JOY_DOWN (player.pad.value)){
		player.dir = FACE_D;

		idx = collision_check(player.x, player.y + 1);
		if(!(idx & LEVEL_BITS_NON_WALKABLE)) player.y++;
	}
	
	
	// Calculate sprite frame index.
	idx = (player.x/4 + player.y/4) & 3;
	idx += 16*Player1 + 4*player.dir;
	meta_spr(player.x, player.y, 0, metasprite_list[idx]);

	(*_player) = player;
}

Gamestate gameplay_screen(u8 difficulty, u8 level){
	static Player player1, player2;

	register u8 x, y;

	// static u8 a1 = 0, da1 = 1, dir1 = FACE_R;
   register u8 a2 = 0, da2 = 1, dir2 = FACE_L;
	
	u8 broken_count = 2 + level;
	TOWN.break_timeout = difficulty;

	player1.x =   8; player1.y = 60;
	player2.x = 248; player2.y = 60;
	
	PX.scroll_x = 0;
	PX.scroll_y = 0;
	px_spr_end();
	px_wait_nmi();
	
	memset(&TOWN, 0, sizeof(TOWN));
	TOWN.countdown = ~0;
	
	px_coro_init(gameplay_coro_body, TOWN.gameplay_coro, sizeof(TOWN.gameplay_coro));
	memset(ATTRIB_TABLE, 0, sizeof(ATTRIB_TABLE));
	memcpy(CITY_BLOCKS, LEVEL0, sizeof(CITY_BLOCKS));
	
	px_ppu_sync_disable();{
		
      px_lz4_to_vram(CHR_ADDR(0,0), SPRITES_CHR);
      px_lz4_to_vram(CHR_ADDR(1,0), GAMEPLAY_CHR);
      px_lz4_to_vram(NT_ADDR(0, 0, 0), GAMEPLAY_TILEMAP);
      px_lz4_to_vram(NT_ADDR(1, 0, 0), GAMEPLAY_TILEMAP);
		px_addr(NT_ADDR(0, 0, 2));
      px_fill(32, 1);
      
		for(iy = 0; iy < 15; ++iy){
			for(ix = 0; ix < 16; ++ix){
				// Calculate tile index.
				idx = 16*iy + ix;
				tmp = CITY_BLOCKS[idx];
				
				if(tmp & LEVEL_BITS_DESTROYED) TOWN.count_rate += RATE_PER_BUILDING;
				
				tmp &= LEVEL_BITS_METATILE;
				if(tmp != 0) load_metatile(ix, iy, tmp);
			}
			
			// Buffer only one row at a time to avoid overflows.
			px_buffer_exec();
		}
		
	} px_ppu_sync_enable();
	
   fade_from_black(GAMEPLAY_PALETTE,4);
   
	music_play(0);
	
	while(true){
      if ( broken_count > 0 )
      {
         break_building(false);
         broken_count--;
      }
      
		read_gamepads();
		
		px_spr(0,23,PX_SPR_BEHIND|2,145);
		paint_score();
    
		px_profile_start();
		player1.pad = pad1;
		player_update(&player1);
		px_profile_end();

      if ( NumPlayers == 2 )
      {
//PLAYER 2 REPAIRS
		if(JOY_BTN_A (pad2.press)) {
			//map player position to city grid
			x = player2.x>>4;
			y = player2.y>>4;

			//change to cell player is facing
			if (dir2 == FACE_L)
				x--;
			else if (dir2 == FACE_R)
				x++;
			else if (dir2 == FACE_D)
				y++;
			else if (dir2 == FACE_U)
				y--;

			//is the building actionable?
			idx = LEVEL_TILE_AT_GRID(x,y); //idx = 16*y + x;
			tmp = CITY_BLOCKS[idx];
			if (tmp & LEVEL_BITS_ACTION_ALLOWED) {

				//resource or damaged?
				//if (tmp == RESOURCE_HUB) {
				//	player2item = RESOURCE;

				//} else if (tmp == RESOURCE ) {
				//	//remove resource by making building red for now
				//	CITY_BLOCKS[idx] = BUILDING;
				//	load_metatile(x, y, 1); 
				//	//player has item
				//	player2item = RESOURCE;

				//} else {
				//	if (player2item == RESOURCE) {
						fix_building(idx);
				//		player2item = NOITEM;
				//	}
				//}
			}
         else
         {
            if ( dir2 == FACE_R )
            {
               idx = LEVEL_TILE_AT_GRID(x,y-8); //idx = 16*y + x;
               tmp = CITY_BLOCKS[idx];
               if (tmp & LEVEL_BITS_ACTION_ALLOWED) {
						fix_building(idx);
               }
            }
            else if ( dir2 == FACE_L )
            {
               idx = LEVEL_TILE_AT_GRID(x,y-8); //idx = 16*y + x;
               tmp = CITY_BLOCKS[idx];
               if (tmp & LEVEL_BITS_ACTION_ALLOWED) {
						fix_building(idx);
               }
            }
         }
		}	

//PLAYER 2 MOVEMENT
         x = player2.x;
         y = player2.y;

		if(JOY_LEFT (pad2.value))
      {
         if ( x&8 ) a2++;
         dir2 = FACE_L;
         a2 = (x>>2)&3;    
         
         idx = collision_check(x-1, y);

         if(!(idx & LEVEL_BITS_NON_WALKABLE)) {
            //allowed update player location to requested
            player2.x = x-1;
            player2.y = y;
         }
      }
		else if(JOY_RIGHT (pad2.value))
      {
         if ( x&8 ) a2++;
         dir2 = FACE_R;
         a2 = (x>>2)&3;    
         
         idx = collision_check(x+1, y);

         if(!(idx & LEVEL_BITS_NON_WALKABLE)) {
            //allowed update player location to requested
            player2.x = x+1;
            player2.y = y;
         }
      }
		if(JOY_UP   (pad2.value))
      {
         dir2 = FACE_U;
         a2 += da2*((y>>2)&1);    
         
         idx = collision_check(x, y-1);

         if(!(idx & LEVEL_BITS_NON_WALKABLE)) {
            //allowed update player location to requested
            player2.x = x;
            player2.y = y-1;
         }
      }
		else if(JOY_DOWN (pad2.value))
      {
         dir2 = FACE_D;
         a2 += da2*((y>>2)&1);    
         
         idx = collision_check(x, y+1);

         if(!(idx & LEVEL_BITS_NON_WALKABLE)) {
            //allowed update player location to requested
            player2.x = x;
            player2.y = y+1;
         }
      }
		if ( a2 == 4 ) 
      {
         a2 = 3;
         da2 = -1;
      }
      if ( a2 == 0xff ) 
      {
         a2 = 0;
         da2 = 1;
      }

		//Draw a sprite.
		// if( player2item == RESOURCE ) {
    //         meta_spr(player2x, player2y, 3, metasprite_list[(1*16)+(dir2*4)+a2]);
		// } else {
            meta_spr(player2.x, player2.y, 0, metasprite_list[(Player2*16)+(dir2*4)+a2]);
		// }
      }
      
		if(count_broken() == 0){
			// This means there are no buildings left causing countdowns.
			// You win!
			return win_screen(difficulty, level);
		} else if(TOWN.countdown < TOWN.count_rate){
			// Out of time, you lose!
			return lose_screen();
		} else {
			TOWN.countdown -= TOWN.count_rate;
		}
		
		px_coro_resume(TOWN.gameplay_coro, 0);

		px_spr_end();

      // adjust scroll position to slide timer
      PX.scroll_x = 0xff-((TOWN.countdown>>8)&0xfe);
      PX.scroll_y = 0;
		px_wait_nmi();

      // wait for sprite 0 hit to adjust back to normal playfield for game
      while(PPU.status&0x40){}
      while(!(PPU.status&0x40)){}
      
      PPU.scroll = 0;
      PPU.scroll = 0;
      PPU.control = 0x90;
	}
   
   music_stop();
	
	return splash_screen();
}
