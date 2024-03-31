#include <fcntl.h>
#include <unistd.h>

#include <string>

#include <fmt/core.h>

#include "diskread.h"

namespace rinku::brc::diskread {

static constexpr int BUF_SIZE = 4'194'304;

// .build/bench --benchmark_filter="DISKREAD" runs all three and gives 0ms
// .build/bench --benchmark_filter="1E9" gives the correct time of 900ms
soln_t solve(std::string const& datafile) {
    int ret = 1, total = 0;
    char buf[BUF_SIZE + 1];
    int fd = open(datafile.c_str(), O_RDONLY);

    while((ret = read(fd, buf, BUF_SIZE)) > 0) {
        total += buf[ret / 2];
    }
    close(fd);
    return total;
}

std::string print(soln_t const& m) {
    return fmt::format("total read: {}\n", m);
}

std::string run(std::string const& datafile) { return print(solve(datafile)); }

} // namespace rinku::brc::diskread
