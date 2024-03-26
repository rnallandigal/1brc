#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "02-parse_int.h"

namespace rinku::brc::parse_int {

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

                char* digits = &buf[semicolon + 1];
                int64_t value = 0, sign = 1;

                if(*digits == '-') sign = -1, digits++;
                value = *digits++ - '0';
                if(*digits != '.') value = 10 * value + *digits++ - '0';
                value = sign * (10 * value + *(++digits) - '0');

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

} // namespace rinku::brc::parse_int
