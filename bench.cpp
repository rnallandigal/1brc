#include <benchmark/benchmark.h>

#include "baseline.h"
#include "diskread.h"
#include "floatparsing.h"
#include "intparsing.h"

#define BENCHMARK_1BRC(bench, sut, input) \
    void bench(benchmark::State& state) { \
        using namespace rinku::brc::sut;  \
        for(auto _ : state)               \
            solve(input);                 \
    }                                     \
    BENCHMARK(bench)->Unit(benchmark::kMillisecond)

BENCHMARK_1BRC(BRC_DISKREAD_1E5, diskread, "resources/100k.txt");
BENCHMARK_1BRC(BRC_DISKREAD_1E7, diskread, "resources/10m.txt");
BENCHMARK_1BRC(BRC_DISKREAD_1E9, diskread, "resources/1b.txt");

BENCHMARK_1BRC(BRC_BASELINE_1E5, baseline, "resources/100k.txt");
BENCHMARK_1BRC(BRC_BASELINE_1E7, baseline, "resources/10m.txt");
BENCHMARK_1BRC(BRC_BASELINE_1E9, baseline, "resources/1b.txt");

BENCHMARK_1BRC(BRC_FLOATPARSING_1E5, floatparsing, "resources/100k.txt");
BENCHMARK_1BRC(BRC_FLOATPARSING_1E7, floatparsing, "resources/10m.txt");
BENCHMARK_1BRC(BRC_FLOATPARSING_1E9, floatparsing, "resources/1b.txt");

BENCHMARK_1BRC(BRC_INTPARSING_1E5, intparsing, "resources/100k.txt");
BENCHMARK_1BRC(BRC_INTPARSING_1E7, intparsing, "resources/10m.txt");
BENCHMARK_1BRC(BRC_INTPARSING_1E9, intparsing, "resources/1b.txt");
