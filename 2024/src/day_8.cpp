#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>

#include "utils.h"

namespace day_8 {
using utils::Pt;
int run(std::vector<std::string> const &lines, bool is_part1) {
    auto antennas = std::unordered_map<char, std::vector<Pt>>();
    int size = lines[0].size();

    for (int row = 0; row < size; row++) {
        for (int x = 0; x < size; x++) {
            char ch = lines[row][x];
            if (ch == '.') continue;
            antennas[ch].push_back({x, size - row - 1});
        }
    }

    auto antinodes = std::unordered_set<int>();
    auto antenna_lines = std::vector<std::tuple<Pt, Pt>>();

    for (auto &[freq, locs] : antennas) {
        for (int i_curr = 0; i_curr < locs.size() - 1; i_curr++) {
            Pt curr = locs[i_curr];
            for (int i_next = i_curr + 1; i_next < locs.size(); i_next++) {
                Pt next = locs[i_next];
                Pt antinode = {next.x + (next.x - curr.x),
                               next.y + (next.y - curr.y)};
                if (!antinode.off_grid(size)) antinodes.insert(antinode.hash());

                antinode = {curr.x + (curr.x - next.x),
                            curr.y + (curr.y - next.y)};
                if (!antinode.off_grid(size)) antinodes.insert(antinode.hash());

                antenna_lines.push_back({curr, next});
            }
        }
    }

    if (is_part1) return antinodes.size();

    int part_2_antinodes_count = 0;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            for (auto [a, b] : antenna_lines) {
                double x_prod =
                    (b.x - a.x) * (y - a.y) - (b.y - a.y) * (x - a.x);
                if (x_prod != 0) continue;
                part_2_antinodes_count++;
                break;
            }
        }
    }

    return part_2_antinodes_count;
}
} // namespace day_8
