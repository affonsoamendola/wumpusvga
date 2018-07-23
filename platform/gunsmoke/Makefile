MAME = ~/mame/gunsmoke_64
ROMPATH = ~/nonfree/mame
Z80ASSEMBLER = sdasz80
Z80COMPILER = sdcc
OBJCOPY = objcopy
CC = gcc -std=gnu99
PCB_VARIANT = gunsmoke
ROMSET = 3.ic85 gs03.09n gs01.11f

all: $(ROMSET)

run: $(ROMSET)
	$(MAME) -rp $(ROMPATH) $(PCB_VARIANT) -window

debug: $(ROMSET)
	$(MAME) -rp $(ROMPATH) $(PCB_VARIANT) -window -debug

boot.rel: boot.s
	$(Z80ASSEMBLER) -o boot.rel boot.s

wumpus.xpm: wumpus.xcf
	xcf2png wumpus.xcf -o wumpus.png
	convert wumpus.png wumpus.xpm

patch_CGROM: wumpus.xpm patch_CGROM.c char_codes.h
	$(CC) patch_CGROM.c -o patch_CGROM

gs01.11f: patch_CGROM
	cp $(ROMPATH)/gunsmoke/11f_gs01.bin gs01.11f
	./patch_CGROM
	cp gs01.11f $(ROMPATH)/gunsmoke/

gunsmoke.rel: gunsmoke.c char_codes.h
	$(Z80COMPILER) -mz80 -I ../.. -c gunsmoke.c

wumpus_maincpu.ihex: ../../wumpus.c gunsmoke.rel char_codes.h boot.rel
	$(Z80COMPILER) -mz80 -I ../.. -c ../../wumpus.c
	$(Z80COMPILER) -mz80 -o wumpus_maincpu.ihex wumpus.rel gunsmoke.rel boot.rel --code-loc 0x0080 --data-loc 0xE000 --no-std-crt0

3.ic85: wumpus_maincpu.ihex
	$(OBJCOPY) wumpus_maincpu.ihex -I ihex -O binary 3.ic85
	cp 3.ic85 $(ROMPATH)/gunsmoke

gs03.09n: wumpus_maincpu.ihex
	$(OBJCOPY) wumpus_maincpu.ihex -I ihex -O binary gs03.09n
	cp gs03.09n $(ROMPATH)/gunsmoke

clean:
	rm -f $(ROMSET)
	rm -f patch_CGROM
	rm -f *.rel *.ihex *.map *.sym *.lnk *.lst *.asm *.png *.xpm *.bin *.noi *.lk *.mem
