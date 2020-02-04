#include <stdlib.h>
#include <string.h>

#include "pixler.h"
#include "common.h"
#include "main.h"

const u8 PALETTE[] = {
	BG_COLOR, 0x00, 0x10, 0x20,
	BG_COLOR, 0x06, 0x16, 0x26,
	BG_COLOR, 0x09, 0x19, 0x29,
	BG_COLOR, 0x01, 0x11, 0x21,
	
	BG_COLOR, 0x00, 0x10, 0x20,
	BG_COLOR, 0x06, 0x16, 0x26,
	BG_COLOR, 0x09, 0x19, 0x29,
	BG_COLOR, 0x01, 0x11, 0x21,
};

const u8* OFFICIALS[] = {
   "MAYOR ",
   "DEPUTY"
};

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

void fade_to_black(const u8* palette, u8 delay){
	darken(palette, 1);
	px_wait_frames(delay);
	darken(palette, 2);
	px_wait_frames(delay);
	darken(palette, 3);
	px_wait_frames(delay);
	darken(palette, 4);
}

static void blit_string(u16 addr, const char* str){
	px_buffer_blit(addr, str, strlen(str));
}

Gamestate player_select_screen(){
	u8 timeout = 60;
	bool show_start = true;
#define TLBR 134
#define L 133
#define TRBL 137
#define B 140
#define R 138
#define T 131
   const u8 tr[] = {TLBR,T,T,T,T,T,T,T,T,TRBL,0,0,TLBR,T,T,T,T,T,T,T,T,TRBL};
   const u8 fr[] = {L,0,0,0,0,0,0,0,0,R,0,0,L,0,0,0,0,0,0,0,0,R};
   const u8 mr[] = {L,0,0,0,0,0,0,0,0,R,0,0,L,0,0,0,0,0,0,0,0,R};
   const u8 br[] = {TRBL,B,B,B,B,B,B,B,B,TLBR,0,0,TRBL,B,B,B,B,B,B,B,B,TLBR};
#undef TLBR
#undef L
#undef TRBL
#undef B
#undef R
#undef T
	music_stop();
   
	px_ppu_sync_disable();{
		px_addr(NT_ADDR(0, 0, 0));
		px_fill(1024, 0);
		
		px_addr(NT_ADDR(0, 8, 8));
		px_str("SELECT OFFICIAL");
      if ( NumPlayers == 2 )
      {
         px_str("S");
      }
		
		px_addr(NT_ADDR(0, 5, 12));
		px_blit(22, tr);
		px_addr(NT_ADDR(0, 5, 13));
		px_blit(22, mr);
		px_addr(NT_ADDR(0, 5, 14));
		px_blit(22, mr);
		px_addr(NT_ADDR(0, 5, 15));
		px_blit(22, mr);
		px_addr(NT_ADDR(0, 5, 16));
		px_blit(22, mr);
		px_addr(NT_ADDR(0, 5, 17));
		px_blit(22, mr);
		px_addr(NT_ADDR(0, 5, 18));
		px_blit(22, mr);
		px_addr(NT_ADDR(0, 5, 19));
		px_blit(22, mr);
		px_addr(NT_ADDR(0, 5, 20));
		px_blit(22, mr);
		px_addr(NT_ADDR(0, 5, 21));
		px_blit(22, br);
      px_addr(NT_ADDR(0, 6, 18));
      px_fill(8, 13);
      px_addr(NT_ADDR(0, 6, 19));
      px_fill(8, 13);
      px_addr(NT_ADDR(0, 6, 20));
      px_fill(8, 13);
      if ( NumPlayers == 2 )
      {
         px_addr(NT_ADDR(0, 18, 18));
         px_fill(8, 13);
         px_addr(NT_ADDR(0, 18, 19));
         px_fill(8, 13);
         px_addr(NT_ADDR(0, 18, 20));
         px_fill(8, 13);
      }
		
		PX.scroll_x = 0;
		PX.scroll_y = 0;
		px_spr_clear();
	} px_ppu_sync_enable();
	
   fade_from_black(GAMEPLAY_PALETTE,4);
	
	while(true){
		read_gamepads();
      meta_spr(80, 136, 0, metasprite_list[(Player1*16)]);
      px_spr(44, 132, 1, '<');
      px_spr(108, 132, 1, '>');
      if ( NumPlayers == 2 )
      {
         meta_spr(176, 136, 0, metasprite_list[(Player2*16)]);
         px_spr(140, 132, 1, '<');
         px_spr(204, 132, 1, '>');
      }
      blit_string(NT_ADDR(0,7,22), OFFICIALS[Player1]);
      if ( NumPlayers == 2 )
      {
         blit_string(NT_ADDR(0,19,22), OFFICIALS[Player2]);
      }         
      
		if(JOY_START(pad1.press | pad2.press)) break;
		
      if(JOY_LEFT(pad1.press) || JOY_RIGHT(pad1.press))
      {
         Player1 = !Player1;
      }
		
      if(JOY_LEFT(pad2.press) || JOY_RIGHT(pad2.press))
      {
         Player2 = !Player2;
      }
      
		if(--timeout == 0){
			show_start = !show_start;
			timeout = 30;
			blit_string(NT_ADDR(0, 10, 25), show_start ? "press start" : "           ");
		}
      
		px_spr_end();
		px_wait_nmi();
	}
	
	sound_play(SOUND_MATCH);
	
	fade_to_black(PALETTE, 4);

   return gameplay_screen(240, 1);
}

