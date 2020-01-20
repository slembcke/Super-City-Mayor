.include "zeropage.inc"

.import __hextab, pusha

.import _px_buffer_data
.importzp PX_buffer

.data

.export _px_debug_hex_addr
_px_debug_hex_addr:
	.addr $2021

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

.export _px_debug_hex
.proc _px_debug_hex
	sta sreg+0
	stx sreg+1
	
	lda #4
	jsr pusha
	lda _px_debug_hex_addr+0
	ldx _px_debug_hex_addr+1
	jsr _px_buffer_data
	
	
	ldy #0
	lda sreg+1
	jsr hex_digit
	lda sreg+0
	jsr hex_digit
	
	rts
.endproc
