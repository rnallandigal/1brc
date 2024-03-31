#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "03-parse_temp_branchless.h"

namespace rinku::brc::parse_temp_branchless {

static constexpr int BUF_SIZE = 4'194'304;

// Credit: @merykitty
int64_t parse_temperature(int64_t temp, int64_t len) {
    int64_t sign = (~temp << 59) >> 63;
    int64_t zeroed = temp & ~(sign & 0xff);
    int64_t aligned = zeroed << (40 - 8 * len);
    int64_t reduced = aligned & 0x0f000f0f00;
    int64_t multiplied = reduced * (1 + 10 * 0x10000 + 100 * 0x1000000);
    int64_t ans = (multiplied >> 32) & 0x3ff;
    return (ans ^ sign) - sign;
}

soln_t solve(std::string const& datafile) {
    soln_t m;
    int ret = 1, next = 0;
    char buf[BUF_SIZE + 1];
    int fd = open(datafile.c_str(), O_RDONLY);

    while((ret = read(fd, buf + next, BUF_SIZE - next)) > 0) {
        ret += next;
        int j = 0, start = 0, semicolon = -1;
        while(j < ret) {
            for(start = j; j < ret && buf[j] != '\n'; j++) {
                if(buf[j] == ';') {
                    semicolon = j;
                }
            }
            if(j == ret) {
                next = 0;
                for(int k = start; k < ret; k++, next++) {
                    buf[next] = buf[k];
                }
            } else {
                buf[j] = '\0';
                std::string key(buf + start, semicolon - start);

                int64_t value = parse_temperature(
                    (*(int64_t*)&buf[semicolon + 1]), j - semicolon - 1
                );

                stats_t& s = m[key];
                s.min = std::min(s.min, value);
                s.sum += value;
                s.max = std::max(s.max, value);
                s.count++;
            }
            j++;
            if(j == ret) {
                next = 0;
            }
        }
    }
    close(fd);
    return m;
}

std::string print(soln_t const& m) {
    std::vector<std::pair<std::string, stats_t>> v;
    for(auto const& [key, val] : m) {
        v.emplace_back(key, val);
    }

    std::sort(v.begin(), v.end(), [](auto const& a, auto const& b) {
        return a.first < b.first;
    });

    std::string s = "{";
    for(auto const& [key, val] : v) {
        s += key;
        s += '=';
        s += fmt::format("{:.1f}", (double)val.min / 10);
        s += '/';
        s += fmt::format("{:.1f}", (double)val.sum / (10 * val.count));
        s += '/';
        s += fmt::format("{:.1f}", (double)val.max / 10);
        s += ", ";
    }
    s.pop_back();
    s[s.size() - 1] = '}';
    return s;
}

std::string run(std::string const& datafile) { return print(solve(datafile)); }

} // namespace rinku::brc::parse_temp_branchless
