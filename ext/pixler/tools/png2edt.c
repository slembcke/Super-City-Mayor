#include <stdbool.h>
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "shared.h"

#define MAX_SQ 256
#define FP_MULT 8
#define TILE_SIZE 32
#define TILE_BYTES (TILE_SIZE*TILE_SIZE)

#define MAX_TEMP_TILES 4096

typedef struct {uint x, y;} uvec;

static float intersect_parabolas(uvec a, uvec b){
	int num = (a.x*a.x + a.y) - (b.x*b.x + b.y);
	int denom = 2*(a.x - b.x);
	return (float)num/(float)denom;
}

// TODO hardcoded sizes.
uvec v[1024];
float z[1024];

static void build_hull(uint sqedt[], uint count){
	v[0] = (uvec){0, sqedt[0]};
	z[0] = -MAX_SQ, z[1] = MAX_SQ;
	
	uint k = 0;
	for(uint i = 1; i < count; i++){
		uvec p = v[k], q = {i, sqedt[i]};
		float s = intersect_parabolas(p, q);
		
		while(s <= z[k]){
			k -= 1, p = v[k], s = intersect_parabolas(p, q);
		}
		
		k += 1;
		v[k] = q;
		z[k] = s;
		z[k + 1] = i + MAX_SQ;
	}
}

static void march_parabolas(uint count, uint output[], uint stride){
	uint j = 0;
	for(uint i = 0; i < count; i++){
		while(z[j + 1] < i) j += 1;
		int dx = i - v[j].x;
		output[i*stride] = dx*dx + v[j].y;
	}
}

static void separable_sqedt(uint output[], uint input[], uint w, uint h){
	for(uint row = 0; row < h; row++){
		build_hull(input + row*w, w);
		march_parabolas(w, output + row, h);
	}
}

static uint *generate_sqedt(Image image){
	uint edt_len = image.w*image.h;
	uint *buff0 = malloc(2*sizeof(*buff0)*edt_len);
	uint *buff1 = buff0 + edt_len;
	buff1[0] = 0;
	
	// Convert to initial squared EDT values.
	for(uint i = 0; i < edt_len; i++) buff0[i] = (image.pixels[i] ? MAX_SQ : 0);
	
	// Apply a pair of separable filter passes.
	separable_sqedt(buff1, buff0, image.w, image.h);
	separable_sqedt(buff0, buff1, image.h, image.w);
	return realloc(buff0, sizeof(*buff0)*edt_len);
}

int find_tile(u8 *tile, u8 tileset[][TILE_BYTES], uint count){
	for(uint i = 0; i < count; i++){
		if(memcmp(tile, tileset[i], TILE_BYTES) == 0) return i;
	}
	
	return -1;
}

int main(int argc, char *argv[]){
	SLIB_ASSERT_HARD(argc == 3, "Usage: %s infile outfile", argv[0]);
	
	FILE *infile = fopen(argv[1], "r");
	SLIB_ASSERT_HARD(infile, "Can't open input file '%s'", argv[1]);
	
	FILE *outfile = fopen(argv[2], "w");
	SLIB_ASSERT_HARD(outfile, "Can't open output file '%s'", argv[2]);
	
	Image image = read_png(infile);
	uint *sqedt = generate_sqedt(image);
	
	// Convert to SQEDT to EDT.
	uint len = image.w*image.h;
	u8 edt[len];
	for(uint i = 0; i < len; i++) edt[i] = FP_MULT*sqrtf(sqedt[i]);
	
	// TODO Apply sample offsets.
	
	// Generate collision map and tileset.
	u8 tileset[MAX_TEMP_TILES][TILE_BYTES];
	uint tileset_count = 0;
	
	uint tile_w = image.w/TILE_SIZE, tile_h = image.h/TILE_SIZE;
	u8 tilemap[tile_w*tile_h];
	
	for(uint ty = 0; ty < tile_h; ty++){
		for(uint tx = 0; tx < tile_w; tx++){
			u8 *tile_pixels = edt + TILE_SIZE*tx + TILE_SIZE*ty*image.w;
			u8 tile[TILE_BYTES];
			for(uint row = 0; row < TILE_SIZE; row++) memcpy(tile + TILE_SIZE*row, tile_pixels + image.w*row, TILE_SIZE);
			
			int tile_idx = find_tile(tile, tileset, tileset_count);
			if(tile_idx < 0){
				SLIB_ASSERT_HARD(tileset_count < MAX_TEMP_TILES, "Max temp files exceeded. (%d)", MAX_TEMP_TILES);
				tile_idx = tileset_count;
				memcpy(tileset[tileset_count], tile, TILE_BYTES);
				tileset_count += 1;
			}
			
			tilemap[tx + ty*tile_w] = tile_idx;
		}
	}
	
	printf("tile count: %d\n", tileset_count);
	
			// printf("tile: (%d, %d)\n", tx, ty);
			// for(uint y = 0; y < 8; y++){
			// 	for(uint x = 0; x < 8; x++) printf("%02X ", tile[x + 8*y]);
			// 	printf("\n");
			// }
	return EXIT_SUCCESS;
}
