#include <functional>
#include <map>
#include <regex>
#include <string>
#include <utility>

#include <docopt/docopt.h>
#include <fmt/core.h>

#include "00-baseline.h"
#include "01-parse_float.h"
#include "02-parse_int.h"
#include "03-parse_temp_branchless.h"
#include "04-parse_line_branchless.h"
#include "05-custom_map.h"
#include "06-mmapio.h"
#include "07-threading.h"
#include "08-chunking.h"
#include "09-perfect_hash.h"
#include "diskread.h"

char const COMMAND[] =
    R"(1BRC Runner

Usage:  1brc [-cq] [FILTER]
        1brc --list

Options:
    FILTER          Select the solutions to run using a regular expression, i.e. "1E5" [default: ""]
    -l, --list      List all available problems
    -c, --check     Check solution against the baseline
    -q, --quiet     Suppress output
    -h, --help      Show this screen
)";

using namespace rinku::brc;

using sut_t = std::function<std::string(std::string const&)>;
using testcase_t = std::pair<std::string, sut_t>;

std::map<std::string, testcase_t> problems = {
    {"DISKREAD_1E5",              {"resources/100k.txt", diskread::run}        },
    {"DISKREAD_1E7",              {"resources/10m.txt", diskread::run}         },
    {"DISKREAD_1E9",              {"resources/1b.txt", diskread::run}          },
    {"BASELINE_1E5",              {"resources/100k.txt", baseline::run}        },
    {"BASELINE_1E7",              {"resources/10m.txt", baseline::run}         },
    {"BASELINE_1E9",              {"resources/1b.txt", baseline::run}          },
    {"PARSE_FLOAT_1E5",           {"resources/100k.txt", parse_float::run}     },
    {"PARSE_FLOAT_1E7",           {"resources/10m.txt", parse_float::run}      },
    {"PARSE_FLOAT_1E9",           {"resources/1b.txt", parse_float::run}       },
    {"PARSE_INT_1E5",             {"resources/100k.txt", parse_int::run}       },
    {"PARSE_INT_1E7",             {"resources/10m.txt", parse_int::run}        },
    {"PARSE_INT_1E9",             {"resources/1b.txt", parse_int::run}         },
    {"PARSE_TEMP_BRANCHLESS_1E5",
     {"resources/100k.txt", parse_temp_branchless::run}                        },
    {"PARSE_TEMP_BRANCHLESS_1E7",
     {"resources/10m.txt", parse_temp_branchless::run}                         },
    {"PARSE_TEMP_BRANCHLESS_1E9",
     {"resources/1b.txt", parse_temp_branchless::run}                          },
    {"PARSE_LINE_BRANCHLESS_1E5",
     {"resources/100k.txt", parse_line_branchless::run}                        },
    {"PARSE_LINE_BRANCHLESS_1E7",
     {"resources/10m.txt", parse_line_branchless::run}                         },
    {"PARSE_LINE_BRANCHLESS_1E9",
     {"resources/1b.txt", parse_line_branchless::run}                          },
    {"CUSTOM_MAP_1E5",            {"resources/100k.txt", custom_map::run}      },
    {"CUSTOM_MAP_1E7",            {"resources/10m.txt", custom_map::run}       },
    {"CUSTOM_MAP_1E9",            {"resources/1b.txt", custom_map::run}        },
    {"MMAPIO_1E5",                {"resources/100k.txt", mmapio::run}          },
    {"MMAPIO_1E7",                {"resources/10m.txt", mmapio::run}           },
    {"MMAPIO_1E9",                {"resources/1b.txt", mmapio::run}            },
    {"THREADING_1E5_x2",          {"resources/100k.txt", threading::run<2>}    },
    {"THREADING_1E7_x2",          {"resources/10m.txt", threading::run<2>}     },
    {"THREADING_1E9_x2",          {"resources/1b.txt", threading::run<2>}      },
    {"THREADING_1E5_x4",          {"resources/100k.txt", threading::run<4>}    },
    {"THREADING_1E7_x4",          {"resources/10m.txt", threading::run<4>}     },
    {"THREADING_1E9_x4",          {"resources/1b.txt", threading::run<4>}      },
    {"THREADING_1E5_x8",          {"resources/100k.txt", threading::run<8>}    },
    {"THREADING_1E7_x8",          {"resources/10m.txt", threading::run<8>}     },
    {"THREADING_1E9_x8",          {"resources/1b.txt", threading::run<8>}      },
    {"THREADING_1E5_x12",         {"resources/100k.txt", threading::run<12>}   },
    {"THREADING_1E7_x12",         {"resources/10m.txt", threading::run<12>}    },
    {"THREADING_1E9_x12",         {"resources/1b.txt", threading::run<12>}     },
    {"THREADING_1E5_x16",         {"resources/100k.txt", threading::run<16>}   },
    {"THREADING_1E7_x16",         {"resources/10m.txt", threading::run<16>}    },
    {"THREADING_1E9_x16",         {"resources/1b.txt", threading::run<16>}     },
    {"THREADING_1E5_x24",         {"resources/100k.txt", threading::run<24>}   },
    {"THREADING_1E7_x24",         {"resources/10m.txt", threading::run<24>}    },
    {"THREADING_1E9_x24",         {"resources/1b.txt", threading::run<24>}     },
    {"THREADING_1E5_x32",         {"resources/100k.txt", threading::run<32>}   },
    {"THREADING_1E7_x32",         {"resources/10m.txt", threading::run<32>}    },
    {"THREADING_1E9_x32",         {"resources/1b.txt", threading::run<32>}     },
    {"CHUNKING_1E5_x2",           {"resources/100k.txt", chunking::run<2>}     },
    {"CHUNKING_1E7_x2",           {"resources/10m.txt", chunking::run<2>}      },
    {"CHUNKING_1E9_x2",           {"resources/1b.txt", chunking::run<2>}       },
    {"CHUNKING_1E5_x4",           {"resources/100k.txt", chunking::run<4>}     },
    {"CHUNKING_1E7_x4",           {"resources/10m.txt", chunking::run<4>}      },
    {"CHUNKING_1E9_x4",           {"resources/1b.txt", chunking::run<4>}       },
    {"CHUNKING_1E5_x8",           {"resources/100k.txt", chunking::run<8>}     },
    {"CHUNKING_1E7_x8",           {"resources/10m.txt", chunking::run<8>}      },
    {"CHUNKING_1E9_x8",           {"resources/1b.txt", chunking::run<8>}       },
    {"CHUNKING_1E5_x12",          {"resources/100k.txt", chunking::run<12>}    },
    {"CHUNKING_1E7_x12",          {"resources/10m.txt", chunking::run<12>}     },
    {"CHUNKING_1E9_x12",          {"resources/1b.txt", chunking::run<12>}      },
    {"CHUNKING_1E5_x16",          {"resources/100k.txt", chunking::run<16>}    },
    {"CHUNKING_1E7_x16",          {"resources/10m.txt", chunking::run<16>}     },
    {"CHUNKING_1E9_x16",          {"resources/1b.txt", chunking::run<16>}      },
    {"CHUNKING_1E5_x24",          {"resources/100k.txt", chunking::run<24>}    },
    {"CHUNKING_1E7_x24",          {"resources/10m.txt", chunking::run<24>}     },
    {"CHUNKING_1E9_x24",          {"resources/1b.txt", chunking::run<24>}      },
    {"CHUNKING_1E5_x32",          {"resources/100k.txt", chunking::run<32>}    },
    {"CHUNKING_1E7_x32",          {"resources/10m.txt", chunking::run<32>}     },
    {"CHUNKING_1E9_x32",          {"resources/1b.txt", chunking::run<32>}      },
    {"PERFECT_HASH_1E5_x2",       {"resources/100k.txt", perfect_hash::run<2>} },
    {"PERFECT_HASH_1E7_x2",       {"resources/10m.txt", perfect_hash::run<2>}  },
    {"PERFECT_HASH_1E9_x2",       {"resources/1b.txt", perfect_hash::run<2>}   },
    {"PERFECT_HASH_1E5_x4",       {"resources/100k.txt", perfect_hash::run<4>} },
    {"PERFECT_HASH_1E7_x4",       {"resources/10m.txt", perfect_hash::run<4>}  },
    {"PERFECT_HASH_1E9_x4",       {"resources/1b.txt", perfect_hash::run<4>}   },
    {"PERFECT_HASH_1E5_x8",       {"resources/100k.txt", perfect_hash::run<8>} },
    {"PERFECT_HASH_1E7_x8",       {"resources/10m.txt", perfect_hash::run<8>}  },
    {"PERFECT_HASH_1E9_x8",       {"resources/1b.txt", perfect_hash::run<8>}   },
    {"PERFECT_HASH_1E5_x12",      {"resources/100k.txt", perfect_hash::run<12>}},
    {"PERFECT_HASH_1E7_x12",      {"resources/10m.txt", perfect_hash::run<12>} },
    {"PERFECT_HASH_1E9_x12",      {"resources/1b.txt", perfect_hash::run<12>}  },
    {"PERFECT_HASH_1E5_x16",      {"resources/100k.txt", perfect_hash::run<16>}},
    {"PERFECT_HASH_1E7_x16",      {"resources/10m.txt", perfect_hash::run<16>} },
    {"PERFECT_HASH_1E9_x16",      {"resources/1b.txt", perfect_hash::run<16>}  },
    {"PERFECT_HASH_1E5_x24",      {"resources/100k.txt", perfect_hash::run<24>}},
    {"PERFECT_HASH_1E7_x24",      {"resources/10m.txt", perfect_hash::run<24>} },
    {"PERFECT_HASH_1E9_x24",      {"resources/1b.txt", perfect_hash::run<24>}  },
    {"PERFECT_HASH_1E5_x32",      {"resources/100k.txt", perfect_hash::run<32>}},
    {"PERFECT_HASH_1E7_x32",      {"resources/10m.txt", perfect_hash::run<32>} },
    {"PERFECT_HASH_1E9_x32",      {"resources/1b.txt", perfect_hash::run<32>}  },
};

