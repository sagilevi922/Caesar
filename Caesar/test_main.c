//#include <windows.h>
//#include <tchar.h>
//#include <stdio.h>
//#include <strsafe.h>
//#include "test_main.h"
//
//#define BUFFERSIZE 16
//
//
//int __cdecl _tmain(int argc, TCHAR* argv[])
//{
//    HANDLE hFile;
//    DWORD  dwBytesRead = 0;
//    char   ReadBuffer[BUFFERSIZE] = { 0 };
//    DWORD dwBytesRead1 = 0;
//    DWORD dwFileSize;
//
//
//    hFile = CreateFileA("test.txt",               // file to open
//        GENERIC_READ,          // open for reading
//        FILE_SHARE_READ,       // share for reading
//        NULL,                  // default security
//        OPEN_ALWAYS,         // existing file only
//        FILE_ATTRIBUTE_NORMAL, // normal file
//        NULL);                 // no attr. template
//
//
//    dwFileSize = GetFileSize(hFile, NULL);
//
//    printf("size is %d bytes\n", dwFileSize);
//
//
//    if (hFile == INVALID_HANDLE_VALUE)
//    {
//        printf(("Terminal failure: unable to open file \"%s\" for read.\n"), argv[1]);
//        return;
//    }
//
//    if (FALSE == ReadFile(hFile, ReadBuffer, BUFFERSIZE - 1, &dwBytesRead, NULL))
//    {
//        printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
//        CloseHandle(hFile);
//        return;
//    }
//
//
//    if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE - 1)
//    {
//        ReadBuffer[dwBytesRead] = '\0'; // NULL character
//
//        printf("Data read from %s (%d bytes): \n", argv[1], dwBytesRead);
//        printf("%s\n", ReadBuffer);
//    }
//    else if (dwBytesRead == 0)
//    {
//        printf(("No data read from file %s\n"), argv[1]);
//    }
//    else
//    {
//        printf("\n ** Unexpected value for dwBytesRead ** \n");
//    }
//
//    CloseHandle(hFile);
//}
//
