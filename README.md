# c-wrapper-for-batch
A c template for running batch files

## INSTRUCTIONS: 

Replace batch.bat, icon.ico and batch.manifest with the right files

Replace the version info in batch.rc with the right info

Use mingw or cygwin to compile


## ARGUMENTS:

argv[0] (executable name) is passed as %1

argv[1] (first argument) is passed as %2

every argument after the first is passed at one higher than their number


EG.

when you type "c:\batch.exe big black chickens" into the prompt:

%1 = c:\batch.exe

%2 = big

%3 = black

%4 = chickens
