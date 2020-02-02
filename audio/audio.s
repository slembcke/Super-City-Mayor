.import FamiToneInit
.import FamiToneSfxInit
.import FamiToneUpdate
.import FamiToneMusicPlay
.import FamiToneMusicPause
.import FamiToneMusicStop
.import FamiToneSfxPlay

.import px_uxrom_select

; .code

.export _music_init
.proc _music_init ; u16 addr
	; ax -> xy
	pha
	txa
	tay
	pla
	tax
	lda #1 ; TODO Hardcoded NTSC
	jsr FamiToneInit
	
	rts
.endproc

.export _sound_init
.proc _sound_init ; u16 addr
	; ax -> xy
	pha
	txa
	tay
	pla
	tax
	jsr FamiToneSfxInit
	
	rts
.endproc

.export _music_play
.proc _music_play
	pha
	ldy #1
	jsr px_uxrom_select
	
	pla
	jsr FamiToneMusicPlay
	
	ldy #0
	jmp px_uxrom_select
.endproc

.export _music_pause = FamiToneMusicPause
.export _music_stop = FamiToneMusicStop
.export _sound_play = FamiToneSfxPlay

.export _px_nmi_callback
.proc _px_nmi_callback
	ldy #1
	jsr px_uxrom_select
	
	jsr FamiToneUpdate
	
	ldy #0
	jmp px_uxrom_select
.endproc

.segment "PRG1"

FT_DPCM_PTR = (FT_DPCM_OFF & $3fff) >> 6
FT_DPCM_OFF:
;	.incbin "after_the_rain.dmc"

.export _MUSIC
_MUSIC:
	.include "after_the_rain.s"

.export _SOUNDS
_SOUNDS:
	.include "sounds.s"
