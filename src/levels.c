#include "common.h"

const u8 LEVEL_META_TILES[70] = {
	0x00, 0x00, 0x00, 0x00, 0, // 0 road
	0x00, 0x00, 0x00, 0x00, 0, // 1 road
	0xE0, 0xE1, 0xF0, 0xF1, 2, // 2 fixed 1
	0xE2, 0xE3, 0xF2, 0xF3, 1, // 3 damaged 1
	0xE4, 0xE5, 0xF4, 0xF5, 2, // 4 fixed 2
	0xE6, 0xE7, 0xF6, 0xF7, 1, // 5 damaged 2
	0xE8, 0xE9, 0xF8, 0xF9, 2, // 6 fixed 3
	0xEA, 0xEB, 0xFA, 0xFB, 1, // 7 damaged 3
	0xE0, 0xE1, 0xF0, 0xF1, 3, // 2 fixed 1
	0xE2, 0xE3, 0xF2, 0xF3, 1, // 3 damaged 1
	0xE4, 0xE5, 0xF4, 0xF5, 3, // 4 fixed 2
	0xE6, 0xE7, 0xF6, 0xF7, 1, // 5 damaged 2
	0xE8, 0xE9, 0xF8, 0xF9, 3, // 6 fixed 3
	0xEA, 0xEB, 0xFA, 0xFB, 1, // 7 damaged 3
};

#define _ 0	//street
#define A (2 | LEVEL_BITS_NON_WALKABLE)	//fixed building
#define a (3 | LEVEL_BITS_ACTION_ALLOWED | LEVEL_BITS_NON_WALKABLE)  //damaged building
#define B (4 | LEVEL_BITS_NON_WALKABLE)	//fixed building
#define b (5 | LEVEL_BITS_ACTION_ALLOWED | LEVEL_BITS_NON_WALKABLE)  //damaged building
#define C (6 | LEVEL_BITS_NON_WALKABLE)	//fixed building
#define c (7 | LEVEL_BITS_ACTION_ALLOWED | LEVEL_BITS_NON_WALKABLE)  //damaged building
#define D (8 | LEVEL_BITS_NON_WALKABLE)	//fixed building
#define d (9 | LEVEL_BITS_ACTION_ALLOWED | LEVEL_BITS_NON_WALKABLE)  //damaged building
#define E (10 | LEVEL_BITS_NON_WALKABLE)	//fixed building
#define e (11 | LEVEL_BITS_ACTION_ALLOWED | LEVEL_BITS_NON_WALKABLE)  //damaged building
#define F (12 | LEVEL_BITS_NON_WALKABLE)	//fixed building
#define f (13 | LEVEL_BITS_ACTION_ALLOWED | LEVEL_BITS_NON_WALKABLE)  //damaged building


#define W LEVEL_BITS_NON_WALKABLE	

const u8 LEVEL0[LEVEL_SIZE] = {
	_, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, 
	W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
	_, _, A, _, B, _, E, B, _, D, B, C, _, B, _, _,
	W, _, A, _, _, _, _, _, _, _, _, _, _, _, _, W,
	W, _, E, _, F, C, B, _, A, E, _, D, _, B, _, W,
	_, _, _, _, B, _, _, _, F, C, _, _, _, A, _, _,
	W, _, D, _, _, _, A, _, _, _, _, B, _, E, _, W,
	W, _, A, _, A, _, F, _, _, b, _, _, _, C, _, W,
	_, _, _, _, C, _, _, _, F, A, _, C, _, _, _, _,
	W, _, C, _, E, A, B, _, _, C, _, E, _, B, _, W,
	W, _, F, _, _, _, _, _, _, _, _, C, _, _, _, W,
	_, _, C, _, A, _, B, A, _, F, _, B, _, c, _, _,
	W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
	W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, 
};
