PROJECT_NAME = GGJ20
ROM = $(PROJECT_NAME).nes

CC = $(CC65_ROOT)/cc65
CL = $(CC65_ROOT)/cl65
AS = $(CC65_ROOT)/ca65
LD = $(CC65_ROOT)/ld65

CFLAGS += \
	-t nes -Oirs \
	--register-space 16 \
	-I ext/pixler/lib \

ASMINC = \
	-I ext/pixler/lib \

SRC = \
	src/main.c \
	src/town.c \

ASM = \
	src/data.s \
	src/misc.s \
	audio/audio.s \
	ext/famitone2/famitone2.s \
	ext/pixler/lib/set_mask_nmi.s \
	ext/pixler/lib/coroutine.s \
	ext/pixler/lib/rand8.s \
	ext/pixler/lib/boot.s \
	ext/pixler/lib/zeropage.s \
	ext/pixler/lib/nmi.s \
	ext/pixler/lib/wait_frames.s \
	ext/pixler/lib/uxrom.s \
	ext/pixler/lib/buffer.s \
	ext/pixler/lib/buffer_inc.s \
	ext/pixler/lib/buffer_color.s \
	ext/pixler/lib/buffer_data.s \
	ext/pixler/lib/buffer_blit.s \
	ext/pixler/lib/inc.s \
	ext/pixler/lib/fill.s \
	ext/pixler/lib/blit.s \
	ext/pixler/lib/str.s \
	ext/pixler/lib/sprite.s \
	ext/pixler/lib/lz4.s \
	ext/pixler/lib/lz4_to_vram.s \
	ext/pixler/lib/lz4_to_ram.s \
	ext/pixler/lib/profile.s \
	ext/pixler/lib/debug_hex.s \

OBJS = \
	$(SRC:.c=.o) \
	$(ASM:.s=.o) \

CHR = \
	chr/0.png \

SONGS = \
	audio/after_the_rain.txt \

default: $(ROM)
rom: $(ROM)

PX_TOOLS_PATH = ext/pixler/tools

px-tools:
	make -C $(PX_TOOLS_PATH)
	touch px-tools

FT2_TOOLS_PATH = ext/famitone2
ft2-tools:
	make -C $(FT2_TOOLS_PATH)
	touch ft2-tools

run-linux: rom
	mesen $(ROM)

run-win: rom
	../Mesen/Mesen.exe $(ROM)

$(ROM): ld65.cfg $(OBJS) $(PX_LIB)
	$(LD) -C ld65.cfg --dbgfile $(ROM:.nes=.dbg) $(OBJS) $(PX_LIB) nes.lib -m link.log -o $@

%.s: %.c
	$(CC) -g $(CFLAGS) $< --add-source $(INCLUDE) -o $@

%.s %.o: %.c
	$(CL) -c -g $(CFLAGS) $(INCLUDE) $< -o $@

%.o: %.s
	$(AS) -g $< $(ASMINC) -o $@

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

audio/sounds.s: audio/sounds.nsf
	$(FT2_TOOLS_PATH)/nsf2data $< -ca65 -ntsc

audio/%.s: audio/%.txt ft2-tools
	$(FT2_TOOLS_PATH)/text2data -ca65 $<

audio/audio.o: $(SONGS:.txt=.s) audio/sounds.s

clean:
	-rm $(ROM) $(OBJS)

.phony: default rom tiles clean
