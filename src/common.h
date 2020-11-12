#ifndef _COMMON_H
#define _COMMON_H

#include "pixler.h"

#define BG_COLOR 0x1D
extern const u8 PALETTE[];

// data.s declarations:
extern u8 CHR0[];
extern u8 MAP_SPLASH[];

// misc.s declarations:
extern u8 ix, iy, iz, idx, tmp;
#pragma zpsym("ix");
#pragma zpsym("iy");
#pragma zpsym("iz");
#pragma zpsym("idx");
#pragma zpsym("tmp");

// audio.s declarations:
typedef struct AudioChunk AudioChunk;
extern const AudioChunk MUSIC;
extern const AudioChunk SOUNDS;

void music_init(const AudioChunk *music);
void music_play(u8 song);
void music_pause();
void music_stop();

enum {
	// Magic numbers from Famitone.
	// Is there a better way to expose these?
	SOUND_CH0 = (0*15) << 8,
	SOUND_CH1 = (1*15) << 8,
	SOUND_CH2 = (2*15) << 8,
	SOUND_CH3 = (3*15) << 8,
	
	SOUND_JUMP = 0 | SOUND_CH0,
	SOUND_MATCH = 1 | SOUND_CH1,
	SOUND_PICKUP = 2 | SOUND_CH0,
	SOUND_DROP = 3 | SOUND_CH0,
};

void sound_init(const AudioChunk *sounds);
void sound_play(u16 sound);

// Handy utils:

typedef struct {
	u8 value, prev, press, release;
} Gamepad;

extern Gamepad pad1, pad2;
void read_gamepads(void);
void wait_noinput(void);

void meta_spr(u8 x, u8 y, u8 pal, const u8* data);

// Gamestates

typedef struct {u8 _;} Gamestate;

Gamestate splash_screen(void);
Gamestate player_select_screen(void);
Gamestate gameplay_screen(u8 difficulty, u8 level);
Gamestate lose_screen(void);
Gamestate win_screen(u8 difficulty, u8 level);
Gamestate bonus_screen(u8 difficulty, u8 level);

// Level declarations.

#define LEVEL_TILE_AT_PIXEL(x, y) ((y & 0xF0)| (x >> 4))	//pixel based
#define LEVEL_TILE_AT_GRID(x, y) ((y << 4)| (x))	//grid based

enum LEVEL_BITS {
	LEVEL_BITS_DESTROYED = 0x01,
	//ie set when building damaged & player can fix
	LEVEL_BITS_ACTION_ALLOWED = 0x40,
	LEVEL_BITS_NON_WALKABLE = 0x80,
	
	// Masks:
	LEVEL_BITS_METATILE = 0x1F,
	LEVEL_BITS_BUILDING = (LEVEL_BITS_METATILE & ~LEVEL_BITS_DESTROYED),
};

#define LEVEL_SIZE (16*15)
extern const u8 LEVEL_META_TILES[70];
extern const u8 LEVEL0[LEVEL_SIZE];

// data.s declarations:

extern u8 SPRITES_CHR[];

extern u8 TITLE_PALETTE[];
extern u8 TITLE_CHR[];
extern u8 TITLE_TILEMAP[];

extern u8 PLAYERSELECT_PALETTE[];
extern u8 PLAYERSELECT_CHR[];
extern u8 PLAYERSELECT_TILEMAP_11[];
extern u8 PLAYERSELECT_TILEMAP_12[];
extern u8 PLAYERSELECT_TILEMAP_21[];
extern u8 PLAYERSELECT_TILEMAP_22[];

extern u8 GAMEPLAY_PALETTE[];
extern u8 GAMEPLAY_CHR[];
extern u8 GAMEPLAY_TILEMAP[];

#endif _COMMON_H
