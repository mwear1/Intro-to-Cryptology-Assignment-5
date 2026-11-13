# Assignment 5: Big Integers

**Name:** Matthew Wear  
**Course:** Introduction to Cryptography  
**Language:** C++17  
**Python Version:** 3.14.8 (reference testing and performance comparison)

## Overview

This assignment implements a big integer type in C++ that supports hexadecimal input/output and multiplication of non-negative integers up to 2048 bits.

The implementation uses a fixed-size byte array and multiplication in base 256. Python's built-in `int` is used only to verify multiplication results and compare performance. No built-in big integer library is used in the C++ implementation.

## Files

- `bigint.hpp` - Bigint type and function declarations.
- `bigint.cpp` - Hexadecimal conversion and multiplication implementation.
- `test_bigint.cpp` - Tests for hexadecimal conversion and invalid inputs.
- `test_multiply.cpp` - Tests for multiplication and overflow.
- `benchmark_cpp.cpp` - C++ correctness checks and multiplication timing.
- `benchmark.py` - Generates test vectors and compares C++ against Python.
- `report.md` - Representation, algorithm, testing results, and performance analysis.
- `vectors.txt` - Test vectors generated automatically by `benchmark.py`.

## Requirements

- A C++17 compiler such as `g++`.
- Python 3 (tested with Python 3.14.8).

No third-party libraries are required.

## Building and Running the Tests

Open PowerShell in the assignment folder.

### Problem 1: Hexadecimal Conversion

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pedantic bigint.cpp test_bigint.cpp -o test_bigint.exe
.\test_bigint.exe
```

Expected result:

```text
Done testing hexadecimal input and output.
```

### Problem 2: Multiplication

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pedantic bigint.cpp test_multiply.cpp -o test_multiply.exe
.\test_multiply.exe
```

Expected result:

```text
Done testing multiplication.
```

## Running the Performance Comparison

### Problem 3: C++ vs. Python

Compile the C++ benchmark with optimization enabled:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pedantic bigint.cpp benchmark_cpp.cpp -o benchmark_cpp.exe
```

Then run the Python script:

```powershell
py -3 benchmark.py
```

The script creates `vectors.txt`, verifies 497 multiplication cases, and measures the average multiplication time for 256-, 512-, and 1024-bit operands in both languages.

Expected correctness results:

```text
C++ correctness checks passed: 497 vectors.
Python correctness checks passed: 497 vectors.
```

The benchmark prints a comparison table. Timing values may vary between runs and computers. The results measured on my Windows computer are recorded in `report.md`.

## Implementation Notes

Each big integer is stored as 256 bytes with the least significant byte at index 0. The hexadecimal functions accept and produce the format specified in the assignment.

Multiplication uses nested loops with base-256 carries. A temporary 512-byte array allows the implementation to detect products exceeding 2048 bits rather than silently truncating them.

Test data generation and hexadecimal conversion occur outside the benchmark's timed sections, so the timing focuses on multiplication.
