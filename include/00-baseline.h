#pragma once

#include <string>
#include <unordered_map>

namespace rinku::brc::baseline {

struct stats_t {
    float min = 100, mean = 0, max = -100;
    int count = 0;
};
using soln_t = std::unordered_map<std::string, stats_t>;

soln_t solve(std::string const& datafile);
std::string print(soln_t const& soln);
std::string run(std::string const& datafile);

} // namespace rinku::brc::baseline
