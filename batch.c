#include<stdio.h> 
#include<windows.h>
#include<string.h>
#define IDR_BIN 1234

CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

char *temp, *batch;
int err = 0;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void clean() 
{ 
  remove(temp);
  SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
  free(temp);
} 

void gettemp() {
   temp = (char*)malloc(sizeof(char)*1024);
   GetTempPathA(1024, temp);
   temp = (char *) realloc(temp, strlen(temp) + 100);
   if (0==(GetTempFileNameA(temp, NULL, GetCurrentProcessId(), temp))) {
     perror("Cannot Generate Temporary File Name");
     err = 1;
     }
   strcpy(temp, strcat(temp, ".bat"));
   temp = (char *) realloc(temp, strlen(temp) + 1);
}

void startup() {
   atexit(clean);
   gettemp();
   SetConsoleCtrlHandler(NULL, TRUE);
   GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
}

void makebatch() {
   HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_BIN), TEXT("BATCH"));
   unsigned int batchsize = SizeofResource(NULL, hResource);
   HGLOBAL hGlobal = LoadResource(NULL, hResource);
   const char* lpData = static_cast<char*> ( LockResource(hGlobal) );
   FreeResource(hResource);
   CloseHandle(hGlobal);
   CloseHandle(hResource);
   HANDLE batchhandle = CreateFile(temp, FILE_SHARE_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY, NULL); 
  if (batchhandle==NULL || (WriteFile( batchhandle, lpData, batchsize, NULL, NULL))==FALSE) {
    perror("Unable To Create Resource Files");
    err = 1;
    }
  CloseHandle(batchhandle);
}

int runbatch() {
   PROCESS_INFORMATION pi;
   STARTUPINFO si;
   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );
   if (0==(CreateProcess( NULL, batch, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ))) {
     perror("Unable To Create Child Process");
     err = 1;
     }
  free(batch);
  if (err!=0) {
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      return err;
   }
   WaitForSingleObject( pi.hProcess, INFINITE );
   DWORD exit_code;
   GetExitCodeProcess(pi.hProcess, &exit_code);
   CloseHandle(pi.hProcess);
   CloseHandle(pi.hThread);
   return exit_code;
}

int main(int argc,char* argv[])
{
  startup();
  remove(temp);
  if (argc>1) {
    int i;
    int intsize = strlen(argv[1]) + 2;
    char *args = (char*)malloc(sizeof(char)*intsize);
    strcpy(args,argv[1]);
    for (i=2; i< argc; i++) {
        intsize = intsize + strlen(argv[i]) + 5;
        args = (char *) realloc(args, sizeof(char)*intsize);
        sprintf(args, "%s %s", args, argv[i]);
        }
    batch = (char*)malloc(strlen(temp) + strlen(argv[0]) + strlen(args) + 20);
    sprintf(batch, "%s \"%s\" %s 2>NUL", temp, argv[0], args);
    free(args);
    batch = (char*)realloc(batch, strlen(batch) + 1);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
  } else {
    batch = (char*)malloc(strlen(temp) + strlen(argv[0]) + 20);
    sprintf(batch, "%s \"%s\" 2>NUL", temp, argv[0]);
    batch = (char*)realloc(batch, strlen(batch) + 1);
  }
  makebatch();
  exit(runbatch());
}