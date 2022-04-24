# Run batch file from C code
A c template for running batch files

So you've probably used the windows command prompt, and experienced the power it gives you. You might have come across various batch files that enable you to do so many cool thing. And I bet you've come across several shady batch to exe "converters". I present to you, a way of running these cool and handy batch files without the hard work. Well actually it needs a bit more work at first, but the end product is much more efficient than other alternatives. Sure you could've done this yourself, but why would you do that?

This C template saves the batch file as a resource within the app, which is extracted to the temp directory, where it is hidden. This file is deleted after closing the app. The control-c escape sequence is also disabled, which means you won't get that annoying "terminate batch job" text. If you change the colours of the terminal window in your batch file, this code will reset it to what it was at the beginning, so it won't leave any odd colours in the command prompt.

But wait, I missed the main bit: as you're just saving the batch as a text-based resource, it doesn't register as a threat and you don't get the windows defender alerts from it, unlike almost every other batch to exe convertor.

If you have a choice, I would still recommend writing your app in an actual programming language, as it's just not good practice to write apps like this.

## INSTRUCTIONS: 
Pretty simple really, and once you've done this, you won't need to do it ever again. Just edit the batch as if you're coding an app in it ;)

Step 1: Replace batch.bat, icon.ico and batch.manifest with the right files

Step 2: Replace the version info in batch.rc with the right info

Step 3: Use mingw to compile (cygwin works for compilation but not cleaning)

Step 4: Tell me how it goes on [my discord server](https://discord.gg/VDUFB3gpeQ)


## ARGUMENTS:

If your app uses arguments that are passed to the batch, this code makes it easy for you to do this:

argv[0] (executable name) is passed as %1

argv[1] (first argument) is passed as %2

Every argument after the first is passed at one higher than their number (I.E. the 4th argument is %5, the 10th argument is %11, etc.)


EG.

when you type ```c:\batch.exe I like big black chickens``` into the prompt:

%1 = c:\batch.exe

%2 = I

%3 = like

%4 = big

%5 = black

%6 = chickens

NOTE: %1 will be whatever you type into the prompt, so if you type ```c:\batch.exe```, %1 will be ```c:\batch.exe```. If you type ```batch.exe```, %1 will be ```batch.exe```, and finally, if you type ```batch```, %1 will be, you guessed it, ```batch```
