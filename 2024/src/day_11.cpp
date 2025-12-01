#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

namespace day_11 {
template <int Max>
void expandInto2(std::unordered_map<uint64_t, uint64_t> &out, uint64_t stone,
                 int step) {
    int next_step = step + 1;

    if (step == Max) {
        out[stone]++;
        return;
    }

    if (stone != 0) {
        int num_digits = floor(log10(stone)) + 1;
        if ((num_digits & 1) == 0) {
            uint64_t mul = pow(10ll, num_digits / 2);
            uint64_t l = stone / mul;
            uint64_t r = stone - l * mul;

            expandInto2<Max>(out, l, next_step);
            expandInto2<Max>(out, r, next_step);
        } else {
            uint64_t multipled = stone * 2024;
            expandInto2<Max>(out, multipled, next_step);
        }
    } else {
        expandInto2<Max>(out, 1, next_step);
    }
}

uint64_t run(std::vector<std::string> const &lines, bool is_part1) {
    auto stones = std::vector<uint64_t>();
    for (std::string str : utils::split(lines[0], " ")) {
        stones.push_back(std::stoull(str));
    }

    auto out1 = std::unordered_map<uint64_t, uint64_t>();
    for (uint64_t stone : stones) {
        expandInto2<25>(out1, stone, 0);
    }

    if (is_part1) {
        int total = 0;
        for (auto [_, val] : out1) {
            total += val;
        }

        return total;
    }

    auto out2 = std::unordered_map<uint64_t, uint64_t>();
    for (auto [stone, mul] : out1) {
        auto temp = std::unordered_map<uint64_t, uint64_t>();
        expandInto2<25>(temp, stone, 0);
        for (auto [k, v] : temp) {
            out2[k] += v * mul;
        }
    }

    auto out3 = std::unordered_map<uint64_t, uint64_t>();
    for (auto [stone, mul] : out2) {
        auto temp = std::unordered_map<uint64_t, uint64_t>();
        expandInto2<25>(temp, stone, 0);
        for (auto [k, v] : temp) {
            out3[k] += v * mul;
        }
    }

    uint64_t part2_result = 0;
    for (auto [key, val] : out3) {
        part2_result += val;
    }

    return part2_result;
}
} // namespace day_11
