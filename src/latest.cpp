#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "latest.h"

namespace rinku::brc::latest {

static constexpr int BUF_SIZE = 4'194'304;

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

                char* val_start = buf + semicolon + 1;
                float multiplier = 1;
                float value = 0;
                if(val_start[0] == '-') {
                    multiplier = -1;
                    val_start++;
                }
                if(val_start[1] == '.') {
                    value =
                        (val_start[0] - '0') + (float)(val_start[2] - '0') / 10;
                } else {
                    value = 10 * (val_start[0] - '0')
                            + (float)(val_start[1] - '0')
                            + (float)(val_start[3] - '0') / 10;
                }
                value *= multiplier;

                stats_t& s = m[key];
                s.min = std::min(s.min, value);
                s.mean = (s.mean * s.count + value) / (s.count + 1);
                s.max = std::max(s.max, value);
                s.count++;
            }
            j++;
            if(j == ret) {
                next = 0;
            }
        }
    }
    return m;
}

void print(soln_t const& m) {
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
        s += fmt::format("{:.1f}", val.min);
        s += '/';
        s += fmt::format("{:.1f}", val.mean);
        s += '/';
        s += fmt::format("{:.1f}", val.max);
        s += ", ";
    }
    s.pop_back();
    s[s.size() - 1] = '}';
    fmt::print("{}\n", s);
}

void run(std::string const& datafile) { print(solve(datafile)); }

} // namespace rinku::brc::latest
