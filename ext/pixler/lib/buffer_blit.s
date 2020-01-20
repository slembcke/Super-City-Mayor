.include "zeropage.inc"
.import pusha, popax

.importzp PX_buffer
.import _px_buffer_data

.export _px_buffer_blit
.proc _px_buffer_blit; u16 addr, void *src, u8 len
	; px_buffer_data(sizeof(PAL_SPLASH), PAL_ADDR);
	; memcpy(PX.buffer, PAL_SPLASH, sizeof(PAL_SPLASH));
	src = ptr1
	dst = PX_buffer
	len = tmp1
	
	sta len
	
	jsr popax
	sta src+0
	stx src+1
	
	jsr popax
	sta ptr2+0
	stx ptr2+1
	
	lda len
	jsr pusha
	lda ptr2+0
	ldx ptr2+1
	jsr _px_buffer_data
	
	ldy len
	dey
	:	lda (src), y
		sta (dst), y
		dey
		bpl :-
	
	rts
.endproc
