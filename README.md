# GGJ20 Template

* Includes my "pixler" C library which provides:
	* Coroutines
	* NMI functions
	* PPU management
	* NMI command buffers
	* LZ4 decompression
* Famitone 2 + utilities.
* Utilities for:
	* png <--> chr conversion
	* Tiled .tmx -> binary conversion with attrib table support.
	* lz4 compression.
* Makefile + VSCode actions for builds.
* JSNES template set up and good to go for a web build.
* UXROM support.
* Starter CHR tiles from: http://wiki.nesdev.com/w/index.php/File:Jroatch-chr-sheet.chr.png

Requires cc65 to be checked out and built at the same level.
