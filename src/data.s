.macro inclz4 symbol, file
	.export symbol
	symbol:
		.incbin file, 8
		.word 0 ; terminator
.endmacro

.segment "PRG0"

inclz4 _SPRITES_CHR, "chr/Sprites.chr.lz4"
inclz4 _GAMEPLAY_CHR, "chr/Tileset.chr.lz4"

.export _GAMEPLAY_PALETTE
_GAMEPLAY_PALETTE:
	.incbin "chr/Tileset.pal"
	.incbin "chr/Sprites.pal"

inclz4 _GAMEPLAY_TILEMAP, "chr/Tileset.nam.lz4"

inclz4 _TITLE_CHR, "chr/TitleScreen.chr.lz4"

.export _TITLE_PALETTE
_TITLE_PALETTE:
	.incbin "chr/TitleScreen.pal"
	.incbin "chr/TitleScreen.pal"

inclz4 _TITLE_TILEMAP, "chr/TitleScreen.nam.lz4"

inclz4 _PLAYERSELECT_CHR, "chr/CharacterSelect.chr.lz4"

.export _PLAYERSELECT_PALETTE
_PLAYERSELECT_PALETTE:
	.incbin "chr/CharacterSelect.pal"
	.incbin "chr/CharacterSelect.pal"

inclz4 _PLAYERSELECT_TILEMAP_11, "chr/CharacterSelect11.nam.lz4"
inclz4 _PLAYERSELECT_TILEMAP_12, "chr/CharacterSelect12.nam.lz4"
inclz4 _PLAYERSELECT_TILEMAP_21, "chr/CharacterSelect21.nam.lz4"
inclz4 _PLAYERSELECT_TILEMAP_22, "chr/CharacterSelect22.nam.lz4"
