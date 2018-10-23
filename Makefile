#Copyright Affonso Amendola 2018
#WumpusVGA

GAMENAME = wumpus
CC = tcc
RM = rm -f
CP = cp

INCLUDE = include;D:\tc\include
LIB = D:\tc\lib
CCFLAGS = 

all: $(GAMENAME).exe

$(GAMENAME).exe:
	$(CC) "-I$(INCLUDE) -L$(LIB) -e$(GAMENAME) $(CCFLAGS)"  *.c 
	
release: $(GAMENAME).exe
	mkdir ./release
	mkdir ./release/graphix
	mkdir ./release/plt
	$(CP) *.EXE ./release/
	$(CP) -r ./graphix/* ./release/graphix/
	$(CP) -r ./plt/* ./release/plt/
	make clean

run:
	dosbox -conf ~/.dosbox/tcc.conf -c "$(GAMENAME)"

clean:
	$(RM) *.OBJ *.EXE *.LOG *.BAT

clean-all: clean
	$(RM) -r ./release 