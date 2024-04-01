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
2024-04-01T06:12:54-04:00
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
DISKREAD_1E9                     918 ms          916 ms            1
BASELINE_1E9                   60980 ms        60915 ms            1
PARSE_FLOAT_1E9                28582 ms        28550 ms            1
PARSE_INT_1E9                  27802 ms        27768 ms            1
PARSE_TEMP_BRANCHLESS_1E9      26592 ms        26560 ms            1
PARSE_LINE_BRANCHLESS_1E9      23659 ms        23631 ms            1
CUSTOM_MAP_1E9                 14434 ms        14415 ms            1
MMAPIO_1E9                     14013 ms        13996 ms            1
THREADING_1E9_x2                7119 ms        14080 ms            1
THREADING_1E9_x4                3935 ms        14939 ms            1
THREADING_1E9_x8                1993 ms        14774 ms            1
THREADING_1E9_x12               1771 ms        16810 ms            1
THREADING_1E9_x16               1411 ms        18129 ms            1
THREADING_1E9_x24               1094 ms        20620 ms            1
THREADING_1E9_x32                996 ms        23155 ms            1
```
