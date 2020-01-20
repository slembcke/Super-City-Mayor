.macro inclz4 symbol, file
	.export symbol
	symbol:
		.incbin file, 8
		.word 0 ; terminator
.endmacro

.rodata

inclz4 _CHR0, "chr/0.lz4"
inclz4 _MAP_SPLASH, "map/splash.lz4"