int main(int argc, char** argv) {
    auto args = docopt::docopt(COMMAND, {argv + 1, argv + argc}, true, "");

    if(args["--list"].asBool()) {
        for(auto [id, _] : problems) {
            fmt::print("{}\n", id);
        }
    } else {
        // docopt.cpp can't set a default value for positional arguments
        std::string filter_string =
            args["FILTER"].kind() == docopt::Kind::String
                ? args["FILTER"].asString()
                : "";

        std::unordered_map<std::string, std::pair<std::string, std::string>>
            answers;

        std::regex filter(filter_string);
        for(auto const& [id, desc] : problems) {
            if(!regex_search(id, filter)) continue;
            auto const& [input, impl] = desc;
            answers[id] = std::pair(input, impl(input));
        }

        if(!args["--quiet"].asBool()) {
            for(auto const& [id, desc] : answers) {
                auto const& [input, ans] = desc;
                fmt::print("{}:\n{}\n\n", id, ans);
            }
        }

        if(args["--check"].asBool()) {
            std::unordered_map<std::string, std::string> cache;
            for(auto const& [id, desc] : answers) {
                auto const& [input, ans] = desc;
                if(cache.find(input) == cache.end()) {
                    cache[input] = parse_int::run(input);
                }
                fmt::print(
                    "{}: {}\n",
                    id,
                    cache[input] == ans ? "correct" : "incorrect"
                );
            }
        }
    }
    return 0;
}
