#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "05-custom_map.h"

namespace rinku::brc::custom_map {

void map_t::add(char const* name, int64_t len, uint64_t hash, int64_t temp) {
    for(int64_t slot = hash % n; true; slot = (slot + 1) % n) {
        if(slots[slot].key.empty()) {
            slots[slot] = elem_t(name, len, temp);
            break;
        } else if(strncmp(name, slots[slot].key.data(), len) == 0) {
            slots[slot].min = std::min(slots[slot].min, temp);
            slots[slot].sum += temp;
            slots[slot].max = std::max(slots[slot].max, temp);
            slots[slot].count++;
            break;
        }
    }
}

static constexpr int BUF_SIZE = 4'194'304;
static constexpr int64_t DOT_DIFFERENTIATOR = 0x1010101010101000;

// Credit: https://richardstartin.github.io/posts/finding-bytes
int64_t find_semicolon(int64_t bytes) {
    static constexpr int64_t SEMICOLON = 0x3b3b3b3b3b3b3b3b;
    static constexpr int64_t MAX_8BYTE_INT = 0x7f7f7f7f7f7f7f7f;
    int64_t matched = bytes ^ SEMICOLON;
    int64_t adjusted = (matched & MAX_8BYTE_INT) + MAX_8BYTE_INT;
    int64_t ans = ~(adjusted | matched | MAX_8BYTE_INT);
    return ans;
}

// Credit: @merykitty
int64_t parse_temperature(int64_t temp, int dot_idx) {
    static constexpr int64_t DIGIT_MASK = 0x0f000f0f00;
    static constexpr int64_t MULTIPLICAND = 1 + 10 * 0x10000 + 100 * 0x1000000;

    int64_t sign = (~temp << 59) >> 63;
    int64_t zeroed = temp & ~(sign & 0xff);
    int64_t aligned = zeroed << (28 - dot_idx);
    int64_t reduced = aligned & DIGIT_MASK;
    int64_t multiplied = reduced * MULTIPLICAND;
    int64_t ans = (multiplied >> 32) & 0x3ff;
    return (ans ^ sign) - sign;
}

soln_t solve(std::string const& datafile) {
    soln_t m(10000);
    int bytes_read = 1, leftover = 0, leftover_start;
    char buf[BUF_SIZE + 8];
    int fd = open(datafile.c_str(), O_RDONLY);

    while((bytes_read = read(fd, buf + leftover, BUF_SIZE - leftover)) > 0) {
        bytes_read += leftover;
        leftover = 0;

        for(leftover_start = bytes_read; buf[--leftover_start] != '\n';)
            ;
        leftover_start++;

        uint64_t offset = 0, hash = 0;
        for(char* line = buf; line + offset < &buf[leftover_start];) {
            int64_t next8 = *(int64_t*)&line[offset];
            int64_t semicolon_one_hot = find_semicolon(next8);

            if(!semicolon_one_hot) {
                offset += 8;
                hash += next8;
            } else {
                int64_t semicolon_bit_idx = __builtin_ctzll(semicolon_one_hot);
                offset += semicolon_bit_idx / 8 + 1;
                int64_t temp = *(int64_t*)(&line[offset]);
                int64_t dot_idx = __builtin_ctzll(~temp & DOT_DIFFERENTIATOR);
                int64_t value = parse_temperature(temp, dot_idx);

                hash += next8 & ~(~0lu << semicolon_bit_idx);
                m.add(line, offset - 1, hash, value);

                line += offset + dot_idx / 8 + 3;
                offset = 0;
                hash = 0;
            }
        }

        for(int i = leftover_start; i < bytes_read; i++, leftover++) {
            buf[leftover] = buf[i];
        }
    }
    close(fd);
    return m;
}

std::string print(soln_t const& m) {

    std::vector<elem_t> v;
    for(auto const& elem : m.slots) {
        if(!elem.key.empty()) v.push_back(elem);
    }

    std::sort(v.begin(), v.end(), [](auto const& a, auto const& b) {
        return a.key < b.key;
    });

    std::string s = "{";
    for(auto const& elem : v) {
        s += elem.key;
        s += '=';
        s += fmt::format("{:.1f}", (double)elem.min / 10);
        s += '/';
        s += fmt::format("{:.1f}", (double)elem.sum / (10 * elem.count));
        s += '/';
        s += fmt::format("{:.1f}", (double)elem.max / 10);
        s += ", ";
    }
    s.pop_back();
    s[s.size() - 1] = '}';
    return s;
}

std::string run(std::string const& datafile) { return print(solve(datafile)); }

} // namespace rinku::brc::custom_map
