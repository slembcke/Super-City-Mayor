#ifndef _PIXLER_H
#define _PIXLER_H

#include <stdbool.h>
#include <stdint.h>

// Convince vscode to ignore cc65 features.
#ifndef __CC65__
	#define asm(...) 0
	#define __A__ (*(uint8_t *)0)
	#define __AX__ (*(uint16_t *)0)
	#define __EAX__ (*(uint32_t *)0)
	#define __fastcall__
#endif

#include <nes.h>
#include <joystick.h>

// Some handy definitions.

typedef int8_t s8;
typedef int16_t s16;

typedef uint8_t u8;
typedef uint16_t u16;

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define CLAMP(x, min, max) MAX(min, MIN(x, max))

// NES PPU mask and control registers.

extern u8 px_mask;
#pragma zpsym("px_mask");
extern u8 px_ctrl;
#pragma zpsym("px_ctrl");

#define PX_MASK_RENDER_DISABLE 0
#define PX_MASK_GRAY 0x01
#define PX_MASK_EMPHASIS_R 0x20
#define PX_MASK_EMPHASIS_G 0x40
#define PX_MASK_EMPHASIS_B 0x80
#define PX_MASK_BG_ENABLE 0x0E
#define PX_MASK_SPRITE_ENABLE 0x10
#define PX_MASK_RENDER_ENABLE (PX_MASK_SPRITE_ENABLE | PX_MASK_BG_ENABLE)

#define PX_CTRL_VRAM_INC 0x04
#define PX_CTRL_SPR_TABLE_ADDR 0X08
#define PX_CTRL_BG_TABLE_ADDR 0x10

void px_profile_start(void);
void px_profile_end(void);

#define px_ppu_sync_enable() {px_mask |= PX_MASK_RENDER_ENABLE; px_wait_nmi();}
#define px_ppu_sync_disable() {px_mask &= ~PX_MASK_RENDER_ENABLE; px_wait_nmi();}

#define px_spr_table(tbl) {px_ctrl |= (tbl ? 0xFF : 0x00) & PX_CTRL_SPR_TABLE_ADDR; PPU.control = px_ctrl;}
#define px_bg_table(tbl) {px_ctrl |= (tbl ? 0xFF : 0x00) & PX_CTRL_BG_TABLE_ADDR; PPU.control = px_ctrl;}

// Frame counter usable for timing.

extern u8 px_ticks;
#pragma zpsym("px_ticks");

// 

typedef struct {
	u16 scroll_x, scroll_y;
	u8 * const buffer;
} PX_t;

extern PX_t PX;
#pragma zpsym("PX");

// UNROM secondary bank selection routine.
void px_uxrom_select(u8 bank);

// Macros for statically determining various PPU addresses.

// PPU Address of a CHR definition.
#define CHR_ADDR(bank, chr) (bank*0x1000 | chr*0x10)

// PPU Address of a tile in a nametable (tilemap screen).
#define NT_ADDR(tbl, x, y) (0x2000 | tbl*0x400 | (y << 5) | x)

// PPU Start address of an attribute (palette) table.
#define AT_ADDR(tbl) (0x23C0 + tbl*0x400)

// PPU Address of a color in the palette.
#define PAL_ADDR 0x3F00

// These functions are only safe to call when the PPU is disabled! 

// Set the PPU address register.
#define px_addr(addr) {	PPU.vram.address = addr >> 8; PPU.vram.address = addr & 0xFF;}

// Wait for nmi and set the PPU mask register.
void px_set_mask_nmi(u8 mask);

// Set the PPU auto-increment direction.
void px_inc_h(void);
void px_inc_v(void);

// Fill a range of PPU bytes.
void px_fill(u16 len, char chr);

// Copy a range of bytes from the CPU to the PPU.
void px_blit(u16 len, const u8 *src);

// Copies up to 256 bytes from a NULL terminated ASCII string to the PPU.
// Cannot handle newlines.
void px_str(const char *str);

// Execute buffered commands immediately. (See next section)
void px_buffer_exec(void);

// These "buffer" functions are safe to call anytime.
// Their changes do not get applied until the next NMI.
// Note that there is a limited amount of buffer memory ~100 bytes.
// There is also very limited time available in the NMI to execute commands.

// Clear the command buffer.
void px_buffer_clear(void);

// Add a command to set the PPU increment direction.
void px_buffer_inc_h(void);
void px_buffer_inc_v(void);

// Reserve space to copy bytes to the PPU.
// The buffer address is returned via PX.buffer.
void px_buffer_data(u8 len, u16 addr);

// Copy memory to the PPU.
void px_buffer_blit(u16 addr, void *src, u8 len);

// Add a command to change a PPU palette color.
void px_buffer_set_color(u8 idx, u8 color);

// These "spr" functions work on sprite memory (OAM) and are safe to call at any time.

// Clear the current list of sprites.
void px_spr_clear(void);

#define PX_SPR_BEHIND 32
#define PX_SPR_FLIPX 64
#define PX_SPR_FLIPY 128

// Draw a sprite at the given location.
void px_spr(u8 x, u8 y, u8 attr, u8 chr);

// This function should be called at the end of every frame when
// sprites are drawn to blank out the remaining sprite memory.
void px_spr_end(void);

// 

void px_wait_nmi(void);
void px_wait_frames(u8 frames);

// LZ4 decompression routines.
// The data should not include the 8 header bytes, and must be terminated by two zero bytes.
// This keeps the size down a little smaller, without requiring the compressed data size beforehand.
// Ex:
// .incbin "mydata.lz4", 8
// .word 0

void px_lz4_to_ram(void *dst, void *src);
void px_lz4_to_vram(u16 addr, void *src);

// Fast, but low quality random numbers based on a LFSR.

extern u16 rand_seed;
#pragma zpsym("rand_seed");
u8 rand8();

// Coroutines.

// Coroutine body function.
// 'value' will be the parameter passed to the px_coro_resume() call that starts the coroutine.
typedef uintptr_t (*coro_func)(uintptr_t value);

// Initialize a coroutine's stack buffer and body function.
// 7 bytes of buffer space are used for coroutine state.
// Coroutine does not begin executing until px_coro_resume() is called.
void px_coro_init(coro_func func, void *coro_buffer, u16 buffer_size);

// Execute a coroutine until it calls px_coro_yield().
// The return value is the value the coroutine passes to px_coro_yield();
uintptr_t px_coro_resume(void *coro_buffer, uintptr_t value);

// Yield from a coroutine back to the main thread.
// 'value' will be returned by the most recent call to px_coro_resume();
uintptr_t px_coro_yield(uintptr_t value);

// MARK: Misc

// Sprite memory buffer (Object Attribute Memory)
// Normally you interact with this using the px_spr*() functions.
extern u8 OAM[256];

// Display a 16 bit number in hex at px_debug_hex_addr.
extern u16 px_debug_hex_addr;
void px_debug_hex(u16 value);

#endif
