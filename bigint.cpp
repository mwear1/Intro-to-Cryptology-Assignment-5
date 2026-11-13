// Matthew Wear
// Function definitions for bigint class

#include "bigint.hpp"

#include <stdexcept>

// Convert a single hex character into its numeric value.
static int hex_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

Bigint bigint_init(const std::string& hex) {
    if (hex.empty()) {
        throw std::invalid_argument("Hex string cannot be empty");
    }

    // Validate the complete input, including any leading zeros.
    for (char c : hex) {
        if (hex_value(c) == -1) {
            throw std::invalid_argument("Invalid hexadecimal character");
        }
    }

    Bigint result;
    const std::size_t first = hex.find_first_not_of('0');
    if (first == std::string::npos) return result; // Zero

    const std::size_t length = hex.size() - first;
    if (length > Bigint::CAPACITY * 2) {
        throw std::overflow_error("Integer exceeds 2048 bits");
    }

    // Read right to left because byte 0 is the least significant byte.
    for (std::size_t i = 0; i < length; ++i) {
        const int digit = hex_value(hex[hex.size() - 1 - i]);
        result.bytes[i / 2] |= static_cast<std::uint8_t>(digit << ((i % 2) * 4));
    }
    return result;
}

std::string bigint_to_string(const Bigint& value) {
    const char* digits = "0123456789abcdef";
    std::string output;

    for (int i = static_cast<int>(Bigint::CAPACITY) - 1; i >= 0; --i) {
        const std::uint8_t byte = value.bytes[i];
        if (output.empty()) {
            if (byte == 0) continue;
            if (byte >= 16) output += digits[byte >> 4];
        } else {
            output += digits[byte >> 4];
        }
        output += digits[byte & 15];
    }

    return output.empty() ? "0" : output;
}

// Multiply using the schoolbook algorithm in base 256.
Bigint bigint_multiply(const Bigint& a, const Bigint& b) {
    // Ignore unused leading zero bytes to reduce unnecessary work.
    std::size_t left_size = Bigint::CAPACITY;
    std::size_t right_size = Bigint::CAPACITY;
    while (left_size > 0 && a.bytes[left_size - 1] == 0) --left_size;
    while (right_size > 0 && b.bytes[right_size - 1] == 0) --right_size;

    Bigint result;
    if (left_size == 0 || right_size == 0) return result;

    // Hold the full product so we can check for overflow before returning.
    std::array<std::uint8_t, 2 * Bigint::CAPACITY> product{};

    for (std::size_t i = 0; i < left_size; ++i) {
        unsigned int carry = 0;
        for (std::size_t j = 0; j < right_size; ++j) {
            const unsigned int value = product[i + j]
                                     + static_cast<unsigned int>(a.bytes[i]) * b.bytes[j]
                                     + carry;
            product[i + j] = static_cast<std::uint8_t>(value & 0xff);
            carry = value >> 8;
        }
        product[i + right_size] = static_cast<std::uint8_t>(carry);
    }

    // Any nonzero byte above index 255 means the result exceeds 2048 bits.
    for (std::size_t i = Bigint::CAPACITY; i < product.size(); ++i) {
        if (product[i] != 0) {
            throw std::overflow_error("Multiplication result exceeds 2048 bits");
        }
    }

    for (std::size_t i = 0; i < Bigint::CAPACITY; ++i) {
        result.bytes[i] = product[i];
    }
    return result;
}
