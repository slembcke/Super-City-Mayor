.importzp _px_mask

.export _px_profile_start
.proc _px_profile_start
	lda _px_mask
	ora #$01
	sta _px_mask
	sta $2001
	rts
.endproc

.export _px_profile_end
.proc _px_profile_end
	lda _px_mask
	and #$FE
	sta _px_mask
	sta $2001
	rts
.endproc
