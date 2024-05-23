#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdbool.h>
#define IDR_BIN 1234

CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

char *temp, *batch;
int err = 0;

HANDLE hConsole;

#ifdef __cplusplus
extern "C"
{
#endif
	bool MyDecryptFile(
		LPTSTR szSource,
		LPTSTR szDestination,
		LPTSTR szPassword);
#ifdef __cplusplus
}
#endif

void clean()
{
	// Delete extracted batch file, reset console colours and free dynamically allocated memory
	DeleteFileA(temp);
	DeleteFileA(strcat(strdup(temp), ".tmp"));
	free(temp);
	SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
}

DWORD gettemp()
{
	// Allow pseudo random string to be generated for the temp file name
	srand(GetTickCount() * GetCurrentProcessId());
	static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-#'!";
	size_t length = 10;
	char *rndstr = NULL;
	if (length)
	{
		rndstr = (char *)malloc(sizeof(char) * (length + 1));
		if (rndstr)
		{
			for (int n = 0; n < length; n++)
			{
				int key = rand() % (int)(sizeof(charset) - 1);
				rndstr[n] = charset[key];
			}
			rndstr[length] = '\0';
		}
	}
	// Generate a unique file path to extract batch file to
	temp = (char *)malloc(sizeof(char) * 1024);
	// Get temp directory
	GetTempPathA(1024, temp);
	temp = (char *)realloc(temp, strlen(temp) + length + 10);
	strcpy(temp, strcat(temp, strcat(rndstr, ".bat")));
	free(rndstr);
	temp = (char *)realloc(temp, strlen(temp) + 1);
	// Allows startup() to check if the file already exists
	return (GetFileAttributes(temp));
}

void startup()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// Register exit protocols
	atexit(clean);
	// Generate temp file path and keep repeating until a unique name is found
	while (gettemp() != INVALID_FILE_ATTRIBUTES && GetFileAttributes(strcat(temp, ".tmp")) != INVALID_FILE_ATTRIBUTES)
		free(temp);
	// Disable control C
	SetConsoleCtrlHandler(NULL, TRUE);
	// Save console attribute (colours)
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
}

void makebatch()
{
	// Extract batch file to a hidden file at temp file location generated from gettemp()
	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_BIN), TEXT("BATCH"));
	unsigned int batchsize = SizeofResource(NULL, hResource);
	HGLOBAL hGlobal = LoadResource(NULL, hResource);
	// Get a pointer to the file data
	const char *lpData = (char *)LockResource(hGlobal);
	FreeResource(hResource);
	CloseHandle(hGlobal);
	CloseHandle(hResource);
	// Create hidden temp file using file path (this file will be saved as xxxx.bat.tmp)
	HANDLE batchhandle = CreateFile(strcat(strdup(temp), ".tmp"), FILE_SHARE_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY, NULL);
	// Write encrypted data to file and check for errors
	if (batchhandle == NULL || !WriteFile(batchhandle, lpData, batchsize, NULL, NULL))
	{
		printf("Unable To Create Resource Files: Error number %d\n", GetLastError());
		err = err + 10;
	}
	CloseHandle(batchhandle);
	MoveFileExA(strcat(strdup(temp), ".tmp"), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
	// Decrypt the data and delete the encrypted temp file (note this file can be decrypted if someone knows the password defined in Makefile so I recommend changing it there)
	if (false == MyDecryptFile(strcat(strdup(temp), ".tmp"), temp, PASSWORD))
	{
		printf("Unable To Read Resources : Error number %d\n", GetLastError());
		err = err + 1;
	}
	DeleteFileA(strcat(strdup(temp), ".tmp"));
	// Mark file for deletion after a reboot just in case the app is killed forcefully
	MoveFileExA(temp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
}

int runbatch()
{
	// Create cmd process to run batch file and return exit code
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (0 == (CreateProcess(NULL, batch, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)))
	{
		printf("Unable To Create Child Process: Error number %d\n", GetLastError());
		err = err + 100;
	}
	free(batch);
	// If there were errors it will return the error code for the failed processes
	if (err != 0)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return err;
	}
	// Wait for batch file to exit
	WaitForSingleObject(pi.hProcess, INFINITE);
	// Get exit code for batch and return it to main
	DWORD exit_code;
	GetExitCodeProcess(pi.hProcess, &exit_code);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return exit_code;
}

int main(int argc, char *argv[])
{
	startup();
	char *args = strdup("");
	if (argc > 1)
	{
		int i;
		int intsize = strlen(argv[1]) + 2;
		args = (char *)malloc(sizeof(char) * intsize);
		// Copies first argument to variable
		strcpy(args, argv[1]);
		for (i = 2; i < argc; i++)
		{
			// Appends every other argument to variable with a space
			intsize = intsize + strlen(argv[i]) + 5;
			args = (char *)realloc(args, sizeof(char) * intsize);
			sprintf(args, "%s %s", args, argv[i]);
		}
	}
	// Generate final command to pass to CreateProcess
	batch = (char *)malloc(strlen(temp) + strlen(argv[0]) + strlen(args) + 20);
	sprintf(batch, "\"%s\" \"%s\" %s", temp, argv[0], args);
	if (argc > 1)
		free(args);
	batch = (char *)realloc(batch, strlen(batch) + 1);
	makebatch();
	// Return final return value of batch to the main function
	exit(runbatch());
}