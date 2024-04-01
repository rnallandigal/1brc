#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include <fmt/format.h>
#include <sys/mman.h>

#include "07-threading.h"

namespace rinku::brc::threading {

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

void work(soln_t& m, char* buf, char* end) {
    uint64_t offset = 0, hash = 0;
    for(char* line = buf; line + offset < end;) {
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
}

template <int N>
std::vector<soln_t> solve(std::string const& datafile) {
    int fd = open(datafile.c_str(), O_RDONLY);
    int64_t file_length = lseek(fd, 0, SEEK_END);
    char* buf = (char*)mmap(nullptr, file_length, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    madvise(buf, file_length, MADV_SEQUENTIAL);

    std::vector<soln_t> solns(N, soln_t(10000));
    std::vector<std::thread> threads;

    int64_t start = 0;
    for(int i = 0; i < N; i++) {
        int64_t end = std::min(file_length, start + file_length / N);
        while(buf[end - 1] != '\n')
            end++;
        threads.emplace_back(work, std::ref(solns[i]), &buf[start], &buf[end]);
        start = end;
    }

    for(int i = 0; i < N; i++)
        threads[i].join();

    munmap(buf, file_length);
    return solns;
}

std::string print(std::vector<soln_t> const& solns) {
    std::map<std::string, elem_t> m;

    for(auto const& soln : solns) {
        for(auto const& elem : soln.slots) {
            if(elem.key.empty()) continue;

            elem_t& acc = m[elem.key];
            acc.min = std::min(acc.min, elem.min);
            acc.sum += elem.sum;
            acc.max = std::max(acc.max, elem.max);
            acc.count += elem.count;
        }
    }

    std::string s = "{";
    for(auto const& [key, val] : m) {
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
    if(!s.empty()) s[s.size() - 1] = '}';

    return s;
}

template <int N>
std::string run(std::string const& datafile) {
    return print(solve<N>(datafile));
}

template std::vector<soln_t> solve<2>(std::string const&);
template std::string run<2>(std::string const&);
template std::vector<soln_t> solve<4>(std::string const&);
template std::string run<4>(std::string const&);
template std::vector<soln_t> solve<8>(std::string const&);
template std::string run<8>(std::string const&);
template std::vector<soln_t> solve<12>(std::string const&);
template std::string run<12>(std::string const&);
template std::vector<soln_t> solve<16>(std::string const&);
template std::string run<16>(std::string const&);
template std::vector<soln_t> solve<24>(std::string const&);
template std::string run<24>(std::string const&);
template std::vector<soln_t> solve<32>(std::string const&);
template std::string run<32>(std::string const&);

} // namespace rinku::brc::threading
