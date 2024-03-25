#pragma once

#include <string>

namespace rinku::brc::diskread {
using soln_t = int;

soln_t solve(std::string const& datafile);
std::string print(soln_t const& soln);
std::string run(std::string const& datafile);

} // namespace rinku::brc::diskread
