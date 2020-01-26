.macpack generic

.include "zeropage.inc"
.import incsp1, incsp2
.import __hextab, pusha

.include "pixler.inc"
.importzp px_sprite_cursor

.zeropage

; TODO export as u8 and u16?
.exportzp _ix, _iy, _idx, _tmp
_ix: .word 0
_iy: .word 0
_idx: .word 0
_tmp: .word 0

.code

.export _tmp_sprite
.proc _tmp_sprite ; u8 x, u8 y
	ldx px_sprite_cursor
	
	; Store chr.
	sta OAM_CHR+ 0, x
	sta OAM_CHR+ 4, x
	sta OAM_CHR+ 8, x
	sta OAM_CHR+12, x
	
	; Store attr.
	lda #0
	sta OAM_ATTR+ 0, x
	sta OAM_ATTR+ 4, x
	sta OAM_ATTR+ 8, x
	sta OAM_ATTR+12, x
	
	; Store x-values.
	ldy #1
	lda (sp), y
	sta OAM_X+ 0, x
	sta OAM_X+ 8, x
	add #8
	sta OAM_X+ 4, x
	sta OAM_X+12, x
	
	; Store y-values.
	ldy #0
	lda (sp), y
	sta OAM_Y+ 0, x
	sta OAM_Y+ 4, x
	add #8
	sta OAM_Y+ 8, x
	sta OAM_Y+12, x
	
	; Increment sprite cursor.
	txa
	add #16
	sta px_sprite_cursor
	
	jmp incsp2
.endproc
