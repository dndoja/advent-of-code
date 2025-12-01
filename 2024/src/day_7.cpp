#include <string>
#include <vector>

#include "utils.h"

namespace day_7 {
bool is_valid_(uint64_t total, std::vector<int> values, bool with_concat) {
    auto results = std::vector<uint64_t>();
    results.push_back(values[0]);

    for (int i = 1; i < values.size(); i++) {
        uint64_t val = values[i];
        auto expanded = std::vector<uint64_t>();
        for (uint64_t prev_val : results) {
            expanded.push_back(prev_val * val);
            expanded.push_back(prev_val + val);

            if (!with_concat) continue;
            uint64_t concat = prev_val * pow(10ll, floor(log10(val)) + 1) + val;
            expanded.push_back(concat);
        }

        results = expanded;
    }

    for (uint64_t val : results) {
        if (val == total) return true;
    }

    return false;
}

uint64_t run(std::vector<std::string> const &lines, bool is_part1) {
    uint64_t result = 0;

    for (std::string line : lines) {
        std::vector<std::string> split = utils::split(line, ":");
        uint64_t total = std::stoull(split[0]);
        split = utils::split(split[1], " ");
        auto values = std::vector<int>();
        for (std::string str : split) {
            values.push_back(std::stoi(str));
        }

        if (is_valid_(total, values, !is_part1)) result += total;
    }

    return result;
}
} // namespace day_7
