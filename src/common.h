#ifndef _COMMON_H
#define _COMMON_H

#include "pixler.h"

// data.s declarations:
extern u8 CHR0[];
extern u8 MAP_SPLASH[];

// misc.s declarations:
extern u8 iz, ix, iy, idx, tmp;
extern u8 player1x, player1y;
extern u8 player2x, player2y;
extern u8 player2item, player1item;
#pragma zpsym("player1x");
#pragma zpsym("player1y");
#pragma zpsym("player2x");
#pragma zpsym("player2y");
#pragma zpsym("player1item");
#pragma zpsym("player2item");
#pragma zpsym("ix");
#pragma zpsym("iy");
#pragma zpsym("iz");
#pragma zpsym("idx");
#pragma zpsym("tmp");

// audio.s declarations:
typedef struct {} AudioChunk;
extern const AudioChunk MUSIC[];
extern const AudioChunk SOUNDS[];

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
typedef struct {} Gamestate;

typedef struct {
	u8 value, prev, press, release;
} Gamepad;

extern Gamepad pad1, pad2;
void read_gamepads(void);
void wait_noinput(void);

void meta_spr(u8 x, u8 y, u8 pal, const u8* data);

// Gamestates

Gamestate splash_screen(void);
Gamestate gameplay_screen(void);
Gamestate lose_screen(void);

extern u8 GAMEPLAY_PALETTE[];
extern u8 GAMEPLAY_CHR[];
extern u8 GAMEPLAY_TILEMAP[];

#endif _COMMON_H
