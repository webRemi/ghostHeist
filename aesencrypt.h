#include <Windows.h>
#ifndef AESENCRYPT_H
#define AESENCRYPT_H

unsigned char* AesEncrypt(int nothing, unsigned char* cEncryptedCode, int code_len, char* key, size_t keylen);

#endif