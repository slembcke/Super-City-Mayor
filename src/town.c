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
	'a', 'b', 'c', 'd',
};

static const u8 CITY_BLOCKS[16*15] = {
	1, 2, 3, 4,
};

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

Gamestate gameplay_screen(void){
	static u16 addr;
	
	music_stop();
	
	px_ppu_sync_disable();{
		px_buffer_blit(PAL_ADDR, PALETTE, sizeof(PALETTE));
		
		px_addr(NT_ADDR(0, 0, 0));
		px_fill(1024, 0);
		
		ix = 8, iy = 8;
		idx = 0 << 2;
		addr = ROW_ADDR[iy] + 2*ix;
		
		px_addr(addr);
		PPU.vram.data = (META_TILES + 0)[idx];
		PPU.vram.data = (META_TILES + 1)[idx];
		px_addr(addr + 32);
		PPU.vram.data = (META_TILES + 2)[idx];
		PPU.vram.data = (META_TILES + 3)[idx];
	} px_ppu_sync_enable();
	
	while(true){
		read_gamepads();
		
		if(JOY_START(pad1.press)) break;
		
		px_spr_end();
		px_wait_nmi();
	}
	
	return splash_screen();
}
