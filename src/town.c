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

Gamestate gameplay_screen(void){
	px_ppu_sync_disable();{
		px_addr(NT_ADDR(0, 0, 0));
		px_fill(1024, 0);
		
		px_addr(NT_ADDR(0, 8, 8));
		px_str("hello world");
	} px_ppu_sync_enable();
	
	music_play(0);
	
	while(true){
		read_gamepads();
		
		if(JOY_START(pad1.press)) break;
		
		px_spr_end();
		px_wait_nmi();
	}
	
	return splash_screen();
}
