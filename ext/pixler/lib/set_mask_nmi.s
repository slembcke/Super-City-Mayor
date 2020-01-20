.importzp px_mask
.import _px_wait_nmi

.export _px_set_mask_nmi
.proc _px_set_mask_nmi; u8 mask
	sta px_mask
	jsr _px_wait_nmi
	
	rts
.endproc
