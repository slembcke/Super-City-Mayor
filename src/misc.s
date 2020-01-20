.include "zeropage.inc"

.import __hextab, pusha

.import _px_buffer_data
.importzp PX_buffer

.zeropage

; TODO export as u8 and u16?
.exportzp _ix, _iy, _idx, _tmp
_ix: .word 0
_iy: .word 0
_idx: .word 0
_tmp: .word 0

.code

.proc hex_digit
	pha
	lsr
	lsr
	lsr
	lsr
	tax
	lda __hextab, x
	sta (PX_buffer), y
	iny
	
	pla
	and #$F
	tax
	lda __hextab, x
	sta (PX_buffer), y
	iny
	
	rts
.endproc

.export _debug_hex
.proc _debug_hex
	addr = $2000 + 1 + 1*32
	
	sta sreg+0
	stx sreg+1
	
	lda #4
	jsr pusha
	lda #<addr
	ldx #>addr
	jsr _px_buffer_data
	
	
	ldy #0
	lda sreg+1
	jsr hex_digit
	lda sreg+0
	jsr hex_digit
	
	rts
.endproc
