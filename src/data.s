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
.incbin "chr/Tileset.chr"

inclz4 _MAP_SPLASH, "map/splash.lz4"
