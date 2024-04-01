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
2024-04-01T06:59:34-04:00
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
DISKREAD_1E9                     927 ms          925 ms            1
BASELINE_1E9                   62533 ms        62467 ms            1
PARSE_FLOAT_1E9                30233 ms        30198 ms            1
PARSE_INT_1E9                  29355 ms        29321 ms            1
PARSE_TEMP_BRANCHLESS_1E9      28204 ms        28170 ms            1
PARSE_LINE_BRANCHLESS_1E9      25144 ms        25115 ms            1
CUSTOM_MAP_1E9                 14004 ms        13986 ms            1
MMAPIO_1E9                     13412 ms        13396 ms            1
THREADING_1E9_x2                6694 ms        13150 ms            1
THREADING_1E9_x4                3757 ms        14412 ms            1
THREADING_1E9_x8                2028 ms        14800 ms            1
THREADING_1E9_x12               1667 ms        15851 ms            1
THREADING_1E9_x16               1384 ms        17361 ms            1
THREADING_1E9_x24               1021 ms        19469 ms            1
THREADING_1E9_x32                853 ms        22801 ms            1
```
