// Multiplication tests for bigint

#include "bigint.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

// Check multiplication in both operand orders.
static void check_product(const std::string& left, const std::string& right,
                          const std::string& expected) {
    const Bigint a = bigint_init(left);
    const Bigint b = bigint_init(right);
    assert(bigint_to_string(bigint_multiply(a, b)) == expected);
    assert(bigint_to_string(bigint_multiply(b, a)) == expected);
    // Multiplication must not modify the inputs.
    assert(bigint_to_string(a) == left);
    assert(bigint_to_string(b) == right);
}

int main() {
    // Multiplication by zero and one.
    check_product("0", "0", "0");
    check_product("0", "1234", "0");
    check_product("1", "abcd", "abcd");

    // Basic multiplication, with carries and different operand sizes.
    check_product("a", "b", "6e");
    check_product("ff", "ff", "fe01");
    check_product("1234", "10", "12340");
    check_product("ffff", "ffff", "fffe0001");
    check_product("1000000", "1000000", "1000000000000");

    // A 1024-bit number squared still fits in 2048 bits.
    const std::string thousand_bits(256, 'f');
    const std::string large_product =
        std::string(255, 'f') + "e" + std::string(255, '0') + "1";
    check_product(thousand_bits, thousand_bits, large_product);

    // The highest bit of a 2048-bit result is allowed to be set.
    const std::string power_2040 = "1" + std::string(510, '0');
    check_product(power_2040, "80", "8" + std::string(511, '0'));

    // Overflow: these results require more than 2048 bits.
    const std::string max_hex(512, 'f');
    for (const std::string& factor : {std::string("2"), max_hex}) {
        bool caught = false;
        try {
            (void)bigint_multiply(bigint_init(max_hex), bigint_init(factor));
        } catch (const std::overflow_error&) {
            caught = true;
        }
        assert(caught);
    }

    std::cout << "Done testing multiplication.\n";
}
