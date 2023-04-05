# Run batch file from C code
A c template for running batch files

So you've probably used the windows command prompt, and experienced the power it gives you. You might have come across various batch files that enable you to do so many cool thing. And I bet you've come across several shady batch to exe "converters". I present to you, a way of running these cool and handy batch files without the hard work. Well actually it needs a bit more work at first, but the end product is much more efficient than other alternatives. Sure you could've done this yourself, but why would you do that?

This C template saves the batch file as a resource within the app, which is extracted to the temp directory, where it is hidden. This file is deleted after closing the app. The control-c escape sequence is also disabled, which means you won't get that annoying "terminate batch job" text. If you change the colours of the terminal window in your batch file, this code will reset it to what it was at the beginning, so it won't leave any odd colours in the command prompt.

But wait, I missed the main bit: as you're just saving the batch as an encrypted text-based resource, it doesn't register as a threat and you don't get the windows defender alerts from it, unlike almost every other batch to exe convertor. This text based resource is also encrypted so it cannot be read from resource hackers

For more information on how it works check out the comments I added in the code (batch.c, batch.rc and Makefile all have comments). My other project [jailm8 for windows](https://github.com/SarKaa/jailm8-windows) uses a modified version of this code for its backend/CLI interface, so be sure to check that out to see how it works.

NOTE: I have used microsoft's own sample code to make [encrypt.c](https://docs.microsoft.com/en-us/windows/win32/seccrypto/example-c-program-encrypting-a-file) and [decrypt.h](https://docs.microsoft.com/en-us/windows/win32/seccrypto/example-c-program-decrypting-a-file), but for decrypt.h I modified it a bit so that it is silent unless any errors occur

I am nowhere near the tidiest with my code, or the most knowledgable, so if you spot any improvements or problems be sure to submit an issue or pull request. 

If you have a choice, I would still recommend writing your app in an actual programming language, as it's just not practical or good practice to write apps like this. I know I made [jailm8 for windows](https://github.com/SarKaa/jailm8-windows) using some of this code, but that's because it requires aspects of this code and its much easier to run other executables from batch.

DISCLAIMER: This isn't completely foolproof. If someone knows where to look, they can find your batch code (unencrypted) while your app is running. If someone knows the password to the resource encryption, then they can decrypt it from the executable itself. The password is stored as a string within the executable, so it is also pretty easy to find if you know where to look. The app and code does its best to prevent people from seeing your batch, but there are shortfalls in the code. Again, if anyone has a way to improve that it would be much appreciated. 

## INSTRUCTIONS: 
Pretty simple really, and once you've done this, you won't need to do it ever again. Just edit the batch as if you're coding an app in it ;)

Step 1: Replace batch.bat and icon.ico with the right files (or change the file names in makefile and batch.rc)

Step 2: Replace the version info in batch.rc with the right info

Step 3: Use mingw to compile (You can use both the windows release and the WSL version of mingw)

NOTE: Cygwin will not work as it creates its own environment

Step 4: Tell me how it goes on [my discord server](https://discord.gg/VDUFB3gpeQ)


## ARGUMENTS:

If your app uses arguments that are passed to the batch, this code makes it easy for you to do this:

argv[0] (executable name) is passed as %1 surrounded  by "quote marks"

argv[1] (first argument) is passed as %2

Every argument after the first is passed at one higher than their number (I.E. the 4th argument is %5, the 10th argument is %11, etc.)


EG.

when you type ```c:\batch.exe I like big black chickens``` into the prompt:

```%1``` = ```"c:\batch.exe"```

```%2``` = ```I```

```%3``` = ```like```

```%4``` = ```big```

```%5``` = ```black```

```%6``` = ```chickens```

NOTE: %1 will be whatever you type into the prompt, so if you type ```c:\batch.exe```, %1 will be ```"c:\batch.exe"```. If you type ```batch.exe```, %1 will be ```"batch.exe"```, and finally, if you type ```batch```, %1 will be, you guessed it, ```"batch"```

## Return codes
If the batch runs successfully, the code will return the exit code of the batch file.

If the batch doesn't run, the program will return a specific error code:

Note ```x``` would be 0 when that part is successful, and 1 when it isn't.

```xxx1``` means the program never managed to generate a temp file location to save the batch to

```xx1x``` means the program either couldn't create the file at the temp file location and/or write the batch data to it

```x1xx``` means the program couldn't start a cmd.exe process to run the file

```1xxx``` means the batch resource couldn't be decrypted

You can get any combo of these codes E.G.:

If you get error code ```1110```, this means that the program did generate the temp file location, but couldn't make the file there and therefore couldn't create a cmd.exe process to run said file

These errors are usually progressive, so if the first bit fails, most, if not all, the others will too.
## Extra Tips
Here's a selection of tips and tricks to use in your batch/c app

If you want to add a manifest to your app, add the line ```CREATEPROCESS_MANIFEST_RESOURCE_ID RT_MANIFEST batch.manifest``` to batch.rc, and save your ```batch.manifest``` file in the same directory

If you want to save all variables except the executable name to one argument, add this line to your batch file: ```for /f "tokens=1,* delims= " %%a in ("%*") do set "args=%%b"```. This will save all variables to one ```%args%``` variable

If you only want the executable name instead of the file path, add the lines ```for %%F in (%1) do set exename=%%~nxF``` and ```if not "%exename:~-4%"==".exe" set "exename=%exename%.exe"``` to your batch file. This will ensure that even if you type ```C:\batch.exe```, ```batch.exe``` or ```batch```, ```%exename%``` will always be ```batch.exe```. This will also remove the "quotation marks".

