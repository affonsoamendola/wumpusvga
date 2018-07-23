cd ..\..\TC\BIN
TCC.EXE -DMSDOS -I..\INCLUDE;..\..\platform\msdos;..\.. -L..\LIB -e..\..\platform\msdos\whunt ..\..\platform\msdos\*.c ..\..\wumpus.c > C:\errors.txt
cd ..\..\platform\msdos
