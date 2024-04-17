# 1BRC

Solution to the [1BRC](https://github.com/gunnarmorling/1brc) challenge in C++.

# Dependencies

- [libfmt](https://fmt.dev)
- [docopt.cpp](https://github.com/docopt/docopt.cpp)
- [googlebench](https://github.com/google/benchmark)

# Build

`(mkdir .build && cd .build && cmake .. && make)`

# Benchmarks

Benchmarks are run on an Intel 13980HX (8P + 16E core) mobile processor, with 32GB DDR5 dual channel RAM at 5600MHz.

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
Running .build/bench
Run on (32 X 5400 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 0.06, 0.64, 0.79
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
DISKREAD_1E9                     885 ms          883 ms            1
BASELINE_1E9                   62218 ms        62153 ms            1
PARSE_FLOAT_1E9                30076 ms        30042 ms            1
PARSE_INT_1E9                  29240 ms        29205 ms            1
PARSE_TEMP_BRANCHLESS_1E9      28196 ms        28163 ms            1
PARSE_LINE_BRANCHLESS_1E9      25341 ms        25311 ms            1
CUSTOM_MAP_1E9                 13947 ms        13929 ms            1
MMAPIO_1E9                     13452 ms        13437 ms            1
THREADING_1E9_x2                6791 ms        13414 ms            1
THREADING_1E9_x4                3713 ms        14203 ms            1
THREADING_1E9_x8                1876 ms        14182 ms            1
THREADING_1E9_x12               1688 ms        15791 ms            1
THREADING_1E9_x16               1321 ms        16962 ms            1
THREADING_1E9_x24               1061 ms        19869 ms            1
THREADING_1E9_x32                976 ms        22609 ms            1
CHUNKING_1E9_x2                 6764 ms        13423 ms            1
CHUNKING_1E9_x4                 3735 ms        14656 ms            1
CHUNKING_1E9_x8                 1885 ms        14439 ms            1
CHUNKING_1E9_x12                1464 ms        16540 ms            1
CHUNKING_1E9_x16                1224 ms        18128 ms            1
CHUNKING_1E9_x24                 954 ms        20723 ms            1
CHUNKING_1E9_x32                 838 ms        23783 ms            1
PERFECT_HASH_1E9_x2             4841 ms         9590 ms            1
PERFECT_HASH_1E9_x4             2612 ms        10200 ms            1
PERFECT_HASH_1E9_x8             1344 ms        10160 ms            1
PERFECT_HASH_1E9_x12            1001 ms        11075 ms            1
PERFECT_HASH_1E9_x16             822 ms        11885 ms            1
PERFECT_HASH_1E9_x24             623 ms        12995 ms            1
PERFECT_HASH_1E9_x32             512 ms        13659 ms            1
```
