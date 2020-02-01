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
};

static const u8 META_TILE_PAL[] = {
	0, 0, 1, 2, 3,
};

static const u8 CITY_BLOCKS[16*15] = {
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 0, 2, 0, 3, 0, 4, 0, 1, 2, 3, 4, 0, 0, 0,
	0, 1, 2, 3, 4, 0, 0, 0, 1, 2, 3, 4, 0, 0, 1, 2,
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

Gamestate gameplay_screen(void){
	static u16 addr;
	static u8 tile;
	
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
				tile = CITY_BLOCKS[idx] << 2;
				
				if(tile == 0) continue;
				
				addr = ROW_ADDR[iy] + 2*ix;
				px_buffer_data(2, addr);
				PX.buffer[0] = (META_TILES + 0)[tile];
				PX.buffer[1] = (META_TILES + 1)[tile];
				px_buffer_data(2, addr + 32);
				PX.buffer[0] = (META_TILES + 2)[tile];
				PX.buffer[1] = (META_TILES + 3)[tile];
				
				// Calculate atrrib table bit index.
				idx = 4*(iy & 0xE) + ix/2;
				px_buffer_data(1, AT_ADDR(0) + idx);
				
				tmp = META_TILE_PAL[tile >> 2];
				tmp = PAL[tmp];
				idx = 2*(iy & 1) + (ix & 1);
				tmp &= META_MASK[idx];
				PX.buffer[0] = tmp;
			}
			
			// Buffer only one row at a time to avoid overflows.
			px_buffer_exec();
		}
		
	} px_ppu_sync_enable();
	
	ix = 0xAB;
	
	while(true){
		read_gamepads();
		
		if(JOY_START(pad1.press)) break;
		
		PX.scroll_x = 0;
		PX.scroll_y = 0;
		px_spr_end();
		px_wait_nmi();
	}
	
	return splash_screen();
}
