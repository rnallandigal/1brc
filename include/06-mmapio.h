#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <fmt/format.h>

namespace rinku::brc::mmapio {

struct elem_t {
    std::string key;
    int64_t min = 1000, sum = 0, max = -1000, count = 0;

    elem_t() {}
    elem_t(char const* s, int len, int64_t temp)
        : key(s, len), min(temp), sum(temp), max(temp), count(1) {}
};

struct map_t {
    std::vector<elem_t> slots;
    int64_t n;

    map_t(int64_t size) : slots(size), n(size) {}

    void add(char const* name, int64_t len, uint64_t hash, int64_t temp);
};

using soln_t = map_t;

soln_t solve(std::string const& datafile);
std::string print(soln_t const& soln);
std::string run(std::string const& datafile);

} // namespace rinku::brc::mmapio
