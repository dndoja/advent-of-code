#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "utils.h"

namespace day_4 {
int run_pt_1_(std::string input, int size) {
    const char valid_chars[] = {'X', 'M', 'A', 'S'};

    int count = 0;

    for (int i = 0; i < input.length(); i++) {
        char ch = input[i];
        if (ch != 'X') continue;

        auto point = utils::Pt::from_index(i, size);

        for (utils::Dir dir : utils::directions) {
            utils::Pt dir_offset = utils::dir_offset(dir);
            utils::Pt extent = {
                point.x + 3 * dir_offset.x,
                point.y + 3 * dir_offset.y,
            };
            if (extent.off_grid(size)) continue;

            bool is_valid = true;

            for (int offset = 1; offset <= 3; offset++) {
                utils::Pt offset_pt = {
                    point.x + offset * dir_offset.x,
                    point.y + offset * dir_offset.y,
                };
                char ch = input[offset_pt.flat(size)];

                if (valid_chars[offset] != ch) {
                    is_valid = false;
                    break;
                }
            }

            if (is_valid) count++;
        }
    }

    return count;
}

int run_pt_2_(std::string input, int size) {
    int count = 0;

    for (int i = 0; i < input.length(); i++) {
        char ch = input[i];
        if (ch != 'A') continue;

        utils::Pt center = utils::Pt::from_index(i, size);

        utils::Pt tl = {center.x - 1, center.y + 1};
        if (tl.off_grid(size)) continue;
        char ch_tl = input[tl.flat(size)];

        utils::Pt tr = {center.x + 1, center.y + 1};
        if (tr.off_grid(size)) continue;
        char ch_tr = input[tr.flat(size)];

        utils::Pt bl = {center.x - 1, center.y - 1};
        if (bl.off_grid(size)) continue;
        char ch_bl = input[bl.flat(size)];

        utils::Pt br = {center.x + 1, center.y - 1};
        if (br.off_grid(size)) continue;
        char ch_br = input[br.flat(size)];

        bool valid =
            (ch_tl == 'S' && ch_br == 'M') || (ch_tl == 'M' && ch_br == 'S');
        valid &=
            (ch_tr == 'S' && ch_bl == 'M') || (ch_tr == 'M' && ch_bl == 'S');
        if (valid) count++;
    }

    return count;
}

int run(std::vector<std::string> &lines, bool is_part1) {
    int size = lines[0].size();
    std::string full = "";
    for (std::string line : lines) {
        full += line;
    }

    if (is_part1) {
        return run_pt_1_(full, size);
    } else {
        return run_pt_2_(full, size);
    }
}
} // namespace day_4
