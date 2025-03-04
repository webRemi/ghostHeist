#include <Windows.h>
#include <stdio.h>

#define ERROR "[-]"
#define SUCCESS "[+]"
#define INFO "[>]"

unsigned char* AesEncrypt(int nothing, char* code, int code_len, char* key, size_t keylen) {
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;
    HCRYPTKEY hKey;

    if (!CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        //printf("%s CryptAcquireContext failed with error: 0x%x\n", ERROR, GetLastError());
        exit(1);
    }
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        //printf("%s CryptCreateHash failed with error: 0x%x\n", ERROR, GetLastError());
        exit(1);
    }
    if (!CryptHashData(hHash, (BYTE*)key, (DWORD)keylen, 0)) {
        //printf("%s CryptHashData failed with error: 0x%x\n", ERROR, GetLastError());
        exit(1);
    }
    if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0, &hKey)) {
        //printf("%s CryptDeriveKey failed with error: 0x%x\n", ERROR, GetLastError());
        exit(1);
    }

    unsigned char* padded_data = PadData((unsigned char*)code, (DWORD*)&code_len);

    DWORD encrypted_len = (DWORD)code_len;

    unsigned char* encrypted_code = (unsigned char*)malloc(encrypted_len);
    if (encrypted_code == NULL) {
        //printf("%s Memory allocation FOR encrypted code failed with error: 0x%x\n", ERROR, GetLastError());
        exit(1);
    }

    memcpy(encrypted_code, code, code_len);

    if (!CryptEncrypt(hKey, (HCRYPTHASH)NULL, 0, 0, encrypted_code, &encrypted_len, encrypted_len)) {
        //printf("%s CryptDecrypt failed with error 0x%x\n", ERROR, GetLastError());
        exit(1);
    }

    CryptReleaseContext(hProv, 0);
    CryptDestroyHash(hHash);
    CryptDestroyKey(hKey);

    //printf("%s Sucessfully encrypted the file\n", SUCCESS);

    return encrypted_code;
}