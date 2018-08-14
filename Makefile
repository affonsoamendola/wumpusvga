GAMENAME = WumpsVGA 
CC = tcc
RM = rm -f

INCLUDE = include;D:\tc\include
LIB = D:\tc\lib
CCFLAGS = 

all: $(GAMENAME).exe

$(GAMENAME).exe:
	$(CC) "-I$(INCLUDE) -L$(LIB) -e$(GAMENAME) $(CCFLAGS)"  *.c 

clean:
	$(RM) *.OBJ *.EXE