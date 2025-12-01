#include <iostream>
#include "utils.h"

namespace day_25 {
int run(std::vector<std::string> &lines, bool is_part1) {
    std::vector<int> locks = {};
    std::vector<int> keys = {};

    for (int start = 0; start < lines.size(); start += 8) {
        int bitmap = 0;
        bool is_key = false;

        for (int i = start; i < start + 7; i++) {
            std::string line = lines[i];
            int mask = 0;
            for (int x = 0; x < line.size(); x++) {
                if (line[x] == '.') continue;
                mask |= 1 << x * 4;
            }

            if (i == start && mask == 0) is_key = true;
            bitmap += mask;
        }

        if (is_key) {
            keys.push_back(bitmap);
        } else {
            locks.push_back(bitmap);
        }
    }

    int result = 0;
    for (int key : keys) {
        for (int lock : locks) {
            constexpr int mask = 0b10001000100010001000;
            if (((key + lock) & mask) == 0) result++;
        }
    }

    return result;
}
} // namespace day_25
