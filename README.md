# Run batch file from C code
A c template for running batch files

So you've probably used the windows command prompt, and experienced the power it gives you. You might have come across various batch files that enable you to do so many cool thing. And I bet you've come across several shady batch to exe "converters". I have an easier way. Well actually it needs a bit more work at first, but the end product is much more efficient and quick

This C template saves the batch file as a resource within the app, which is extracted to the temp directory, where it is hidden. This file is deleted after closing the app. The control-c escape sequence is also disabled, which means you won't get that annoying "terminate batch job" text.

But wait, I missed the main bit, as you're just saving the batch as a text-based resource, it doesn't register as a threat and you don't get the windows defender alerts from it, unlike almost every other batch to exe convertor.

If you have a choice, I would still recommend writing your app in an actual programming language, as it's just not good practice to write apps like this.

## INSTRUCTIONS: 

Replace batch.bat, icon.ico and batch.manifest with the right files

Replace the version info in batch.rc with the right info

Use mingw to compile (cygwin works for compilation but not cleaning)


## ARGUMENTS:

argv[0] (executable name) is passed as %1

argv[1] (first argument) is passed as %2

Every argument after the first is passed at one higher than their number (I.E. the 4th argument is %5, the 10th argument is %11, etc.)


EG.

when you type "c:\batch.exe big black chickens" into the prompt:

%1 = c:\batch.exe

%2 = big

%3 = black

%4 = chickens
