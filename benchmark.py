""" Generate correctness vectors and compare C++ vs Python multiplication speed.
    Run after compiling benchmark_cpp.cpp with -O2. No third-party packages.
"""

from pathlib import Path
import random
import subprocess
import sys
import time

HERE = Path(__file__).resolve().parent
VECTORS = HERE / "vectors.txt"
SEED = 2026
CASES_PER_SIZE = 64
RANDOM_CASES = 300
REPEATS = 200
TRIALS = 7
SIZES = (256, 512, 1024)


def number_with_bits(rng, bits):
    if bits == 0:
        return 0
    return (1 << (bits - 1)) | rng.getrandbits(bits - 1)


def vector_line(kind, bits, left, right):
    product = left * right
    expected = "OVERFLOW" if product.bit_length() > 2048 else format(product, "x")
    return f"{kind} {bits} {left:x} {right:x} {expected}\n"


def generate_vectors():
    rng = random.Random(SEED)
    with VECTORS.open("w", encoding="ascii") as output:
        # The same exact operands will be used for C++ and Python timing.
        for bits in SIZES:
            for _ in range(CASES_PER_SIZE):
                a = number_with_bits(rng, bits)
                b = number_with_bits(rng, bits)
                output.write(vector_line("benchmark", bits, a, b))

        # Additional random sizes, including products that overflow 2048 bits.
        boundaries = (0, 1, 7, 8, 9, 255, 256, 511, 512,
                      1023, 1024, 1025, 1536, 2047, 2048)
        for index in range(RANDOM_CASES):
            a_bits = boundaries[index % len(boundaries)] if index < 60 else rng.randrange(2049)
            b_bits = boundaries[(index * 7 + 3) % len(boundaries)] if index < 60 else rng.randrange(2049)
            a = number_with_bits(rng, a_bits)
            b = number_with_bits(rng, b_bits)
            output.write(vector_line("random", max(a_bits, b_bits), a, b))

        # Explicit edge cases for zero, the maximum value, and overflow.
        maximum = (1 << 2048) - 1
        for a, b in ((0, maximum), (1, maximum), (maximum, 2),
                     (maximum, maximum), (1 << 2047, 1)):
            output.write(vector_line("random", 2048, a, b))


def load_python_pairs():
    pairs = {bits: [] for bits in SIZES}
    checked = 0
    for line in VECTORS.read_text(encoding="ascii").splitlines():
        kind, bit_text, left_hex, right_hex, expected = line.split()
        a, b = int(left_hex, 16), int(right_hex, 16)
        product = a * b
        actual = "OVERFLOW" if product.bit_length() > 2048 else format(product, "x")
        assert actual == expected, f"Incorrect expected result at vector {checked}"
        if kind == "benchmark":
            pairs[int(bit_text)].append((a, b))
        checked += 1
    return pairs, checked


def benchmark_python(pairs):
    times = {}
    for bits in SIZES:
        cases = pairs[bits]
        outputs = [0] * len(cases)
        total_ns = 0
        for _ in range(TRIALS):
            start = time.perf_counter_ns()
            for _ in range(REPEATS):
                for index, (a, b) in enumerate(cases):
                    outputs[index] = a * b
            total_ns += time.perf_counter_ns() - start
            # Consume results after timing, rather than during multiplication.
            assert all(value >= 0 for value in outputs)
        times[bits] = total_ns / (TRIALS * REPEATS * len(cases))
    return times


def benchmark_cpp():
    executable = HERE / ("benchmark_cpp.exe" if sys.platform == "win32" else "benchmark_cpp")
    if not executable.exists():
        raise FileNotFoundError(f"Compile the C++ benchmark first: {executable.name}")
    run = subprocess.run([str(executable), str(VECTORS)], cwd=HERE,
                         capture_output=True, text=True, check=True)
    print(run.stderr.strip())
    times = {}
    for line in run.stdout.splitlines():
        bits, average_ns, _checksum = line.split(",")
        times[int(bits)] = float(average_ns)
    return times


def main():
    generate_vectors()
    pairs, checked = load_python_pairs()
    cpp = benchmark_cpp()
    python = benchmark_python(pairs)
    print(f"Python correctness checks passed: {checked} vectors.")
    print(f"Seed: {SEED} | {CASES_PER_SIZE} pairs/size | {REPEATS} repeats x {TRIALS} trials")
    print("Size (bits) | C++ avg (ns) | Python avg (ns) | C++ / Python")
    for bits in SIZES:
        print(f"{bits:11} | {cpp[bits]:12.2f} | {python[bits]:15.2f} | {cpp[bits] / python[bits]:12.2f}x")


if __name__ == "__main__":
    main()
