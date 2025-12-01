#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#include "utils.h"

namespace day_10 {
int run(std::vector<std::string> const &lines, bool is_part1) {
    int size = lines[0].size();
    int matrix[size * size];
    auto path_starts = std::vector<utils::Pt>();

    for (int row = 0; row < size; row++) {
        for (int x = 0; x < size; x++) {
            int val = lines[row][x] - '0';
            int y = size - row - 1;
            utils::Pt pt = {x, y};
            matrix[pt.flat(size)] = val;
            if (val == 0) path_starts.push_back(pt);
        }
    }

    int result = 0;

    for (utils::Pt start : path_starts) {
        auto queue = std::queue<utils::Pt>();
        auto visited = std::unordered_set<int>();
        queue.push(start);

        while (!queue.empty()) {
            utils::Pt curr = queue.front();
            queue.pop();

            if (is_part1) {
                if (visited.contains(curr.hash())) continue;
                visited.insert(curr.hash());
            }

            int curr_val = matrix[curr.flat(size)];
            if (curr_val == 9) {
                result++;
                continue;
            }

            for (utils::Dir dir : utils::directions_cartesian) {
                utils::Pt neighbour = curr + utils::dir_offset(dir);
                if (neighbour.off_grid(size)) continue;
                if (is_part1 && visited.contains(neighbour.hash())) continue;

                int neighbour_val = matrix[neighbour.flat(size)];
                if (neighbour_val - curr_val != 1) continue;
                queue.push(neighbour);
            }
        }
    }

    return result;
}
} // namespace day_10
