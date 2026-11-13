// Tests for bigint

#include "bigint.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    // Default value and zero formatting.
    assert(bigint_to_string(Bigint{}) == "0");
    assert(bigint_to_string(bigint_init("0")) == "0");
    assert(bigint_to_string(bigint_init("00000")) == "0");

    // Hexadecimal parsing and lowercase output.
    assert(bigint_to_string(bigint_init("a")) == "a");
    assert(bigint_to_string(bigint_init("ABC")) == "abc");
    assert(bigint_to_string(bigint_init("0000AbCd")) == "abcd");
    assert(bigint_to_string(bigint_init("0100")) == "100");
    assert(bigint_to_string(bigint_init("00FF00")) == "ff00");
    assert(bigint_to_string(bigint_init("ab54a98ceb1f0ad2")) ==
           "ab54a98ceb1f0ad2");

    // Maximum 2048-bit value and arbitrarily many leading zeros.
    const std::string max_hex(512, 'f');
    assert(bigint_to_string(bigint_init(max_hex)) == max_hex);
    assert(bigint_to_string(bigint_init(std::string(600, '0') + "Ab")) == "ab");

    // Empty, non-hexadecimal, and prefixed values must be rejected.
    for (const std::string input : {"", "xyz", "12G", "0x12", "-1"}) {
        bool caught = false;
        try {
            (void)bigint_init(input);
        } catch (const std::invalid_argument&) {
            caught = true;
        }
        assert(caught);
    }

    // A nonzero 513-digit number exceeds 2048 bits.
    bool overflow = false;
    try {
        (void)bigint_init(std::string(513, 'f'));
    } catch (const std::overflow_error&) {
        overflow = true;
    }
    assert(overflow);

    std::cout << "Done testing hexadecimal input and output.\n";
}
