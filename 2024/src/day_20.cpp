#include "utils.h"
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_set>

namespace day_20 {
using utils::Pt;

struct Map {
    int size;
    std::vector<bool> obstacles;
    std::vector<int> weights;

    bool is_obstacle(Pt const &pt) const { return obstacles[pt.flat(size)]; }
    int weight(Pt const &pt) const { return weights[pt.flat(size)]; }

    void print(Pt curr) {
        int curr_i = curr.flat(size);

        for (int y = size - 1; y >= 0; y--) {
            for (int x = 0; x < size; x++) {
                Pt pt = {x, y};
                int i = pt.flat(size);
                if (i == curr_i) {
                    std::cout << "$$ ";
                } else if (obstacles[i]) {
                    std::cout << "## ";
                } else if (weights[i] == INT_MAX) {
                    std::cout << "--";
                } else if (weights[i] < 10) {
                    std::cout << weights[i] << "  ";
                } else {
                    std::cout << weights[i] << " ";
                }
            }
            std::cout << std::endl;
        }
    }
};

struct Cheat {
    Pt start, end;
    int saved;

    uint64_t hash() {
        return 0ul | start.hash() | ((uint64_t)end.hash() << 32);
    }
};

/// Some caching would be good here because it's giga slow but whatever
void find_cheat_exits(Map const &map,
                      std::unordered_map<uint64_t, Cheat> &cheats, Pt &start,
                      int max_dist = 20) {
    int start_weight = map.weight(start);
    auto queue = std::queue<Pt>();
    queue.push({start});
    auto visited = std::unordered_set<int>();

    while (!queue.empty()) {
        Pt curr = queue.front();
        queue.pop();

        if (visited.contains(curr.hash())) continue;
        visited.insert(curr.hash());

        // std::cout << curr.pt.to_string() << " " << curr.depth << std::endl;

        for (auto dir : utils::directions_cartesian) {
            Pt next = curr + utils::dir_offset(dir);
            if (next.off_grid(map.size) || visited.contains(next.hash())) {
                continue;
            }

            if (map.is_obstacle(next)) {
                queue.push(next);
            } else {
                int dist = utils::manhattan(start, next);
                if (dist <= max_dist) {
                    int saved = start_weight - map.weight(next) - dist;
                    Cheat cheat = {start, next, saved};
                    cheats[cheat.hash()] = cheat;
                }
            }
        }
    }
}

int run(std::vector<std::string> &lines, bool is_part1) {
    int size = lines.size();
    Map map = {
        size,
        std::vector<bool>(size * size, false),
        std::vector<int>(size * size, INT_MAX),
    };
    Pt start_pos, goal_pos;

    for (int row = 0; row < size; row++) {
        for (int x = 0; x < size; x++) {
            char ch = lines[row][x];
            Pt pos = {x, size - row - 1};

            if (ch == 'S') {
                start_pos = pos;
            } else if (ch == 'E') {
                goal_pos = pos;
            }

            int i = pos.flat(size);
            map.obstacles[i] = ch == '#';
            map.weights[i] = INT_MAX;
        }
    }

    map.weights[goal_pos.flat(size)] = 0;

    bool changed = true;
    for (int _ = 0; _ < size * size - 1; _++) {
        for (int i = 0; i < size * size; i++) {
            if (map.obstacles[i]) continue;

            int weight = map.weights[i];
            if (weight == INT_MAX) continue;

            Pt curr = Pt::from_index(i, size);

            for (utils::Dir dir : utils::directions_cartesian) {
                Pt neighbour = curr + utils::dir_offset(dir);
                if (neighbour.off_grid(size)) continue;

                int neighbour_i = neighbour.flat(size);
                if (map.obstacles[neighbour_i]) continue;

                int n_weight = map.weights[neighbour_i];
                if (n_weight > weight + 1) {
                    map.weights[neighbour_i] = weight + 1;
                }
            }
        }
    }

    auto stack = std::stack<int>();
    stack.push(start_pos.flat(size));
    auto visited = std::vector<bool>(size * size, false);
    auto cheats = std::unordered_map<uint64_t, Cheat>();
    int max_dist = is_part1 ? 2 : 20;

    while (!stack.empty()) {
        int curr_i = stack.top();
        stack.pop();

        if (visited[curr_i]) continue;
        visited[curr_i] = true;

        Pt curr = Pt::from_index(curr_i, size);
        int curr_w = map.weights[curr_i];

        for (utils::Dir dir : utils::directions_cartesian) {
            Pt neighbour = curr + utils::dir_offset(dir);
            if (neighbour.off_grid(size) || map.is_obstacle(neighbour)) {
                continue;
            }

            int n_index = neighbour.flat(size);
            int n_weight = map.weights[n_index];
            if (n_weight < curr_w) stack.push(n_index);
        }

        find_cheat_exits(map, cheats, curr, max_dist);
    }

    int total = 0;
    for (auto [_, cheat] : cheats) {
        if (cheat.saved >= 100) total++;
    }

    return total;
}
} // namespace day_20
