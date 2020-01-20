PROJECT_NAME = GGJ20
ROM = $(PROJECT_NAME).nes

CC65_ROOT = `pwd`/../cc65
CC = $(CC65_ROOT)/bin/cc65
AS = $(CC65_ROOT)/bin/ca65
LD = $(CC65_ROOT)/bin/ld65

CFLAGS += -t nes -Oirs --register-space 16 -I ext/pixler/lib

SRC = \
	src/main.c \

ASM = \
	src/data.s \
	src/misc.s \
	audio/audio.s \
	ext/famitone2/famitone2.s \

OBJS = $(SRC:.c=.o) $(ASM:.s=.o)

CHR = \
	chr/0.png \

SONGS = \
	audio/after_the_rain.txt \


default: $(ROM)


PX_TOOLS_PATH = ext/pixler/tools
PX_LIB_PATH = ext/pixler/lib
PX_LIB = $(PX_LIB_PATH)/px.lib
$(PX_LIB):
	make CC65_ROOT=$(CC65_ROOT) -C $(PX_LIB_PATH)

px-tools:
	make -C $(PX_TOOLS_PATH)
	touch px-tools

FT2_TOOLS_PATH = ext/famitone2
ft2-tools:
	make -C $(FT2_TOOLS_PATH)
	touch ft2-tools

rom: $(ROM)

run-mac: rom
	open -a Nestopia $(ROM)

run-linux: rom
	nestopia -w -l 1 -n -s 2 -t $(ROM)

run-win: rom
	../Mesen/Mesen.exe $(ROM)

$(ROM): ld65.cfg $(OBJS) $(PX_LIB)
	$(LD) -C ld65.cfg $(OBJS) $(PX_LIB) nes.lib -m link.log -o $@
	
%.s: %.c
	$(CC) $(CFLAGS) $< --add-source $(INCLUDE) -o $@

%.o: %.s
	$(AS) $< $(ASMINC) -o $@

%.chr: %.png px-tools
	$(PX_TOOLS_PATH)/png2chr $< $@

%.lz4: %.chr px-tools
	$(PX_TOOLS_PATH)/lz4x -f9 $< $@
	touch $@

%.bin: %.tmx
	python $(PX_TOOLS_PATH)/tmx2bin.py $< > $@

%.lz4: %.bin px-tools
	$(PX_TOOLS_PATH)/lz4x -f9 $< $@
	touch $@

src/data.o: $(CHR:.png=.lz4) map/splash.lz4

tiles: chr/0.chr
	$(PX_TOOLS_PATH)/chr2png "1D 00 10 20" chr/0.chr chr/0-pal0.png
	$(PX_TOOLS_PATH)/chr2png "1D 06 16 26" chr/0.chr chr/0-pal1.png
	$(PX_TOOLS_PATH)/chr2png "1D 09 19 29" chr/0.chr chr/0-pal2.png
	$(PX_TOOLS_PATH)/chr2png "1D 01 11 21" chr/0.chr chr/0-pal3.png

audio/sounds.s: audio/sounds.nsf ft2-tools
	$(FT2_TOOLS_PATH)/nsf2data $< -ca65 -ntsc

audio/%.s: audio/%.txt ft2-tools
	$(FT2_TOOLS_PATH)/text2data -ca65 $<

audio/audio.o: $(SONGS:.txt=.s) audio/sounds.s

clean:
	-rm $(OBJS) $(CHR:.png=.chr) $(CHR:.png=.lz4)
	-rm $(SONGS:.txt=.s)
	-rm px-tools
	make -C $(PX_TOOLS_PATH) clean
	make -C $(PX_LIB_PATH) clean
	-rm ft2-tools
	make -C $(FT2_TOOLS_PATH) clean

.phony: default rom tiles clean

# Cancel built in rule for .c files.
%.o: %.c
