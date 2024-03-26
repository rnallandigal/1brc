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
#include "03-parse_branchless.h"
#include "diskread.h"

char const COMMAND[] =
    R"(1BRC Runner

Usage:  1brc [--check] [FILTER]
        1brc --list

Options:
    FILTER          Select the solutions to run using a regular expression, i.e. "1E5" [default: ""]
    -l, --list      List all available problems
    -c, --check     Check solution against the baseline
    -h, --help      Show this screen
)";

using namespace rinku::brc;

#define PRINT_SOLUTIONS(name, namespace) fmt::print("BRC_name_1E5")

void blah() {}

std::map<
    std::string,
    std::pair<std::string, std::function<std::string(std::string const&)>>>
    problems = {
        {"BRC_DISKREAD_1E5",         {"resources/100k.txt", diskread::run}      },
        {"BRC_DISKREAD_1E7",         {"resources/10m.txt", diskread::run}       },
        {"BRC_DISKREAD_1E9",         {"resources/1b.txt", diskread::run}        },
        {"BRC_BASELINE_1E5",         {"resources/100k.txt", baseline::run}      },
        {"BRC_BASELINE_1E7",         {"resources/10m.txt", baseline::run}       },
        {"BRC_BASELINE_1E9",         {"resources/1b.txt", baseline::run}        },
        {"BRC_PARSE_FLOAT_1E5",      {"resources/100k.txt", parse_float::run}   },
        {"BRC_PARSE_FLOAT_1E7",      {"resources/10m.txt", parse_float::run}    },
        {"BRC_PARSE_FLOAT_1E9",      {"resources/1b.txt", parse_float::run}     },
        {"BRC_PARSE_INT_1E5",        {"resources/100k.txt", parse_int::run}     },
        {"BRC_PARSE_INT_1E7",        {"resources/10m.txt", parse_int::run}      },
        {"BRC_PARSE_INT_1E9",        {"resources/1b.txt", parse_int::run}       },
        {"BRC_PARSE_BRANCHLESS_1E5",
         {"resources/100k.txt", parse_branchless::run}                          },
        {"BRC_PARSE_BRANCHLESS_1E7",
         {"resources/10m.txt", parse_branchless::run}                           },
        {"BRC_PARSE_BRANCHLESS_1E9", {"resources/1b.txt", parse_branchless::run}
        },
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

        if(args["--check"].asBool()) {
            std::unordered_map<std::string, std::string> cache;
            for(auto const& [id, desc] : answers) {
                auto const& [input, ans] = desc;
                if(cache.find(input) == cache.end()) {
                    cache[input] = baseline::run(input);
                }
                fmt::print(
                    "{}: {};\n{}: {}\n",
                    id,
                    ans,
                    id,
                    cache[input] == ans ? "correct" : "incorrect"
                );
            }
        }
    }
    return 0;
}
