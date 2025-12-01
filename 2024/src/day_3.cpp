#include <string>
#include <vector>

#include "utils.h"

namespace day_3 {
int run_pt_1_(std::vector<std::string> &lines) {
    int result = 0;

    for (std::string line : lines) {
        std::vector<std::string> split = utils::split(line, ",");
        if (split.size() == 1) continue;
        result += std::stoi(split[0]) * std::stoi(split[1]);
    }

    return result;
}

int run_pt_2_(std::vector<std::string> &lines) {
    int result = 0;
    bool doing = true;

    for (std::string line : lines) {
        if (line == "do") {
            doing = true;
            continue;
        }

        if (line == "don't") {
            doing = false;
            continue;
        }

        if (!doing) continue;

        std::vector<std::string> split = utils::split(line, ",");
        result += std::stoi(split[0]) * std::stoi(split[1]);
    }

    return result;
}

int run(std::vector<std::string> &lines, bool is_part1) {
    if (is_part1) {
        return run_pt_1_(lines);
    } else {
        return run_pt_2_(lines);
    }
}
} // namespace day_3
