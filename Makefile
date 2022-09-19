# Set the compiling tools to use (NOTE: gcc will not work)
CC = g++
RC = windres
EXE = batch.exe
BATCH = batch.bat

# Compile the main body c with the resource to produce the final exe
${EXE}:  batch.o batch.res
	${CC} batch.o batch.res -o batch.exe

# Where all the magic happens
batch.res:  batch.rc icon.ico encrypted-batch.txt
	${RC} batch.rc -O coff -o batch.res

# Deletes all compiled files
clean: 
	if exist ${EXE} del ${EXE}
	if exist batch.res del batch.res
	if exist batch.o del batch.o
	if exist encrypt.exe del encrypt.exe
	if exist encrypted-batch.txt del encrypted-batch.txt

# Compile the c separate from the resource to make compilation quick when you don't change the c
batch.o:  batch.c Makefile
	${CC} batch.c -c -o batch.o -fpermissive

# Encrypt the batch before saving to the resource
encrypted-batch.txt:  ${BATCH} encrypt.exe
	encrypt ${BATCH} encrypted-batch.txt Shamballa

# Compile the windows sample code for encrypting a file
encrypt.exe:  encrypt.c Makefile
	${CC} encrypt.c -o encrypt.exe -fpermissive