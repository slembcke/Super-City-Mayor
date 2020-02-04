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
	.incbin "chr/Sprites.chr",0,4096
	.incbin "chr/Tileset.chr",0,4096

.export _GAMEPLAY_PALETTE
_GAMEPLAY_PALETTE:
	.incbin "chr/Tileset.pal", 0, 16
	.incbin "chr/Sprites.pal", 0, 16

.export _GAMEPLAY_TILEMAP
_GAMEPLAY_TILEMAP:
	.incbin "chr/Tileset.nam"

inclz4 _MAP_SPLASH, "map/splash.lz4"
