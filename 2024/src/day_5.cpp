#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "utils.h"

namespace day_5 {
int run(std::vector<std::string> &lines, bool is_part1) {
    auto rows = std::vector<std::vector<int>>();
    auto rules = std::unordered_map<int, std::unordered_set<int>>();

    for (std::string line : lines) {
        std::vector<std::string> split = utils::split(line, "|");

        if (split.size() == 2) {
            int l = std::stoi(split[0]);
            int r = std::stoi(split[1]);
            rules[r].insert(l);
            continue;
        }

        split = utils::split(line, ",");
        if (split.empty()) continue;

        auto numbers = std::vector<int>();
        for (std::string num_str : split) {
            numbers.push_back(std::stoi(num_str));
        }

        rows.push_back(numbers);
    }

    int pt1_result = 0;
    auto invalid_rows = std::vector<std::vector<int>>();

    for (std::vector<int> values : rows) {
        auto blacklist = std::unordered_set<int>();
        bool valid = true;
        for (int i = 0; i < values.size(); i++) {
            int val = values[i];
            if (blacklist.contains(val)) {
                valid = false;
                break;
            }

            auto forbidden_values = rules.find(val);
            if (forbidden_values == rules.end()) continue;
            for (int forbidden_val : forbidden_values->second) {
                blacklist.insert(forbidden_val);
            }
        }

        if (valid) {
            pt1_result += values[values.size() / 2];
        } else {
            invalid_rows.push_back(values);
        }
    }

    if (is_part1) return pt1_result;

    int pt2_result = 0;
    for (std::vector<int> values : invalid_rows) {
        auto sorted = std::vector<int>();

        for (int curr : values) {
            std::unordered_set<int> right_bounds = rules[curr];

            if (right_bounds.empty()) {
                sorted.push_back(curr);
                continue;
            }

            bool inserted = false;
            for (int i = 0; i < sorted.size(); i++) {
                if (!right_bounds.contains(sorted[i])) continue;

                sorted.insert(sorted.begin() + i, curr);
                inserted = true;
                break;
            }

            if (!inserted) sorted.push_back(curr);
        }

        pt2_result += sorted[sorted.size() / 2];
    }

    return pt2_result;
}
} // namespace day_5
