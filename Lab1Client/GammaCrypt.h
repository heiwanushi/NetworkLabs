#pragma once
#include <vector>
#include <string>

// Генерация гаммы из строки-ключа
inline std::vector<uint8_t> generate_gamma(const std::string& key, size_t length) {
    std::vector<uint8_t> gamma(length);
    for (size_t i = 0; i < length; ++i)
        gamma[i] = key[i % key.size()] ^ (uint8_t)(i * 31);
    return gamma;
}

// Шифрование/дешифрование данных гаммированием
inline void gamma_crypt(const uint8_t* input, uint8_t* output, size_t length, const std::vector<uint8_t>& gamma) {
    for (size_t i = 0; i < length; ++i)
        output[i] = input[i] ^ gamma[i];
}