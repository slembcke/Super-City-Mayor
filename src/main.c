#include <stdlib.h>
#include <string.h>

#include "pixler.h"
#include "common.h"

#define BG_COLOR 0x31
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

#define BUTTON_BIT 0x04
#define NON_WALKABLE_BIT 0x08
#define STORAGE_BIT 0x10
#define FULL_BIT 0x20

#define MPTY 0x00
#define SBUT (BUTTON_BIT | 0x00)
#define DBUT (BUTTON_BIT | 0x01)
#define RBUT (BUTTON_BIT | 0x02)
#define GBUT (BUTTON_BIT | 0x03)
#define WALL (NON_WALKABLE_BIT)
#define STOR (NON_WALKABLE_BIT | STORAGE_BIT)
#define FULL (NON_WALKABLE_BIT | STORAGE_BIT | FULL_BIT)

// Uff, running out of time. Gotta just cram this in.
//static u8 MAP[] = {
////  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
//	WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, // 0
//	WALL, FULL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, STOR, WALL, WALL, STOR, WALL, // 1
//	WALL, FULL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, STOR, WALL, // 2
//	WALL, FULL, MPTY, RBUT, MPTY, WALL, WALL, WALL, WALL, WALL, MPTY, MPTY, WALL, WALL, STOR, WALL, // 3
//	WALL, FULL, MPTY, MPTY, MPTY, WALL, STOR, MPTY, MPTY, WALL, SBUT, MPTY, WALL, WALL, STOR, WALL, // 4
//	WALL, FULL, MPTY, MPTY, MPTY, WALL, WALL, MPTY, MPTY, WALL, WALL, WALL, WALL, WALL, STOR, WALL, // 5
//	WALL, FULL, MPTY, MPTY, MPTY, WALL, STOR, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, STOR, WALL, // 6
//	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, STOR, WALL, // 7
//	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, STOR, WALL, // 8
//	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, STOR, WALL, // 9
//	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, STOR, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, STOR, WALL, // A
//	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, STOR, WALL, // B
//	WALL, WALL, MPTY, MPTY, MPTY, WALL, WALL, WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, STOR, WALL, // C
//	WALL, WALL, MPTY, MPTY, MPTY, WALL, DBUT, MPTY, MPTY, MPTY, MPTY, GBUT, MPTY, MPTY, STOR, WALL, // D
//	WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, // E
//};
static u8 MAP[] = {
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, // 0
	WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, MPTY, WALL, // 1
	WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, MPTY, WALL, // 2
	WALL, MPTY, MPTY, MPTY, MPTY, WALL, WALL, WALL, WALL, WALL, MPTY, MPTY, WALL, WALL, MPTY, WALL, // 3
	WALL, MPTY, MPTY, MPTY, MPTY, WALL, MPTY, MPTY, MPTY, WALL, MPTY, MPTY, WALL, WALL, MPTY, WALL, // 4
	WALL, MPTY, MPTY, MPTY, MPTY, WALL, WALL, MPTY, MPTY, WALL, WALL, WALL, WALL, WALL, MPTY, WALL, // 5
	WALL, MPTY, MPTY, MPTY, MPTY, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, MPTY, WALL, // 6
	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, MPTY, WALL, // 7
	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, // 8
	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, // 9
	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, // A
	WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, // B
	WALL, WALL, MPTY, MPTY, MPTY, WALL, WALL, WALL, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, // C
	WALL, WALL, MPTY, MPTY, MPTY, WALL, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, MPTY, WALL, // D
	WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, // E
};

#define MAP_BLOCK_AT(x, y) ((y & 0xF0)| (x >> 4))


Gamepad pad1, pad2;

void read_gamepads(void){
	pad1.prev = pad1.value;
	pad1.value = joy_read(0);
	pad1.press = pad1.value & (pad1.value ^ pad1.prev);
	pad1.release = pad1.prev & (pad1.value ^ pad1.prev);
	
	pad2.prev = pad2.value;
	pad2.value = joy_read(1);
	pad2.press = pad2.value & (pad2.value ^ pad2.prev);
	pad2.release = pad2.prev & (pad2.value ^ pad2.prev);
}

void wait_noinput(void){
	while(joy_read(0) || joy_read(1)) px_wait_nmi();
}

static void darken(register const u8* palette, u8 shift){
	for(idx = 0; idx < 32; idx++){
		ix = palette[idx];
		ix -= shift << 4;
		if(ix > 0x40 || ix == 0x0D) ix = 0x1D;
		px_buffer_set_color(idx, ix);
	}
}

void fade_from_black(const u8* palette, u8 delay){
	darken(palette, 4);
	px_wait_frames(delay);
	darken(palette, 3);
	px_wait_frames(delay);
	darken(palette, 2);
	px_wait_frames(delay);
	darken(palette, 1);
	px_wait_frames(delay);
	darken(palette, 0);
}

void meta_spr(u8 x, u8 y, u8 pal, const u8* data);
static const u8 META[] = {
	-8, -8, 0xD0, 0,
	 0, -8, 0xD1, 0,
	-8,  0, 0xD2, 0,
	 0,  0, 0xD3, 0,
	128,
};

static Gamestate splash_screen(void){
	register u8 player1x = 32, player1y = 32;
	register u8 x, y;
	register s16 sin = 0, cos = 0x3FFF;
	
	px_ppu_sync_disable();{
		// Load the splash tilemap into nametable 0.
		px_lz4_to_vram(NT_ADDR(0, 0, 0), MAP_SPLASH);
	} px_ppu_sync_enable();
	
	music_play(0);
	
	fade_from_black(PALETTE, 4);
	
	while(true){

		x = player1x;
		y = player1y;

		read_gamepads();
		if(JOY_LEFT (pad1.value)) x -= 1;
		if(JOY_RIGHT(pad1.value)) x += 1;
		if(JOY_DOWN (pad1.value)) y += 1;
		if(JOY_UP   (pad1.value)) y -= 1;
		if(JOY_BTN_A(pad1.press)) sound_play(SOUND_JUMP);

		//collision detection
		//ix = (player1x >> 8);
		//iy = (player1y >> 8) - 4;

		iz = MAP_BLOCK_AT(x,y);
		idx = MAP[iz];// & NON_WALKABLE_BIT;

		
		// Draw a sprite.
		//if(idx) {
		if(idx&WALL) {
			//blocked
			meta_spr(player1x, player1y, 1, META);
		}
		else {
			//allowed
			meta_spr(player1x, player1y, 2, META);
			player1x = x;
			player1y = y;
		}
		
		
		PX.scroll_y = 480 + (sin >> 9);
		sin += cos >> 6;
		cos -= sin >> 6;
		
		px_spr_end();
		px_wait_nmi();
	}
	
	return splash_screen();
}

void main(void){
	// Set up CC65 joystick driver.
	joy_install(nes_stdjoy_joy);
	
	// Set which tiles to use for the background and sprites.
	px_bg_table(0);
	px_spr_table(0);
	
	// Not using bank switching, but a good idea to set a reliable value at boot.
	px_uxrom_select(0);
	
	// Black out the palette.
	for(idx = 0; idx < 32; idx++) px_buffer_set_color(idx, 0x1D);
	px_wait_nmi();
	
	// Decompress the tileset into character memory.
	px_lz4_to_vram(CHR_ADDR(0, 0), CHR0);
	
	sound_init(SOUNDS);
	music_init(MUSIC);
	
	// Jump to the splash screen state.
	splash_screen();
}
