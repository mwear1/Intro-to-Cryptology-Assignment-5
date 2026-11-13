// Matthew Wear
// Header file for bigint class

#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

class Bigint {
public:
    static constexpr std::size_t CAPACITY = 256; // 256 bytes = 2048 bits

private:
    // Index 0 holds the least significant byte.
    std::array<std::uint8_t, CAPACITY> bytes{};

    friend Bigint bigint_init(const std::string& hex);
    friend std::string bigint_to_string(const Bigint& value);
    friend Bigint bigint_multiply(const Bigint& a, const Bigint& b);
};

Bigint bigint_init(const std::string& hex);
std::string bigint_to_string(const Bigint& value);
Bigint bigint_multiply(const Bigint& a, const Bigint& b);

#endif
