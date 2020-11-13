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
   if ( Score == 0x00 && NumTerms < 3) 
   {
      NumTerms++;
   }
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

#define PAINT_STATS \
{ \
   register u8 i; \
   register u8 temp = Score; \
\
   px_spr(128,  16, 1, '0'+((u8)temp%10)); \
   temp /= 10; \
   px_spr(120,  16, 1, '0'+(u8)temp%10); \
   temp /= 10; \
   px_spr(112,  16, 1, '0'+(u8)temp); \
   px_spr(136,  16, 1, '0'); \
   px_spr(144,  16, 1, '0'); \
   for ( i = 0; i < NumTerms; i++ ) \
   { \
      px_spr(152+(i<<3),  16, 1, 21); \
   } \
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
	u8 x, y, dir, character;
} Player;

// Player player1, player2;

static void player_update(Player* _player){
	static Player player;
   u8 idx;
	player = *_player;
	
	// Masking
	idx = LEVEL_TILE_AT_PIXEL(player.x,player.y+8); 
	if(CITY_BLOCKS[idx] & LEVEL_BITS_BUILDING) {
		ix = (idx << 4) & 0xF0;
		iy = (idx << 0) & 0xF0;
		px_spr(ix + 0, iy - 1, PX_SPR_BEHIND|3, 0xA0);
		px_spr(ix + 8, iy - 1, PX_SPR_BEHIND|3, 0xA1);
	}
	
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
		if(CITY_BLOCKS[idx] & LEVEL_BITS_ACTION_ALLOWED){
			fix_building(idx);
		} else {
			if ( player.dir == FACE_R ) {
				idx = LEVEL_TILE_AT_GRID(ix,iy-8);
				if (CITY_BLOCKS[idx] & LEVEL_BITS_ACTION_ALLOWED) {
					fix_building(idx);
				}
			}
			else if ( player.dir == FACE_L ) {
				idx = LEVEL_TILE_AT_GRID(ix,iy-8);
				if (CITY_BLOCKS[idx] & LEVEL_BITS_ACTION_ALLOWED) {
					fix_building(idx);
				}
			}
		}
	}	
		
	if(JOY_LEFT (player.pad.value)) {
		player.dir = FACE_L;

		idx = LEVEL_TILE_AT_PIXEL(player.x - 1, player.y);
		if(!(CITY_BLOCKS[idx] & LEVEL_BITS_NON_WALKABLE)) player.x--;
	} else if(JOY_RIGHT (player.pad.value)){
		player.dir = FACE_R;

      if ( player.x < 250 )
      { 
         idx = LEVEL_TILE_AT_PIXEL(player.x + 1, player.y);
         if(!(CITY_BLOCKS[idx] & LEVEL_BITS_NON_WALKABLE)) player.x++;
      }
      else
      {
         player.x++;
      }
	}

	if(JOY_UP(player.pad.value)) {
		player.dir = FACE_U;

		idx = LEVEL_TILE_AT_PIXEL(player.x, player.y - 1);
		if(!(CITY_BLOCKS[idx] & LEVEL_BITS_NON_WALKABLE)) player.y--;
	} else if(JOY_DOWN (player.pad.value)){
		player.dir = FACE_D;

		idx = LEVEL_TILE_AT_PIXEL(player.x, player.y + 1);
		if(!(CITY_BLOCKS[idx] & LEVEL_BITS_NON_WALKABLE)) player.y++;
	}
	
	
	// Calculate sprite frame index.
	idx = (player.x/4 + player.y/4) & 3;
	idx += 16*player.character + 4*player.dir;
	meta_spr(player.x, player.y, 0, metasprite_list[idx]);

	(*_player) = player;
}

Gamestate gameplay_screen(void){
	static Player player1, player2;
	
	static u8 broken_count;
	broken_count = Level;
	TOWN.break_timeout = Difficulty;
	
	player1.x =   8; player1.y = 60; player1.dir = FACE_R; player1.character = Player1;
	player2.x = 248; player2.y = 60; player2.dir = FACE_L; player2.character = Player2;
	
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
      
      px_spr(0,23,PX_SPR_BEHIND|2,145);
      PAINT_STATS;
      
		read_gamepads();
    
		player1.pad = pad1;
		player_update(&player1);

		if ( NumPlayers == 2 ){
			player2.pad = pad2;
			player_update(&player2);
		}
      
		if(count_broken() == 0){
			// This means there are no buildings left causing countdowns.
			// You win!
         return bonus_screen();
		} else if(TOWN.countdown < TOWN.count_rate){
         NumTerms--;
         if ( NumTerms == 0xff )
         {
            NumTerms = 0;
         }
         if ( NumTerms == 0 )
         {
            // Out of time, you lose!
            return lose_screen();
         }
         else
         {
            TOWN.countdown = ~0;
         }
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

Gamestate bonus_screen(){
	static u8 timeout = 60;

	music_stop();
	
	px_ppu_sync_disable();{
      PX.scroll_x = 0;
      PX.scroll_y = 0;

      PPU.mask |= 0x80;
      
		px_spr_clear();
	} px_ppu_sync_enable();
	
	while(true){
      px_spr(0,23,PX_SPR_BEHIND|2,145);
      
      PAINT_STATS;

		px_spr_end();

      // adjust scroll position to slide timer
      PX.scroll_x = 0xff-((TOWN.countdown>>8)&0xfe);
      PX.scroll_y = 0;
      if ( TOWN.countdown < 0xff ) break;
      TOWN.countdown -= 0xff;
      
      Score++;
      if ( Score == 0x00 && NumTerms < 3) NumTerms++;
      
		px_wait_nmi();

      // wait for sprite 0 hit to adjust back to normal playfield for game
      while(PPU.status&0x40){}
      while(!(PPU.status&0x40)){}
      
      PPU.scroll = 0;
      PPU.scroll = 0;
      PPU.control = 0x90;
	}
	
   PPU.mask &= 0x7F;
   
	sound_play(SOUND_MATCH);
      
   fade_to_black(PALETTE,4);

  return win_screen();
}
