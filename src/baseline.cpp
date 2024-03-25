#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "baseline.h"

namespace rinku::brc::baseline {

static constexpr int BUF_SIZE = 8192;

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
                float value = atof(buf + semicolon + 1);

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

void print(soln_t const& soln) {
    std::vector<std::pair<std::string, stats_t>> v;
    for(auto const& [key, val] : soln) {
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

} // namespace rinku::brc::baseline
