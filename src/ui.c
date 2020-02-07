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
   "Ernest",
   "Bianca"
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

	music_stop();
   
   Player1 = MAYOR;
   Player2 = DEPUTY;
   
	px_ppu_sync_disable();{
      PX.scroll_x = 0;
      PX.scroll_y = 0;
      px_inc_h();
      // Load the chr into ram.
      px_lz4_to_vram(CHR_ADDR(0,0), SPRITES_CHR);
      px_lz4_to_vram(CHR_ADDR(1,0), PLAYERSELECT_CHR);
      if ( NumPlayers == 2 )
      {
         px_lz4_to_vram(NT_ADDR(0, 0, 0), PLAYERSELECT_TILEMAP_21);
         px_lz4_to_vram(NT_ADDR(1, 0, 0), PLAYERSELECT_TILEMAP_22);         
      }
      else
      {
         px_lz4_to_vram(NT_ADDR(0, 0, 0), PLAYERSELECT_TILEMAP_11);
         px_lz4_to_vram(NT_ADDR(1, 0, 0), PLAYERSELECT_TILEMAP_12);         
      }
		
		PX.scroll_x = 0;
		PX.scroll_y = 0;
		px_spr_clear();
	} px_ppu_sync_enable();
	
   fade_from_black(PLAYERSELECT_PALETTE,4);
	
	while(true){
		read_gamepads();
      
		if(JOY_START(pad1.press | pad2.press)) break;
		
      if(JOY_LEFT(pad1.press) || JOY_RIGHT(pad1.press))
      {
         Player1 = !Player1;
         Player2 = !Player2;
         PX.scroll_x += 256;
      }
      
		px_spr_end();
		px_wait_nmi();
	}
	
	sound_play(SOUND_MATCH);
	
	fade_to_black(PLAYERSELECT_PALETTE, 4);

   return gameplay_screen(240, 1);
}

Gamestate lose_screen(void){
	u8 timeout = 60;
	bool show_start = true;
	
	music_stop();
	
	px_ppu_sync_disable();{
      PX.scroll_x = 0;
      PX.scroll_y = 0;
		
		px_addr(NT_ADDR(0, 0, 0));
		px_fill(1024, 0);
		
		px_addr(NT_ADDR(0, 11, 14));
		px_str("YOU LOST!");
		
		PX.scroll_x = 0;
		PX.scroll_y = 0;
		px_spr_clear();
	} px_ppu_sync_enable();
	
   fade_from_black(PALETTE,4);

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

Gamestate win_screen(u8 difficulty, u8 level){
	u8 timeout = 60;
	bool show_start = true;
	music_stop();
	
	px_ppu_sync_disable();{
      PX.scroll_x = 0;
      PX.scroll_y = 0;
		
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
	
   fade_from_black(PALETTE,4);

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
      
   fade_to_black(PALETTE,4);

  return gameplay_screen(difficulty - difficulty/4, level + 1);
}
