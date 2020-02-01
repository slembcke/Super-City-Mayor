#include "pixler.h"
#include "common.h"

#define BG_COLOR 0x1D
static const u8 PALETTE[] = {
	BG_COLOR, 0x00, 0x10, 0x20,
	BG_COLOR, 0x06, 0x16, 0x26,
	BG_COLOR, 0x09, 0x19, 0x29,
	BG_COLOR, 0x01, 0x11, 0x21,
	
	BG_COLOR, 0x00, 0x10, 0x20,
	BG_COLOR, 0x06, 0x16, 0x26,
	BG_COLOR, 0x09, 0x19, 0x29,
	BG_COLOR, 0x01, 0x11, 0x21,
};

static const u8 META_TILES[] = {
	'.', '.', '.', '.',
	'W', 'W', 'W', 'W',
	'R', 'R', 'R', 'R',
	'G', 'G', 'G', 'G',
	'B', 'B', 'B', 'B',
	'5', '5', '5', '5',
	'6', '6', '6', '6',
	'7', '7', '7', '7',
};

static const u8 META_TILE_PAL[64] = {
	0, 0, 1, 2, 3,
};

//#define BUTTON_BIT 0x04
#define NON_WALKABLE_BIT 0x01
//#define STORAGE_BIT 0x10
//#define FULL_BIT 0x20

//#define MPTY 0x00
//#define SBUT (BUTTON_BIT | 0x00)
//#define DBUT (BUTTON_BIT | 0x01)
//#define RBUT (BUTTON_BIT | 0x02)
//#define GBUT (BUTTON_BIT | 0x03)
#define WALL (NON_WALKABLE_BIT)
//#define STOR (NON_WALKABLE_BIT | STORAGE_BIT)
//#define FULL (NON_WALKABLE_BIT | STORAGE_BIT | FULL_BIT)

#define MAP_BLOCK_AT(x, y) ((y & 0xF0)| (x >> 4))

static const u8 CITY_BLOCKS[16*15] = {
	1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	3, 4, 0, 3, 0, 5, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 1, 2, 5, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 3, 0, 5, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 3, 0, 5, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 3, 0, 5, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 2, 0, 3, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 3, 0, 5, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};

static u8 ATTRIB_TABLE[64];

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

static u8 META_MASK[] = {0x03, 0x0C, 0x30, 0xC0};
static u8 PAL[] = {0x00, 0x55, 0xAA, 0xFF};

/*
static const u8 META[] = {
	-8, -8, 0xD0, 0,
	 0, -8, 0xD1, 0,
	-8,  0, 0xD2, 0,
	 0,  0, 0xD3, 0,
	128,
};
*/
static u8 META[][2][17] = 
{
 {
   {
	-8, -8, 0xE0, 0,
	 0, -8, 0xE1, 0,
	-8,  0, 0xE2, 0,
	 0,  0, 0xE3, 0,
	128,
   },
   {
	-8, -8, 0xE4, 0,
	 0, -8, 0xE5, 0,
	-8,  0, 0xE6, 0,
	 0,  0, 0xE7, 0,
	128,
   }
 },
 {
   {
	0, -8, 0xE0, 0x40,
	-8, -8, 0xE1, 0x40,
	0,  0, 0xE2, 0x40,
	-8,  0, 0xE3, 0x40,
	128,
   },
   {
	0, -8, 0xE4, 0x40,
	-8, -8, 0xE5, 0x40,
	0,  0, 0xE6, 0x40,
	-8,  0, 0xE7, 0x40,
	128,
   }
 },
 {
   {
	-8, -8, 0xEC, 0,
	0, -8, 0xED, 0,
	-8,  0, 0xEE, 0,
	0,  0, 0xEF, 0,
	128,
   },
   {
	-8, -8, 0xEC, 0,
	0, -8, 0xED, 0,
	-8,  0, 0xF2, 0,
	0,  0, 0xF3, 0,
	128,
   }
 },
 {
   {
	-8, -8, 0xF0, 0,
	0, -8, 0xF1, 0,
	-8,  0, 0xEE, 0,
	0,  0, 0xEF, 0,
	128,
   },
   {
	-8, -8, 0xF0, 0,
	0, -8, 0xF1, 0,
	-8,  0, 0xF2, 0,
	0,  0, 0xF3, 0,
	128,
   }
 }
};


