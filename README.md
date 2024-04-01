# 1BRC

Solution to the [1BRC](https://github.com/gunnarmorling/1brc) challenge in C++.

# Dependencies

- [libfmt](https://fmt.dev)
- [docopt.cpp](https://github.com/docopt/docopt.cpp)
- [googlebench](https://github.com/google/benchmark)

# Build

`(mkdir .build && cd .build && cmake .. && make)`

# Benchmarks:

Benchmarks are run on an Intel 13980HX mobile processor, with 32GB DDR5 dual channel RAM at 5600MHz.

## Baseline (Java)

```
$ hyperfine --warmup 1 ./calculate_average_baseline.sh
Benchmark 1: ./calculate_average_baseline.sh
  Time (mean ± σ):     100.183 s ±  1.870 s    [User: 97.419 s, System: 3.377 s]
  Range (min … max):   95.961 s … 102.826 s    10 runs
```

## Solution (C++)

```
$ .build/bench --benchmark_filter="1E9(_x[0-9]+/|/)"
2024-04-01T05:49:52-04:00
Running .build/bench
Run on (32 X 5400 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
DISKREAD_1E9                    3513 ms         1878 ms            1
BASELINE_1E9                   60559 ms        60493 ms            1
PARSE_FLOAT_1E9                27162 ms        27133 ms            1
PARSE_INT_1E9                  26602 ms        26571 ms            1
PARSE_TEMP_BRANCHLESS_1E9      25846 ms        25816 ms            1
PARSE_LINE_BRANCHLESS_1E9      22608 ms        22582 ms            1
CUSTOM_MAP_1E9                 15066 ms        15047 ms            1
MMAPIO_1E9                     14471 ms        14453 ms            1
THREADING_1E9_x2               11428 ms        20260 ms            1
THREADING_1E9_x4               10156 ms        32120 ms            1
THREADING_1E9_x8                5375 ms        32352 ms            1
THREADING_1E9_x12               4171 ms        38560 ms            1
THREADING_1E9_x16               3304 ms        43116 ms            1
THREADING_1E9_x24               2762 ms        49247 ms            1
THREADING_1E9_x32               2389 ms        58107 ms            1
```
