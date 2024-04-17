#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <sys/mman.h>

#include "09-perfect_hash.h"

namespace rinku::brc::perfect_hash {

// Perfect hash generated with gperf
int64_t map_t::phash(char const* str, int64_t len) {
    int64_t hval = len;

    switch(hval) {
    default: hval += hash_lookup_table[static_cast<unsigned char>(str[4])];
    /*FALLTHROUGH*/
    case 4:
    case 3: hval += hash_lookup_table[static_cast<unsigned char>(str[2])];
    /*FALLTHROUGH*/
    case 2:
    case 1:
        hval += hash_lookup_table[static_cast<unsigned char>(str[0])];
        break;
    }
    return hval + hash_lookup_table[static_cast<unsigned char>(str[len - 1])];
}

void map_t::add(char const* name, int64_t len, int64_t temp) {
    unsigned int slot = phash(name, len);
    slots[slot].min = std::min(slots[slot].min, temp);
    slots[slot].sum += temp;
    slots[slot].max = std::max(slots[slot].max, temp);
    slots[slot].count++;
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

static constexpr int64_t CHUNK_SIZE = 256 * 1024;
static std::atomic<int64_t> cursor;

void work(soln_t& m, char* buf, char* end) {
    while(true) {
        int64_t start = cursor.fetch_add(CHUNK_SIZE, std::memory_order_relaxed);
        char* chunk_end = std::min(end, &buf[start + CHUNK_SIZE]);

        if(&buf[start] >= end) break;

        if(start > 0) {
            while(buf[start - 1] != '\n')
                start++;
        }

        uint64_t offset = 0;
        for(char* line = &buf[start]; line < chunk_end;) {
            int64_t next8 = *(int64_t*)&line[offset];
            int64_t semicolon_one_hot = find_semicolon(next8);

            if(!semicolon_one_hot) {
                offset += 8;
            } else {
                int64_t semicolon_bit_idx = __builtin_ctzll(semicolon_one_hot);
                offset += semicolon_bit_idx / 8 + 1;
                int64_t temp = *(int64_t*)(&line[offset]);
                int64_t dot_idx = __builtin_ctzll(~temp & DOT_DIFFERENTIATOR);
                int64_t value = parse_temperature(temp, dot_idx);

                m.add(line, offset - 1, value);

                line += offset + dot_idx / 8 + 3;
                offset = 0;
            }
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

    std::vector<soln_t> solns(N, soln_t(MAX_HASH_VALUE + 1));
    std::vector<std::thread> threads;

    cursor = 0;
    for(int i = 0; i < N; i++)
        threads.emplace_back(work, std::ref(solns[i]), buf, &buf[file_length]);

    for(int i = 0; i < N; i++)
        threads[i].join();

    munmap(buf, file_length);
    return solns;
}

std::string print(std::vector<soln_t> const& solns) {
    std::vector<std::pair<char const*, elem_t>> acc(MAX_HASH_VALUE + 1);

    for(int i = 0; i < (int)acc.size(); i++)
        acc[i].first = cities[i];

    for(auto const& soln : solns) {
        for(int i = 0; i < (int)soln.slots.size(); i++) {
            elem_t& elem = acc[i].second;
            elem.min = std::min(elem.min, soln.slots[i].min);
            elem.sum += soln.slots[i].sum;
            elem.max = std::max(elem.max, soln.slots[i].max);
            elem.count += soln.slots[i].count;
        }
    }

    std::sort(acc.begin(), acc.end(), [](auto const& a, auto const& b) {
        return ((a.second.count == 0) != (b.second.count == 0))
                   ? a.second.count > b.second.count
                   : strcmp(a.first, b.first) < 0;
    });

    std::string s = "{";
    for(int i = 0; i < (int)acc.size() && acc[i].second.count > 0; i++) {
        auto const& [key, val] = acc[i];
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

} // namespace rinku::brc::perfect_hash
