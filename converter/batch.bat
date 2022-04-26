@echo off

:convert
set /p name=App name: 
SET /p targetfile=File to convert to exe: 
if not exist %targetfile% echo ERROR: File does not exist && exit /b 1
if not "%targetfile:~-4%"==".bat" echo ERROR: File is not a batch file && exit /b 1
SET /p mingw=MINGW installation directory (without ""): 
if not exist "%mingw%\gcc.exe" echo ERROR: gcc/mingw is not installed in this location && exit /b 1
set PATH=%mingw%;%PATH%
set /P icon=App icon (.ico) to use: 
if not "%icon:~-4%"==".ico" echo ERROR: File is not an icon file && exit /b 1
set /p ver=Which version of the app is it? 
set /p comp=What is your company/developer name? 
set /p desc=Describe your app? 
mkdir template
cd template
echo Initialising templates
if not exist batch.c curl -L https://github.com/SarKaa/C-Wrapper-For-Batch/raw/main/batch.c --output batch.c --silent
if not exist Makefile curl -L https://github.com/SarKaa/C-Wrapper-For-Batch/raw/main/Makefile --output Makefile --silent
if not exist batch.rc curl -L https://github.com/SarKaa/C-Wrapper-For-Batch/raw/main/batch.rc --output batch.rc --silent
cd ..
copy template\batch.c %name%.c
copy template\Makefile Makefile
copy template\batch.rc %name%.rc
powershell -Command "(gc %name%.rc) -replace 'VERSION 1,0,0,0', 'VERSION %ver:.=,%' | Set-Content %name%.rc"
powershell -Command "(gc %name%.rc) -replace 'VERSION_STR "1.0.0.0"', 'VERSION_STR "%ver%"' | Set-Content %name%.rc"
powershell -Command "(gc %name%.rc) -replace 'ICON icon.ico', 'ICON %icon%' | Set-Content %name%.rc"
powershell -Command "(gc %name%.rc) -replace 'SarKaa', '%comp%' | Set-Content %name%.rc"
powershell -Command "(gc %name%.rc) -replace 'batch.bat', '%targetfile%' | Set-Content %name%.rc"
powershell -Command "(gc %name%.rc) -replace 'C app converted from batch', '%desc%' | Set-Content %name%.rc"
powershell -Command "(gc %name%.rc) -replace 'batch-to-c', '%name%' | Set-Content %name%.rc"

powershell -Command "(gc Makefile) -replace 'batch', '%name%' | Set-Content Makefile"
powershell -Command "(gc Makefile) -replace '%name%.bat', '%targetfile%' | Set-Content Makefile"
powershell -Command "(gc Makefile) -replace 'icon.ico', '%icon%' | Set-Content Makefile"
mingw32-make
