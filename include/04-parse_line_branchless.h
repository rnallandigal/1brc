#pragma once

#include <string>
#include <unordered_map>

namespace rinku::brc::parse_line_branchless {

struct stats_t {
    int64_t min = 1000, sum = 0, max = -1000, count = 0;
};
using soln_t = std::unordered_map<std::string, stats_t>;

soln_t solve(std::string const& datafile);
std::string print(soln_t const& soln);
std::string run(std::string const& datafile);

} // namespace rinku::brc::parse_line_branchless
