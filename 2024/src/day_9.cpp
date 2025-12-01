#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

namespace day_9 {
void print_ids_(std::vector<int> &ids) {
    for (int i = 0; i < ids.size() - 1; i += 2) {
        int id = ids[i];
        int repeats = ids[i + 1];

        uint64_t sum = 0;
        for (int j = 0; j < repeats; j++) {
            std::cout << id;
            sum += j;
        }
    }

    std::cout << "\n";
}

std::vector<int> pack_and_rle_(std::vector<int> &vals, std::vector<int> &gaps) {
    auto packed_rle = std::vector<int>();
    packed_rle.push_back(0);
    packed_rle.push_back(vals[0]);

    int gap_i = 0;
    int val_head = 1;
    int val_tail = vals.size() - 1;

    while (gap_i < gaps.size() && val_tail > 0 && gap_i < val_tail) {
        int gap_size = gaps[gap_i];
        int val = vals[val_tail];
        int gap_left = gap_size - val;

        if (gap_left > 0) { // gap bigger than val
            packed_rle.push_back(val_tail);
            packed_rle.push_back(val);
            gaps[gap_i] = gap_left;
            val_tail--;
        } else if (gap_left < 0) { // gap smaller than value, fragment
            int partial_fill = val + gap_left;
            packed_rle.push_back(val_tail);
            packed_rle.push_back(partial_fill);
            vals[val_tail] -= partial_fill;
            gap_i++;
            if (val_head < val_tail) {
                packed_rle.push_back(val_head);
                packed_rle.push_back(vals[val_head]);
                val_head++;
            }
        } else {
            packed_rle.push_back(val_tail);
            packed_rle.push_back(val);
            val_tail--;
            gap_i++;
            if (val_head < val_tail) {
                packed_rle.push_back(val_head);
                packed_rle.push_back(vals[val_head]);
                val_head++;
            }
        }
    }

    return packed_rle;
}

std::vector<int> defrag_and_rle_(std::vector<int> &vals,
                                 std::vector<int> &gaps) {
    auto defragged_rle = std::vector<int>();
    auto moved_vals = std::vector<bool>(vals.size(), false);
    defragged_rle.push_back(0);
    defragged_rle.push_back(vals[0]);

    int val_head = 1;
    int val_tail = vals.size() - 1;

    int gap_i = 0;
    while (gap_i < gaps.size()) {
        int gap_size = gaps[gap_i];

        if (gap_size == 0) {
            gap_i++;
            if (!moved_vals[gap_i]) {
                defragged_rle.push_back(gap_i);
                defragged_rle.push_back(vals[gap_i]);
                moved_vals[gap_i] = true;
            } else {
                defragged_rle.push_back(0);
                defragged_rle.push_back(vals[gap_i]);
            }

            continue;
        }

        int val_i = -1;
        for (int i = vals.size() - 1; i > gap_i; i--) {
            if (moved_vals[i]) continue;
            if (vals[i] <= gap_size) {
                val_i = i;
                break;
            }
        }

        if (val_i == -1) {
            gap_i++;
            defragged_rle.push_back(0);
            defragged_rle.push_back(gap_size);
            if (!moved_vals[gap_i]) {
                defragged_rle.push_back(gap_i);
                defragged_rle.push_back(vals[gap_i]);
                moved_vals[gap_i] = true;
            } else {
                defragged_rle.push_back(0);
                defragged_rle.push_back(vals[gap_i]);
            }

            continue;
        }

        int val = vals[val_i];
        defragged_rle.push_back(val_i);
        defragged_rle.push_back(val);
        moved_vals[val_i] = true;

        if (gap_size > val) {
            gaps[gap_i] -= val;
        } else {
            gap_i++;
            if (!moved_vals[gap_i]) {
                defragged_rle.push_back(gap_i);
                defragged_rle.push_back(vals[gap_i]);
                moved_vals[gap_i] = true;
            } else {
                defragged_rle.push_back(0);
                defragged_rle.push_back(vals[gap_i]);
            }
        }
    }

    return defragged_rle;
}

uint64_t run(std::vector<std::string> const &lines, bool is_part1) {
    auto vals = std::vector<int>();
    auto gaps = std::vector<int>();

    for (int i = 0; i < lines[0].size(); i++) {
        int val = lines[0][i] - '0';
        bool is_gap = (i & 1) == 1;
        if (is_gap) {
            gaps.push_back(val);
        } else {
            vals.push_back(val);
        }
    }

    std::vector<int> ids =
        is_part1 ? pack_and_rle_(vals, gaps) : defrag_and_rle_(vals, gaps);
    uint64_t checksum = 0;
    int segment_start = 0;

    for (int i = 0; i < ids.size() - 1; i += 2) {
        int id = ids[i];
        int repeats = ids[i + 1];
        uint64_t sum =
            ceill((repeats / 2.0) * (2 * segment_start + (repeats - 1)));

        checksum += sum * id;
        segment_start += repeats;
    }

    return checksum;
}
} // namespace day_9
