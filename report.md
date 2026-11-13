# Assignment 5: Big Integers

**Name:** Matthew Wear  
**Course:** Introduction to Cryptography  
**Language:** C++17 (Python 3.14.8 for testing and performance comparison)

## Problem 1: Big Integer Representation

### Representation

I represented each big integer using a fixed-size `std::array<std::uint8_t, 256>`. Each element stores one byte (8 bits), giving a maximum size of 2048 bits. The least significant byte is stored at index 0 and the most significant byte at the highest used index.

I chose this representation because the assignment sets a fixed 2048-bit limit. It is straightforward to work with individual bytes and does not require dynamic memory allocation or a built-in big integer library.

### Hexadecimal Conversion

**`bigint_init`:** Converts a hexadecimal string into the byte array. It reads the string from right to left, storing two hex digits per byte. Uppercase and lowercase digits and leading zeros are accepted. Empty strings, invalid characters, and numbers larger than 2048 bits are rejected with exceptions.

**`bigint_to_string`:** Reads the bytes from most significant to least significant and returns lowercase hexadecimal without leading zeros. Zero is output as `0`.

For example, `bigint_init("0000ABCD")` is converted back to the string `abcd`.

## Problem 2: Big Integer Multiplication

For each pair of operand bytes $a_i$ and $b_j$, the algorithm adds their product to the corresponding result byte and includes the carry from the previous calculation:

$$
t = p_{i+j} + a_i b_j + \mathrm{carry}
$$

$$
p_{i+j} = t \bmod 256, \qquad \mathrm{carry} = \left\lfloor \frac{t}{256} \right\rfloor
$$

The implementation follows these steps:

1. Find the significant byte lengths of both operands, ignoring unused leading zero bytes.
2. Multiply each byte of the first operand by each byte of the second using nested loops.
3. Store each partial product in its correct position and carry values above 255 into the next byte.
4. Use a temporary 512-byte array for the full product. If any byte beyond the 2048-bit limit is nonzero, throw `std::overflow_error`. Otherwise, return the lower 256 bytes.

For operands with $m$ and $n$ significant bytes, the time complexity is $O(mn)$. 

## Problem 3: Testing and Performance Evaluation

### (a) Correctness Testing

I used C++ `assert()` tests for correctness testing.

- `test_bigint.cpp` checks zero, hexadecimal input and output, uppercase letters, leading zeros, invalid input, and the 2048-bit limit.
- `test_multiply.cpp` checks multiplication by zero and one, basic products, carries, reversed operands, large products, and overflow detection.
- `benchmark.py` generates 497 reproducible test vectors using Python's built-in `int`: 192 cases using 256-, 512-, and 1024-bit operands, 300 additional random cases, and 5 boundary cases.

**Test Results:** Both C++ test programs passed. The C++ implementation matched Python's expected result or expected overflow for all 497 vectors, and the Python reference checks also passed.

```text
Done testing hexadecimal input and output.
Done testing multiplication.
C++ correctness checks passed: 497 vectors.
Python correctness checks passed: 497 vectors.
```

### (b) Performance Evaluation

I compiled the C++ code with `-O2` optimization. The C++ benchmark uses `std::chrono::steady_clock`, and Python uses `time.perf_counter_ns()`.

Both programs use the same generated operands, with a fixed random seed of `2026`. Parsing, printing, file operations, program startup, and correctness checks occur outside the timed sections. Each operand size uses 64 pairs, 200 repetitions, and 7 trials, for **89,600 multiplications per size in each language**. The results are average nanoseconds per multiplication.

**Test Environment:** Windows with VS Code PowerShell, C++17 compiled using `g++ -O2`, and Python 3.14.8.

**Performance Results:**

| Operand Size | C++ Average (ns) | Python Average (ns) | C++ / Python |
|---|---:|---:|---:|
| 256 bits | 1,194.31 | 135.34 | 8.82× |
| 512 bits | 2,993.99 | 490.11 | 6.11× |
| 1024 bits | 10,717.72 | 1,252.67 | 8.56× |

### (c) Explanation

Python was faster at all three operand sizes. Although my implementation is written in C++, it uses a simple algorithm that multiplies one byte at a time. Python's built-in big integers use optimized native-code arithmetic and larger internal units, reducing the amount of work required.

The C++ running time increases as operand size increases. Doubling both operand lengths requires approximately four times as many byte multiplications. The measured C++/Python ratios vary because of differences in implementation and timing overhead.

These results show that the implementation works correctly for the required sizes, but it prioritizes a straightforward design over advanced multiplication algorithms. The exact timings may vary between computers and benchmark runs.
