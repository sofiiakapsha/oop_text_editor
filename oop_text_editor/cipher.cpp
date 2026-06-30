#include <windows.h>
#include "cipher_api.h"
#include <iostream>
#include <string>
#include "text_editor.h"
#include "line.h"

void TextEditor::Encrypt() {

    HMODULE hLib = LoadLibrary(TEXT("cipher.dll"));

    if (hLib == nullptr) {
        std::cout << "Error: reading library!" << std::endl;
        return;
    }

    typedef cipher_t(*CreateCaesarFunc)(int);
    CreateCaesarFunc create_caesar = (CreateCaesarFunc)GetProcAddress(hLib, "cipher_create_caesar");

    typedef cipher_t(*CreateVigenereFunc)(const char*);
    CreateVigenereFunc create_vigenere = (CreateVigenereFunc)GetProcAddress(hLib, "cipher_create_vigenere");

    typedef cipher_t(*CreateAtbashFunc)();
    CreateAtbashFunc create_atbash = (CreateAtbashFunc)GetProcAddress(hLib, "cipher_create_atbash");

    typedef char* (*EncryptFunc)(cipher_t, const char*);
    EncryptFunc encrypt = (EncryptFunc)GetProcAddress(hLib, "cipher_encrypt");

    typedef void(*FreeFunc)(char*);
    FreeFunc free_func = (FreeFunc)GetProcAddress(hLib, "cipher_free");

    typedef void(*DestroyFunc)(cipher_t);
    DestroyFunc destroy = (DestroyFunc)GetProcAddress(hLib, "cipher_destroy");

    if (!create_caesar || !create_vigenere || !create_atbash || !encrypt || !free_func || !destroy) {
        std::cout << "Error: loading functions!" << std::endl;
        FreeLibrary(hLib);
        return;
    }

    int answer;

    std::printf("\nChoose cipher:\n");
    std::printf("1 - Caesar cipher\n");
    std::printf("2 - Vigenere cipher\n");
    std::printf("3 - Atbash cipher\n");

    std::scanf("%d", &answer);

    while (getchar() != '\n');

    switch (answer) {
    case 1: {
        int length;
        std::printf("Choose the length for cipher:\n");
        std::scanf("%d", &length);
        while (getchar() != '\n');

        cipher_t cipher_c = create_caesar(length);

        std::vector<char*> encryptedText;

        char* code = encrypt(cipher_c, text.c_str());

        destroy(cipher_c);

    }
          break;
    case 2: {

        std::string key;
        std::printf("Choose the key for cipher:\n");
        std::getline(std::cin, key);

        cipher_t cipher_v = create_vigenere(key.c_str());

        int choice;

        std::printf("Choose encrypt (1) or decrypt (2):\n");
        std::scanf("%d", &choice);
        while (getchar() != '\n');

        if (choice == 1) {
            std::string text;

            std::printf("Write the text to encrypt:\n");
            std::getline(std::cin, text);

            char* enc = encrypt(cipher_v, text.c_str());
            std::printf("Vigenere Encrypted: %s\n", enc);

            free_func(enc);
        }
        else if (choice == 2) {
            std::string text;

            std::printf("Write the text to decrypt:\n");
            std::getline(std::cin, text);

            char* dec = decrypt(cipher_v, text.c_str());
            std::printf("Vigenere Decrypted: %s\n", dec);

            free_func(dec);

        }

        destroy(cipher_v);
    }
          break;

    case 3: {

        cipher_t cipher_a = create_atbash();

        int choice;

        std::printf("Choose encrypt (1) or decrypt (2):\n");
        std::scanf("%d", &choice);
        while (getchar() != '\n');

        if (choice == 1) {
            std::string text;

            std::printf("Write the text to encrypt:\n");
            std::getline(std::cin, text);

            char* enc = encrypt(cipher_a, text.c_str());
            std::printf("Atbash Encrypted: %s\n", enc);

            free_func(enc);
        }
        else if (choice == 2) {
            std::string text;

            std::printf("Write the text to decrypt:\n");
            std::getline(std::cin, text);

            char* dec = decrypt(cipher_a, text.c_str());
            std::printf("Atbash Decrypted: %s\n", dec);

            free_func(dec);

        }

        destroy(cipher_a);
    }
          break;
    case 4: {
        FreeLibrary(hLib);
        return 0;
        break;
    }
    }
}

int main() {

    HMODULE hLib = LoadLibrary(TEXT("cipher.dll"));

    if (hLib == nullptr) {
        std::cout << "Error: reading library!" << std::endl;
        return 1;
    }

    std::cout << "cipher.dll loaded successfully at runtime." << std::endl;

    typedef cipher_t(*CreateCaesarFunc)(int);
    CreateCaesarFunc create_caesar = (CreateCaesarFunc)GetProcAddress(hLib, "cipher_create_caesar");

    typedef cipher_t(*CreateVigenereFunc)(const char*);
    CreateVigenereFunc create_vigenere = (CreateVigenereFunc)GetProcAddress(hLib, "cipher_create_vigenere");

    typedef cipher_t(*CreateAtbashFunc)();
    CreateAtbashFunc create_atbash = (CreateAtbashFunc)GetProcAddress(hLib, "cipher_create_atbash");

    typedef char* (*EncryptFunc)(cipher_t, const char*);
    EncryptFunc encrypt = (EncryptFunc)GetProcAddress(hLib, "cipher_encrypt");

    typedef char* (*DecryptFunc)(cipher_t, const char*);
    DecryptFunc decrypt = (DecryptFunc)GetProcAddress(hLib, "cipher_decrypt");

    typedef void(*FreeFunc)(char*);
    FreeFunc free_func = (FreeFunc)GetProcAddress(hLib, "cipher_free");

    typedef void(*DestroyFunc)(cipher_t);
    DestroyFunc destroy = (DestroyFunc)GetProcAddress(hLib, "cipher_destroy");

    if (!create_caesar || !create_vigenere || !create_atbash || !encrypt || !decrypt || !free_func || !destroy) {
        std::cout << "Error: loading functions!" << std::endl;
        FreeLibrary(hLib);
        return 1;
    }

    std::cout << "All function pointers resolved correctly." << std::endl;
