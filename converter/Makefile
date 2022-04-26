# Compile the main body c with the resource to produce the final exe
batch.exe:  batch.o batch.res
	g++ batch.o batch.res -o batch.exe

# Where all the magic happens
batch.res:  batch.rc icon.ico batch.bat Makefile
	windres batch.rc -O coff -o batch.res

# Deletes all compiled files, NOTE: This bit doesn't work with cygwin
clean: 
	if exist batch.exe del batch.exe
	if exist batch.res del batch.res
	if exist batch.o del batch.o

# Compile the c separate from the resource to make compilation quick when you don't change the c
batch.o:  batch.c Makefile
	g++ batch.c -c -o batch.o