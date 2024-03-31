#include <benchmark/benchmark.h>

#include "00-baseline.h"
#include "01-parse_float.h"
#include "02-parse_int.h"
#include "03-parse_temp_branchless.h"
#include "04-parse_line_branchless.h"
#include "05-custom_map.h"
#include "06-mmapio.h"
#include "07-threading.h"
#include "diskread.h"

#define BRC_IMPL(bench, sut, input, solve, print) \
    void bench(benchmark::State& state) {         \
        using namespace rinku::brc::sut;          \
        for(auto _ : state)                       \
            print(solve(input));                  \
    }                                             \
    BENCHMARK(bench)                              \
        ->Unit(benchmark::kMillisecond)           \
        ->MeasureProcessCPUTime()                 \
        ->UseRealTime();

#define BRC(bench, sut, input) BRC_IMPL(bench, sut, input, solve, print)
#define BRC_PURE(bench, sut, input) BRC_IMPL(bench##_PURE, sut, input, solve, )

#define BRC_THREADED(bench, sut, input, N) \
    BRC_IMPL(bench, sut, input, solve<N>, print)

#define BRC_THREADED_PURE(bench, sut, input, N) \
    BRC_IMPL(bench##_PURE, sut, input, solve<N>, )

#define BRC_ALL(bench, sut)                           \
    BRC(bench##_1E5, sut, "resources/100k.txt");      \
    BRC(bench##_1E7, sut, "resources/10m.txt");       \
    BRC(bench##_1E9, sut, "resources/1b.txt");        \
    BRC_PURE(bench##_1E5, sut, "resources/100k.txt"); \
    BRC_PURE(bench##_1E7, sut, "resources/10m.txt");  \
    BRC_PURE(bench##_1E9, sut, "resources/1b.txt")

#define BRC_THREADED_ALL(bench, sut, N)                                \
    BRC_THREADED(bench##_1E5_x##N, sut, "resources/100k.txt", N);      \
    BRC_THREADED(bench##_1E7_x##N, sut, "resources/10m.txt", N);       \
    BRC_THREADED(bench##_1E9_x##N, sut, "resources/1b.txt", N);        \
    BRC_THREADED_PURE(bench##_1E5_x##N, sut, "resources/100k.txt", N); \
    BRC_THREADED_PURE(bench##_1E7_x##N, sut, "resources/10m.txt", N);  \
    BRC_THREADED_PURE(bench##_1E9_x##N, sut, "resources/1b.txt", N)

BRC_ALL(DISKREAD, diskread);
BRC_ALL(BASELINE, baseline);
BRC_ALL(PARSE_FLOAT, parse_float);
BRC_ALL(PARSE_INT, parse_int);
BRC_ALL(PARSE_TEMP_BRANCHLESS, parse_temp_branchless);
BRC_ALL(PARSE_LINE_BRANCHLESS, parse_line_branchless);
BRC_ALL(CUSTOM_MAP, custom_map);
BRC_ALL(MMAPIO, mmapio);

BRC_THREADED_ALL(THREADING, threading, 2);
BRC_THREADED_ALL(THREADING, threading, 4);
BRC_THREADED_ALL(THREADING, threading, 8);
BRC_THREADED_ALL(THREADING, threading, 12);
BRC_THREADED_ALL(THREADING, threading, 16);
BRC_THREADED_ALL(THREADING, threading, 24);
BRC_THREADED_ALL(THREADING, threading, 32);
