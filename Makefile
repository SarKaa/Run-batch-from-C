# Set the compiling tools to use (NOTE: gcc will not work)
CC = g++
RC = windres
# Change batch.exe to whatever you want your executable to be called
EXE = batch.exe
# Change batch.bat to whatever your batch file is called
BATCH = batch.bat
# Change the password to whatever you want, as long as you change it in batch.c too
PASSWORD = "Aal izz well"

# Compile the main body c with the resource to produce the final exe
${EXE}:  batch.o batch.res
	${CC} batch.o batch.res -o ${EXE}

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
batch.o:  batch.c Makefile decrypt.h
	${CC} batch.c -c -o batch.o -fpermissive -Wwrite-strings

# Encrypt the batch before saving to the resource
encrypted-batch.txt:  ${BATCH} encrypt.exe
	encrypt.exe ${BATCH} encrypted-batch.txt ${PASSWORD}

# Compile the windows sample code for encrypting a file
encrypt.exe:  encrypt.c Makefile
	${CC} encrypt.c -o encrypt.exe -fpermissive
