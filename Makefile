# Set the compiling tools to use (NOTE: gcc will not work)
CC = g++
RC = windres
# Change batch.exe to whatever you want your executable to be called
EXE = batch.exe
# Change batch.bat to whatever your batch file is called
BATCH = batch.bat
# Change the password to whatever you want
PASSWORD = "Aal izz well"
# Flags when compiling
FLAGS = -w

SHELL = powershell.exe

.PHONY: clean all

all:: ${EXE}

# Compile the main body c with the resource to produce the final exe
${EXE}:  batch.o batch.res decrypt.o
	@Write-Host ">>> " -NoNewline -ForegroundColor Yellow; Write-Host "Building ${EXE}" -ForegroundColor Green
	@${CC} batch.o batch.res decrypt.o -o ${EXE}

# Where all the magic happens
batch.res:  batch.rc icon.ico encrypted-batch
	@Write-Host ">>> " -NoNewline -ForegroundColor Yellow; Write-Host "Building batch.res" -ForegroundColor Blue
	@${RC} batch.rc -O coff -o batch.res

# Deletes all compiled files
clean: 
	@Write-Host ">>> " -NoNewline -ForegroundColor Yellow; Write-Host "Cleaning..." -ForegroundColor Magenta
	@if (Test-Path "${EXE}")          { Remove-Item ${EXE} }
	@if (Test-Path "batch.res")       { Remove-Item batch.res }
	@if (Test-Path "batch.o")         { Remove-Item batch.o }
	@if (Test-Path "decrypt.o")       { Remove-Item decrypt.o }
	@if (Test-Path "encrypt.exe")     { Remove-Item encrypt.exe }
	@if (Test-Path "encrypted-batch") { Remove-Item encrypted-batch }
	@Write-Host ">>> " -NoNewline -ForegroundColor Yellow; Write-Host "Done!" -ForegroundColor Green

# Compile the c separate from the resource to make compilation quick when you don't change the c
batch.o:  batch.c
	@Write-Host ">>> " -NoNewline -ForegroundColor Yellow; Write-Host "Building batch.c" -ForegroundColor Blue
	@${CC} batch.c -c -o batch.o ${FLAGS} -DPASSWORD='${PASSWORD}'

# Encrypt the batch before saving to the resource
encrypted-batch:  ${BATCH} encrypt.exe
	@Write-Host ">>> " -NoNewline -ForegroundColor Yellow; Write-Host "Encrypting the batch file" -ForegroundColor Blue
	@./encrypt.exe ${BATCH} encrypted-batch ${PASSWORD}

# Compile the windows sample code for encrypting a file
encrypt.exe:  encrypt.cpp
	@Write-Host ">>> " -NoNewline -ForegroundColor Yellow; Write-Host "Building encrypt.exe" -ForegroundColor Blue
	@${CC} encrypt.cpp -o encrypt.exe -ladvapi32 ${FLAGS}

decrypt.o:  decrypt.cpp
	@Write-Host ">>> " -NoNewline -ForegroundColor Yellow; Write-Host "Building decrypt.o" -ForegroundColor Blue
	@${CC} decrypt.cpp -c -o decrypt.o -ladvapi32 ${FLAGS}