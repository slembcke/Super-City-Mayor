.macro inclz4 symbol, file
	.export symbol
	symbol:
		.incbin file, 8
		.word 0 ; terminator
.endmacro

.segment "PRG0"

.export _CHR0
_CHR0:
.incbin "chr/0.chr"

.export _GAMEPLAY_CHR
_GAMEPLAY_CHR:
	.incbin "chr/Tileset.chr"

.export _GAMEPLAY_PALETTE
_GAMEPLAY_PALETTE:
	.incbin "chr/Tileset.pal", 0, 16
; TODO! Temporary sprite palettes
BG_COLOR = $2D
	.byte BG_COLOR, $00, $10, $20
	.byte BG_COLOR, $06, $16, $26
	.byte BG_COLOR, $09, $19, $29
	.byte BG_COLOR, $01, $11, $21


.export _GAMEPLAY_TILEMAP
_GAMEPLAY_TILEMAP:
	.incbin "chr/Tileset.nam"

inclz4 _MAP_SPLASH, "map/splash.lz4"
