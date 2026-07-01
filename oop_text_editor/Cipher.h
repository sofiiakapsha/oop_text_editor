#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <memory>
#include <ranges>

class Text;

class Cipher {
public:
    Cipher() = default;
    ~Cipher() = default;
    void Encrypt(Text& textStorage, int cipherType, const std::string& key);
    void Decrypt(Text& textStorage, int cipherType, const std::string& key);
};