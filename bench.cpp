#include <benchmark/benchmark.h>

#include "00-baseline.h"
#include "01-parse_float.h"
#include "02-parse_int.h"
#include "03-parse_temp_branchless.h"
#include "diskread.h"

#define BENCHMARK_1BRC(bench, sut, input) \
    void bench(benchmark::State& state) { \
        using namespace rinku::brc::sut;  \
        for(auto _ : state)               \
            solve(input);                 \
    }                                     \
    BENCHMARK(bench)->Unit(benchmark::kMillisecond)

BENCHMARK_1BRC(DISKREAD_1E5, diskread, "resources/100k.txt");
BENCHMARK_1BRC(DISKREAD_1E7, diskread, "resources/10m.txt");
BENCHMARK_1BRC(DISKREAD_1E9, diskread, "resources/1b.txt");

BENCHMARK_1BRC(BASELINE_1E5, baseline, "resources/100k.txt");
BENCHMARK_1BRC(BASELINE_1E7, baseline, "resources/10m.txt");
BENCHMARK_1BRC(BASELINE_1E9, baseline, "resources/1b.txt");

BENCHMARK_1BRC(PARSE_FLOAT_1E5, parse_float, "resources/100k.txt");
BENCHMARK_1BRC(PARSE_FLOAT_1E7, parse_float, "resources/10m.txt");
BENCHMARK_1BRC(PARSE_FLOAT_1E9, parse_float, "resources/1b.txt");

BENCHMARK_1BRC(PARSE_INT_1E5, parse_int, "resources/100k.txt");
BENCHMARK_1BRC(PARSE_INT_1E7, parse_int, "resources/10m.txt");
BENCHMARK_1BRC(PARSE_INT_1E9, parse_int, "resources/1b.txt");

BENCHMARK_1BRC(
    PARSE_TEMP_BRANCHLESS_1E5, parse_temp_branchless, "resources/100k.txt"
);
BENCHMARK_1BRC(
    PARSE_TEMP_BRANCHLESS_1E7, parse_temp_branchless, "resources/10m.txt"
);
BENCHMARK_1BRC(
    PARSE_TEMP_BRANCHLESS_1E9, parse_temp_branchless, "resources/1b.txt"
);
