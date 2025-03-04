#include <Windows.h>
#include <stdio.h>

#define ERROR "[-]"
#define SUCCESS "[+]"
#define INFO "[>]"

unsigned char* LoadFile(LPCSTR lFileName, long* file_size) {
    HANDLE hFile = INVALID_HANDLE_VALUE;
    unsigned char* lFileContent = NULL;
    DWORD bytesRead = 0;

    hFile = CreateFileA(lFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("%s CreateFileA failed with error: 0x%x\n", ERROR, GetLastError());
        return NULL;
    }

    *file_size = GetFileSize(hFile, NULL);
    if (*file_size == INVALID_FILE_SIZE) {
        printf("%s GetFileSize failed with error: 0x%x\n", ERROR, GetLastError());
        CloseHandle(hFile);
        return NULL;
    }
    printf("%s FileSize is: %ld bytes\n", INFO, *file_size);

    lFileContent = (unsigned char*)malloc(*file_size);
    if (lFileContent == NULL) {
        printf("%s Memory allocation failed with error: 0x%x\n", ERROR, GetLastError());
        CloseHandle(hFile);
        return NULL;
    }

    if (!ReadFile(hFile, lFileContent, *file_size, &bytesRead, NULL)) {
        printf("%s ReadFile failed with error: 0x%x\n", ERROR, GetLastError());
        free(lFileContent);
        CloseHandle(hFile);
        return NULL;
    }

    if (bytesRead != (DWORD)*file_size) {
        printf("%s ReadFile read less bytes than expected!\n", ERROR);
        free(lFileContent);
        CloseHandle(hFile);
        return NULL;
    }

    CloseHandle(hFile);

    return lFileContent;
}
