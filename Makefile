batch.exe:  batch.o batch.res
	g++ batch.o batch.res -o batch.exe

batch.res:  batch.rc icon.ico batch.bat Makefile batch.manifest
	windres batch.rc -O coff -o batch.res

clean: 
	if exist batch.exe del batch.exe
	if exist batch.res del batch.res
	if exist batch.o del batch.o

batch.o:  batch.c Makefile
	g++ batch.c -c -o batch.o