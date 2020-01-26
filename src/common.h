#ifndef _COMMON_H
#define _COMMON_H

#include "pixler.h"

// data.s declarations:
extern u8 CHR0[];
extern u8 MAP_SPLASH[];

// misc.s declarations:
extern u8 ix, iy, idx, tmp;
#pragma zpsym("ix");
#pragma zpsym("iy");
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

#endif _COMMON_H
