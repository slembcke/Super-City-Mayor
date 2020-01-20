.include "zeropage.inc"
.include "pixler.inc"

.import popax

.export _px_str
.proc _px_str ; char *str
	; Store address to ptr1.
	sta ptr1 + 0
	stx ptr1 + 1
	
	ldy #0
	:	lda (ptr1), y
		beq @break
		sta PPU_VRAM_IO
		iny
		bne :-
	@break:
	
	rts
.endproc
