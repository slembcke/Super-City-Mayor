#include <stdlib.h>
#include <string.h>

#include "pixler.h"
#include "common.h"
#include "main.h"

u8 NumPlayers = 1;
u8 Player1 = MAYOR;
u8 Player2 = DEPUTY;

Gamestate splash_screen(void){
   static u16 c = 300;
   
   NumPlayers = 1;
   Score = 0;
   
   ix = 0;
	
	px_ppu_sync_disable();{
		// Clear nametable 0
		px_addr(NT_ADDR(0, 0, 0));
		px_fill(1024, 0);
		
		// Load the splash tilemap into nametable 1.
		px_lz4_to_vram(NT_ADDR(1, 0, 0), MAP_SPLASH);
      if ( c )
      {
         PX.scroll_x = ix;
      }
      else
      {
         PX.scroll_x = 256;
      }
      px_inc_h();
	} px_ppu_sync_enable();
	
	px_spr_end();
	fade_from_black(PALETTE, 4);
	
	while(true){
		// Randomize the seed based on player input.
    rand_seed++;
		
		read_gamepads();

      if ( c > 0 )
      {
         c -= 2;
         if ( ix < 253 )
         {
            ix += 2;
            PX.scroll_x = ix;
         }
      }
		else
      {
         px_buffer_blit(NT_ADDR(1,8,7),"SUPER CITY MAYOR",16);
         px_buffer_blit(NT_ADDR(1,12,22),"1 PLAYER",8);
         px_buffer_blit(NT_ADDR(1,12,24),"2 PLAYER",8);
         px_spr(80,  175+((NumPlayers-1)*16), 0, '>');
         if(JOY_START(pad1.press)) 
         {
            fade_to_black(PALETTE,4);
            sound_play(SOUND_JUMP);
            break;
         }
      }

		if(JOY_SELECT(pad1.press)) 
      {
         NumPlayers = 3-NumPlayers;
         sound_play(SOUND_JUMP);
      }
		if(JOY_UP(pad1.press)) 
      {
         NumPlayers = 3-NumPlayers;
         sound_play(SOUND_JUMP);
      }
		if(JOY_DOWN(pad1.press)) 
      {
         NumPlayers = 3-NumPlayers;
         sound_play(SOUND_JUMP);
      }
      
		px_spr_end();
		px_wait_nmi();
	}

   return player_select_screen();
}

void main(void){
	// Set up CC65 joystick driver.
	joy_install(nes_stdjoy_joy);
	
	// Set which tiles to use for the background and sprites.
	px_bg_table(1);
	px_spr_table(0);
	
	px_uxrom_select(1);
	sound_init(SOUNDS);
	music_init(MUSIC);
	
	px_uxrom_select(0);
	
	// Black out the palette.
	for(idx = 0; idx < 32; idx++) px_buffer_set_color(idx, 0x1D);
	px_wait_nmi();
	
	// Decompress the tileset into character memory.
//	px_lz4_to_vram(CHR_ADDR(0, 0), CHR0);
   px_addr(CHR_ADDR(0,0));
   px_blit(0x2000, GAMEPLAY_CHR);
	
	rand_seed = 31394;
	
	// Jump to the splash screen state.
	splash_screen();
}
