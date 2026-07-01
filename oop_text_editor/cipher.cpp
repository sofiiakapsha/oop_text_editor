#include <windows.h>
#include "cipher_api.h"
#include <iostream>
#include <string>
#include <sstream>
#include "cipher.h"
#include "line.h"
#include "text.h"

namespace {
    std::vector<std::string> splitFields(const std::string& s) {
        std::vector<std::string> parts;
        std::stringstream ss(s);
        std::string field;
        while (std::getline(ss, field, '|')) {
            parts.push_back(field);
        }
        if (!s.empty() && s.back() == '|') {
            parts.push_back("");
        }
        return parts;
    }

    std::string joinFields(const std::vector<std::string>& parts) {
        std::string result;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) result += "|";
            result += parts[i];
        }
        return result;
    }
}

void Cipher::Encrypt(Text& textStorage, int cipherType, const std::string& key) {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    printf("EXE path: %s\n", path);

    HMODULE hLib = LoadLibrary(TEXT("Cipher_dll.dll"));
    if (hLib == nullptr) {
        std::cout << "Error: reading library!" << std::endl;
        return;
    }

    typedef cipher_t(*CreateCaesarFunc)(int);
    typedef cipher_t(*CreateVigenereFunc)(const char*);
    typedef cipher_t(*CreateAtbashFunc)();
    typedef char* (*EncryptFunc)(cipher_t, const char*);
    typedef void(*FreeFunc)(char*);
    typedef void(*DestroyFunc)(cipher_t);

    auto create_caesar = (CreateCaesarFunc)GetProcAddress(hLib, "cipher_create_caesar");
    auto create_vigenere = (CreateVigenereFunc)GetProcAddress(hLib, "cipher_create_vigenere");
    auto create_atbash = (CreateAtbashFunc)GetProcAddress(hLib, "cipher_create_atbash");
    auto encrypt = (EncryptFunc)GetProcAddress(hLib, "cipher_encrypt");
    auto free_func = (FreeFunc)GetProcAddress(hLib, "cipher_free");
    auto destroy = (DestroyFunc)GetProcAddress(hLib, "cipher_destroy");

    if (!create_caesar || !create_vigenere || !create_atbash || !encrypt || !free_func || !destroy) {
        std::cout << "Error: loading functions!" << std::endl;
        FreeLibrary(hLib);
        return;
    }

    cipher_t cipher_inst = nullptr;
    if (cipherType == 1) cipher_inst = create_caesar(std::stoi(key));
    else if (cipherType == 2) cipher_inst = create_vigenere(key.c_str());
    else if (cipherType == 3) cipher_inst = create_atbash();

    std::vector<std::string> serialized = textStorage.allSerialized();
    std::vector<std::string> encryptedLines;

    for (const auto& lineStr : serialized) {
        bool hasMarker = (lineStr.find('|') != std::string::npos && 
                         (lineStr.substr(0, 5) == "TEXT|" || 
                          lineStr.substr(0, 5) == "CONT|" || 
                          lineStr.substr(0, 5) == "CHEK|"));

        if (hasMarker) {
            std::vector<std::string> fields = splitFields(lineStr);
            for (size_t i = 1; i < fields.size(); ++i) {
                if (fields[i].empty()) continue;
                char* encBytes = encrypt(cipher_inst, fields[i].c_str());
                fields[i] = encBytes ? encBytes : fields[i];
                if (encBytes) free_func(encBytes);
            }
            encryptedLines.push_back(joinFields(fields));
        } else {
            char* encBytes = encrypt(cipher_inst, lineStr.c_str());
            encryptedLines.push_back(encBytes ? encBytes : lineStr);
            if (encBytes) free_func(encBytes);
        }
    }

    textStorage.loadFromSerialized(encryptedLines);
    destroy(cipher_inst);
    FreeLibrary(hLib);
    std::printf("Text encrypted successfully.\n");
}

void Cipher::Decrypt(Text& textStorage, int cipherType, const std::string& key) {
    HMODULE hLib = LoadLibrary(TEXT("Cipher_dll.dll"));
    if (hLib == nullptr) {
        std::cout << "Error: reading library!" << std::endl;
        return;
    }

    typedef cipher_t(*CreateCaesarFunc)(int);
    typedef cipher_t(*CreateVigenereFunc)(const char*);
    typedef cipher_t(*CreateAtbashFunc)();
    typedef char* (*DecryptFunc)(cipher_t, const char*);
    typedef void(*FreeFunc)(char*);
    typedef void(*DestroyFunc)(cipher_t);

    auto create_caesar = (CreateCaesarFunc)GetProcAddress(hLib, "cipher_create_caesar");
    auto create_vigenere = (CreateVigenereFunc)GetProcAddress(hLib, "cipher_create_vigenere");
    auto create_atbash = (CreateAtbashFunc)GetProcAddress(hLib, "cipher_create_atbash");
    auto decrypt = (DecryptFunc)GetProcAddress(hLib, "cipher_decrypt");
    auto free_func = (FreeFunc)GetProcAddress(hLib, "cipher_free");
    auto destroy = (DestroyFunc)GetProcAddress(hLib, "cipher_destroy");

    if (!create_caesar || !create_vigenere || !create_atbash || !decrypt || !free_func || !destroy) {
        std::cout << "Error: loading functions!" << std::endl;
        FreeLibrary(hLib);
        return;
    }

    cipher_t cipher_inst = nullptr;
    if (cipherType == 1) cipher_inst = create_caesar(std::stoi(key));
    else if (cipherType == 2) cipher_inst = create_vigenere(key.c_str());
    else if (cipherType == 3) cipher_inst = create_atbash();

    std::vector<std::string> serialized = textStorage.allSerialized();
    std::vector<std::string> decryptedLines;

    for (const auto& lineStr : serialized) {
        bool hasMarker = (lineStr.find('|') != std::string::npos && 
                         (lineStr.substr(0, 5) == "TEXT|" || 
                          lineStr.substr(0, 5) == "CONT|" || 
                          lineStr.substr(0, 5) == "CHEK|"));

        if (hasMarker) {
            std::vector<std::string> fields = splitFields(lineStr);
            for (size_t i = 1; i < fields.size(); ++i) {
                if (fields[i].empty()) continue;
                char* decBytes = decrypt(cipher_inst, fields[i].c_str());
                fields[i] = decBytes ? decBytes : fields[i];
                if (decBytes) free_func(decBytes);
            }
            decryptedLines.push_back(joinFields(fields));
        } else {
            char* decBytes = decrypt(cipher_inst, lineStr.c_str());
            decryptedLines.push_back(decBytes ? decBytes : lineStr);
            if (decBytes) free_func(decBytes);
        }
    }

    textStorage.loadFromSerialized(decryptedLines);
    destroy(cipher_inst);
    FreeLibrary(hLib);
    std::printf("Text decrypted successfully.\n");
}