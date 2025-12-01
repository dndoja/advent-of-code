#include <algorithm>
#include <iostream>
#include <vector>

#include "utils.h"

namespace day_1 {
uint run_pt1_(uint length, uint values_l[], uint values_r[]) {
    std::sort(values_l, values_l + length);
    std::sort(values_r, values_r + length);

    int diff = 0;
    for (int i = 0; i < length; i++) {
        int l = values_l[i];
        int r = values_r[i];
        diff += abs(r - l);
    }

    return diff;
}

uint run_pt2_(uint length, uint values_l[], uint values_r[]) {
    std::unordered_map<uint, uint> r_counts = std::unordered_map<uint, uint>();
    for (int i = 0; i < length; i++) {
        uint val = values_r[i];
        r_counts[val] += 1;
    }

    uint similarity = 0;
    for (int i = 0; i < length; i++) {
        uint val = values_l[i];
        uint count = r_counts[val];
        similarity += val * count;
    }

    return similarity;
}

int run(std::vector<std::string> const &lines, bool is_part1) {
    uint line_count = lines.size();
    uint values_r[line_count];
    uint values_l[line_count];

    for (int i = 0; i < line_count; i++) {
        std::vector<std::string> split = utils::split(lines[i], " ");
        values_l[i] = stoi(split[0]);
        values_r[i] = stoi(split[1]);
    }

    if (is_part1) {
        return run_pt1_(line_count, values_l, values_r);
    } else {
        return run_pt2_(line_count, values_l, values_r);
    }
}
} // namespace day_1
