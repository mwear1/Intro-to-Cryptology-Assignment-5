// Reads generated test vectors, verifies C++ bigint multiplication,
// and measures average execution time for comparison

#include "bigint.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

constexpr int REPEATS = 200;
constexpr int TRIALS = 7;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: benchmark_cpp <vectors.txt>\n";
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "Cannot open test vectors.\n";
        return 1;
    }

    // Loading, parsing, and correctness checks all happen before timing.
    std::map<int, std::vector<std::pair<Bigint, Bigint>>> benchmarks;
    std::string kind, left_hex, right_hex, expected;
    int bits = 0;
    int tested = 0;

    while (input >> kind >> bits >> left_hex >> right_hex >> expected) {
        try {
            Bigint left = bigint_init(left_hex);
            Bigint right = bigint_init(right_hex);
            Bigint result = bigint_multiply(left, right);
            if (expected == "OVERFLOW" || bigint_to_string(result) != expected) {
                throw std::runtime_error("Incorrect multiplication result");
            }
            if (kind == "benchmark") {
                benchmarks[bits].emplace_back(left, right);
            }
        } catch (const std::overflow_error&) {
            if (expected != "OVERFLOW") {
                std::cerr << "Unexpected overflow in vector " << tested << '\n';
                return 1;
            }
        } catch (const std::exception& error) {
            std::cerr << "Vector " << tested << ": " << error.what() << '\n';
            return 1;
        }
        ++tested;
    }

    if (tested == 0 || benchmarks.size() != 3) {
        std::cerr << "Missing test cases or benchmark sizes.\n";
        return 1;
    }
    std::cerr << "C++ correctness checks passed: " << tested << " vectors.\n";

    // Report average nanoseconds per multiplication across all trials.
    for (const auto& group : benchmarks) {
        const auto& pairs = group.second;
        std::vector<Bigint> outputs(pairs.size());
        double total_nanoseconds = 0;
        std::size_t checksum = 0;

        for (int trial = 0; trial < TRIALS; ++trial) {
            const auto start = std::chrono::steady_clock::now();
            for (int repeat = 0; repeat < REPEATS; ++repeat) {
                for (std::size_t i = 0; i < pairs.size(); ++i) {
                    outputs[i] = bigint_multiply(pairs[i].first, pairs[i].second);
                }
            }
            const auto end = std::chrono::steady_clock::now();
            total_nanoseconds +=
                std::chrono::duration<double, std::nano>(end - start).count();

            // Consume results outside the timed region.
            for (const Bigint& result : outputs) {
                checksum += bigint_to_string(result).size();
            }
        }

        const double operations = static_cast<double>(pairs.size()) * REPEATS * TRIALS;
        std::cout << group.first << ',' << std::fixed << std::setprecision(2)
                  << total_nanoseconds / operations << ',' << checksum << '\n';
    }
}
