#include <stdlib.h>
#include <string.h>

#include "pixler.h"
#include "common.h"
#include "main.h"

u8 Score = 0;
u8 NumPlayers = 1;
u8 Player1 = MAYOR;
u8 Player2 = DEPUTY;
u8 NumTerms = 1;
u8 Difficulty;
u8 Level;

Gamestate splash_screen(void){
   static const u8 selectors[2][2] = { {0x9F,0},{0,0x9F} };
   
   NumPlayers = 1;
   Score = 0;
   
	px_ppu_sync_disable();{
      PX.scroll_x = 0;
      PX.scroll_y = 0;
      px_inc_h();
      // Load the title chr into ram.
      px_lz4_to_vram(CHR_ADDR(0,0), SPRITES_CHR);
      px_lz4_to_vram(CHR_ADDR(1,0), TITLE_CHR);
      // Load the title tilemap into nametable 0.
      px_lz4_to_vram(NT_ADDR(0, 0, 0), TITLE_TILEMAP);
	} px_ppu_sync_enable();
	
	px_spr_end();
	fade_from_black(TITLE_PALETTE, 4);
	
	while(true){
		// Randomize the seed based on player input.
    rand_seed++;
		
		read_gamepads();

      px_buffer_blit(NT_ADDR(0,9,19),&selectors[NumPlayers-1][0],1);
      px_buffer_blit(NT_ADDR(0,9,21),&selectors[NumPlayers-1][1],1);
         if(JOY_START(pad1.press)) 
         {
            fade_to_black(PALETTE,4);
            sound_play(SOUND_JUMP);
            break;
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
	sound_init(&SOUNDS);
	music_init(&MUSIC);
	
	px_uxrom_select(0);
	
	// Black out the palette.
	for(idx = 0; idx < 32; idx++) px_buffer_set_color(idx, 0x1D);
	px_wait_nmi();
	
	// Decompress the tileset into character memory.
   px_addr(CHR_ADDR(0,0));
   px_blit(0x2000, GAMEPLAY_CHR);
	
	rand_seed = 31394;
	
	// Jump to the splash screen state.
	splash_screen();
}