Gamestate gameplay_screen(void){
	static u16 addr;
	static u8 tile, mask, pal;
	
	register u8 player1x = 32, player1y = 32;
	register u8 x, y;

   register u8 a = 0, d = 1;
   register u8 f = 0, b = 0;

	PX.scroll_x = 0;
	PX.scroll_y = 0;
	px_spr_end();
	px_wait_nmi();

	music_stop();
	
	px_ppu_sync_disable();{
		px_buffer_blit(PAL_ADDR, PALETTE, sizeof(PALETTE));
		
		px_addr(NT_ADDR(0, 0, 0));
		px_fill(32*30, '.');
		px_fill(64, 0);
		
		for(iy = 0; iy < 15; ++iy){
			for(ix = 0; ix < 16; ++ix){
				// Calculate tile index.
				idx = 16*iy + ix;
				tile = CITY_BLOCKS[idx];
				
				if(tile == 0) continue;
				
				// Load the attribute quadrant mask and palette.
				idx = 2*(iy & 1) + (ix & 1);
				mask = META_MASK[idx];
				idx = META_TILE_PAL[tile];
				pal = PAL[idx];
				
				// Calculate atrrib table byte index.
				idx = 4*(iy & 0xE) + ix/2;
				tmp = ATTRIB_TABLE[idx];
				tmp = (tmp & ~mask) | (pal & mask);
				ATTRIB_TABLE[idx] = tmp;
				
				px_buffer_data(1, AT_ADDR(0) + idx);
				PX.buffer[0] = tmp;
				
				// Load metatile.
				tile *= 4;
				addr = ROW_ADDR[iy] + 2*ix;
				px_buffer_data(2, addr);
				PX.buffer[0] = (META_TILES + 0)[tile];
				PX.buffer[1] = (META_TILES + 1)[tile];
				px_buffer_data(2, addr + 32);
				PX.buffer[0] = (META_TILES + 2)[tile];
				PX.buffer[1] = (META_TILES + 3)[tile];
			}
			
			// Buffer only one row at a time to avoid overflows.
			px_buffer_exec();
		}
		
	} px_ppu_sync_enable();
	
	ix = 0xAB;
	
	while(true){
		read_gamepads();
		
		if(JOY_START(pad1.press)) break;

		x = player1x;
		y = player1y;
		//if(JOY_LEFT (pad1.value)) x -= 1;
		//if(JOY_RIGHT(pad1.value)) x += 1;
		//if(JOY_DOWN (pad1.value)) y += 1;
		//if(JOY_UP   (pad1.value)) y -= 1;

		if(JOY_LEFT (pad1.value))
      {
         if ( x&8 ) a++;
         d = 1;
         a = (x>>3)&1;    
         x -= 1;
         if ( x < 5 ) b++;
      }
		else if(JOY_RIGHT (pad1.value))
      {
         if ( x&8 ) a++;
         d = 0;
         a = (x>>3)&1;    
         x += 1;
         if ( x > 250 ) b++;
      }
		else if(JOY_UP   (pad1.value))
      {
         if ( y&8 ) a++;
         d = 2;
         a = (y>>3)&1;    
         y -= 1;
         if ( y < 5 ) b++;
      }
		else if(JOY_DOWN (pad1.value))
      {
         if ( y&8 ) a++;
         d = 3;
         a = (y>>3)&1;    
         y += 1;
         if ( y > 235 ) b++;
      }
		
		//check requested move location
		iz = MAP_BLOCK_AT(x,y);
		idx = CITY_BLOCKS[iz];// & NON_WALKABLE_BIT;

		
		// Draw a sprite.
		if(idx&WALL) {
			//blocked
			//meta_spr(player1x, player1y, 1, META);
      			meta_spr(player1x, player1y, 1, META[d][a]);
		}
		else {
			//allowed update player location to requested
			player1x = x;
			player1y = y;
			//meta_spr(player1x, player1y, 2, META);
      			meta_spr(player1x, player1y, 2, META[d][a]);
		}

		// PX.scroll_x = 0;
		// PX.scroll_y = 0;
		px_spr_end();
		px_wait_nmi();
	}
	
	return splash_screen();
}