Gamestate lose_screen(void){
	u8 timeout = 60;
	bool show_start = true;
	
	music_stop();
	
	px_ppu_sync_disable();{
		px_buffer_blit(PAL_ADDR, PALETTE, sizeof(PALETTE));
		
		px_addr(NT_ADDR(0, 0, 0));
		px_fill(1024, 0);
		
		px_addr(NT_ADDR(0, 11, 14));
		px_str("YOU LOST!");
		
		PX.scroll_x = 0;
		PX.scroll_y = 0;
		px_spr_clear();
	} px_ppu_sync_enable();
	
	while(true){
		read_gamepads();
		if(JOY_START(pad1.press | pad2.press)) break;
		
		if(--timeout == 0){
			show_start = !show_start;
			timeout = 30;
			blit_string(NT_ADDR(0, 10, 16), show_start ? "press start" : "           ");
		}
		
		px_wait_nmi();
	}
	
	sound_play(SOUND_MATCH);
	
	fade_to_black(PALETTE, 4);
  return splash_screen();
}

Gamestate ultimate_win_screen(void){
	u8 timeout = 60;
	bool show_start = true;
	
	music_stop();
	
	px_ppu_sync_disable();{
		px_buffer_blit(PAL_ADDR, PALETTE, sizeof(PALETTE));
		
		px_addr(NT_ADDR(0, 0, 0));
		px_fill(1024, 0);
		
		px_addr(NT_ADDR(0, 3, 14));
		px_str("You're the world's superest");
		px_addr(NT_ADDR(0, 10, 15));
		px_str("mayor! Congrats!");
		
		px_addr(NT_ADDR(0, 10, 18));
		px_str("press start");
		
		PX.scroll_x = 0;
		PX.scroll_y = 0;
		px_spr_clear();
	} px_ppu_sync_enable();
	
	while(true){
		read_gamepads();
		if(JOY_START(pad1.press | pad2.press)) break;
		
		if(--timeout == 0){
			show_start = !show_start;
			timeout = 30;
			blit_string(NT_ADDR(0, 10, 18), show_start ? "press start" : "           ");
		}
		
		px_wait_nmi();
	}
	
	sound_play(SOUND_MATCH);
  return splash_screen();
}

Gamestate win_screen(u8 difficulty, u8 level){
	u8 timeout = 60;
	bool show_start = true;
	music_stop();
	
	px_ppu_sync_disable();{
		px_buffer_blit(PAL_ADDR, PALETTE, sizeof(PALETTE));
		
		px_addr(NT_ADDR(0, 0, 0));
		px_fill(1024, 0);
		
		px_addr(NT_ADDR(0, 6, 14));
		px_str("Your approval rating");
		px_addr(NT_ADDR(0, 10, 15));
		px_str("is improving.");
		
		px_addr(NT_ADDR(0, 10, 18));
		px_str("press start");
		
		PX.scroll_x = 0;
		PX.scroll_y = 0;
		px_spr_clear();
	} px_ppu_sync_enable();
	
	while(true){
		read_gamepads();
		if(JOY_START(pad1.press | pad2.press)) break;
		
		if(--timeout == 0){
			show_start = !show_start;
			timeout = 30;
			blit_string(NT_ADDR(0, 10, 18), show_start ? "press start" : "           ");
		}
		
		px_wait_nmi();
	}
	
	sound_play(SOUND_MATCH);
  return gameplay_screen(difficulty - difficulty/4, level + 1);
}
