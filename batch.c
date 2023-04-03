#include<stdio.h> 
#include<windows.h>
#include<string.h>
#include"decrypt.h"
#define IDR_BIN 1234

CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

char *temp, *batch;
int err = 0;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void clean() 
{ 
  // Delete extracted batch file, reset console colours and free dynamically allocated memory
  remove(temp);
  remove(strcat(strdup(temp), ".tmp"));
  SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
  // If deleting the batch file didn't work the first time, we can mark it to be deleted after a reboot
  if (GetFileAttributes(temp) != INVALID_FILE_ATTRIBUTES) MoveFileExA(temp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
  if (GetFileAttributes(strcat(strdup(temp), ".tmp")) != INVALID_FILE_ATTRIBUTES) MoveFileExA(strcat(strdup(temp), ".tmp"), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
  free(temp);
} 

void gettemp() {
   // Generate a unique file path to extract batch file to
   temp = (char*)malloc(sizeof(char)*1024);
   // Get temp directory
   GetTempPathA(1024, temp);
   temp = (char *) realloc(temp, strlen(temp) + 100);
   // Generate file name and make sure it doesn't already exist
   if (0==(GetTempFileNameA(temp, NULL, GetTickCount()*GetCurrentProcessId(), temp))) {
     perror("Cannot Generate Temporary File Name");
     err = err + 1;
     }
   // Change file extension from .tmp to .tmp.bat
   strcpy(temp, strcat(temp, ".bat"));
   temp = (char *) realloc(temp, strlen(temp) + 1);
}

void startup() {
   // Register exit protocols
   atexit(clean);
   // Generate temp file path
   gettemp();
   // Disable control C
   SetConsoleCtrlHandler(NULL, TRUE);
   // Save console attribute (colours)
   GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
}

void makebatch() {
   // Extract batch file to a hidden file at temp file location generated from gettemp()
   HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_BIN), TEXT("BATCH"));
   unsigned int batchsize = SizeofResource(NULL, hResource);
   HGLOBAL hGlobal = LoadResource(NULL, hResource);
   // Get a pointer to the file data
   const char* lpData = static_cast<char*> ( LockResource(hGlobal) );
   FreeResource(hResource);
   CloseHandle(hGlobal);
   CloseHandle(hResource);
   // Create hidden temp file using file path (yes this file will be saved as xxxx.tmp.bat.tmp)
   HANDLE batchhandle = CreateFile(strcat(strdup(temp), ".tmp"), FILE_SHARE_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY, NULL);
   // Write encrypted data to file and check for errors 
  if (batchhandle==NULL || (WriteFile( batchhandle, lpData, batchsize, NULL, NULL))==FALSE) {
    perror("Unable To Create Resource Files");
    err = err + 10;
    }
  CloseHandle(batchhandle);
  // Decrypt the data and delete the encrypted temp file (note this file can be decrypted if someone knows the password "Aal izz well" so I recommend changing it)
  if (false==MyDecryptFile(strcat(strdup(temp), ".tmp"), temp, "Aal izz well")) {
    perror("Unable To Read Resources");
    err = err + 1000;
    }
  remove(strcat(strdup(temp), ".tmp"));
  SetFileAttributesA(temp, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY);
}

int runbatch() {
   // Create cmd process to run batch file and return exit code
   PROCESS_INFORMATION pi;
   STARTUPINFO si;
   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );
   if (0==(CreateProcess( NULL, batch, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ))) {
     perror("Unable To Create Child Process");
     err = err + 100;
     }
  free(batch);
  // If there were errors it will return 1
  if (err!=0) {
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      return err;
   }
   // Wait for batch file to exit
   WaitForSingleObject( pi.hProcess, INFINITE );
   // Get exit code for batch and return it to main
   DWORD exit_code;
   GetExitCodeProcess(pi.hProcess, &exit_code);
   CloseHandle(pi.hProcess);
   CloseHandle(pi.hThread);
   return exit_code;
}

int main(int argc,char* argv[])
{
  startup();
  if (argc>1) {
      int i;
      int intsize = strlen(argv[1]) + 2;
      char *args = (char*)malloc(sizeof(char)*intsize);
      // Copies first argument to variable
      strcpy(args,argv[1]);
      for (i=2; i< argc; i++) {
      // Appends every other argument to variable with a space
          intsize = intsize + strlen(argv[i]) + 5;
           args = (char *) realloc(args, sizeof(char)*intsize);
          sprintf(args, "%s %s", args, argv[i]);
          }
      batch = (char*)malloc(strlen(temp) + strlen(argv[0]) + strlen(args) + 20);
      // Generate final command to pass to CreateProcess
      sprintf(batch, "%s \"%s\" %s", temp, argv[0], args);
      free(args);
      batch = (char*)realloc(batch, strlen(batch) + 1);
    } else {
      batch = (char*)malloc(strlen(temp) + strlen(argv[0]) + 20);
      // Generate final command to pass to CreateProcess
      sprintf(batch, "%s \"%s\"", temp, argv[0]);
      batch = (char*)realloc(batch, strlen(batch) + 1);
  }
  makebatch();
  // Return final return value of batch to the main function
  exit(runbatch());
}