CC65_ROOT = ~/Development/NES/cc65/bin
include Makefile

run-mac: rom
	open -a "Nestopia.app" $(ROM)
