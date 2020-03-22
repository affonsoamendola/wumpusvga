obj = wumpus.obj vga.obj fonflib.obj
bin = wumpus.exe

CC = wcc
CFLAGS = -zq -otx
LD = wlink
INCLUDE = include
LIB = /usr/share/openwatcom/lib286/dos/:/usr/share/openwatcom/lib286

$(bin): $(obj)
	$(LD) name $@ file { $(obj) } libpath $(LIB) form dos

.c.obj:
	$(CC) -i$(INCLUDE) -fo=$@ $(CFLAGS) $<

clean: .symbolic
	rm *.obj
	rm *.err
	rm $(bin)

run: .symbolic
	dosbox $(bin)