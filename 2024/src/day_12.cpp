#include <climits>
#include <iostream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "utils.h"

namespace day_12 {
using utils::Pt;

struct Cluster {
    std::vector<Pt> points;
    std::unordered_set<int> hashes;

    int min_x = INT_MAX;
    int max_x = 0;
    int min_y = INT_MAX;
    int max_y = 0;

    void add(Pt point) {
        points.push_back(point);
        hashes.insert(point.hash());
        if (point.x < min_x) min_x = point.x;
        if (point.x > max_x) max_x = point.x;
        if (point.y < min_y) min_y = point.y;
        if (point.y > max_y) max_y = point.y;
    }

    bool contains(Pt &point, int tolerance = 0) {
        return point.x >= min_x - tolerance && point.x <= max_x + tolerance &&
               point.y >= min_y - tolerance && point.y <= max_y + tolerance;
    }

    void print(Pt curr, char ch = '#', int padding = 0, bool clear = false,
               bool manual = false) {
        if (clear) system("clear");

        for (int y = max_y + padding; y >= min_y - padding; y--) {
            for (int x = min_x - padding; x <= max_x + padding; x++) {
                Pt pt = {x, y};
                if (curr.x == x && curr.y == y) {
                    std::cout << "*";
                } else if (hashes.contains(pt.hash())) {
                    std::cout << ch;
                } else {
                    std::cout << ".";
                }
            }
            std::cout << std::endl;
        }

        if (clear) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (manual) {
                std::string _;
                std::cin >> _;
            }
        }
    }

    Pt scale(Pt original) { return {original.x - min_x, original.y - min_y}; }

    void contract(int amount) {
        min_x += amount;
        max_x -= amount;
        min_y += amount;
        max_y -= amount;
    }

    void expand(int amount) {
        min_x -= amount;
        max_x += amount;
        min_y -= amount;
        max_y += amount;
    }

    int width() { return max_x - min_x; }
    int height() { return max_y - min_y; }
};

int count_cluster_vertices(Cluster &cluster, int grid_size) {
    if (cluster.points.size() == 1) return 4;

    Pt start = cluster.points[0];
    auto stack = std::stack<Pt>();
    auto visited = std::unordered_set<int>();
    stack.push(start);

    auto vertices = std::unordered_map<int, int>();

    while (!stack.empty()) {
        Pt curr = stack.top();
        stack.pop();

        if (visited.contains(curr.hash())) continue;
        visited.insert(curr.hash());

        for (utils::Dir dir : utils::directions_cartesian) {
            utils::Pt neighbour = curr + utils::dir_offset(dir);

            if (!cluster.contains(neighbour, 1)) continue;
            if (!cluster.hashes.contains(neighbour.hash())) continue;
            if (visited.contains(neighbour.hash())) continue;

            stack.push(neighbour);
        }

        for (utils::Dir diagonal : utils::directions_diagonal) {
            Pt l0 = curr + utils::dir_offset(diagonal);
            Pt l1 = curr + utils::dir_offset(utils::rot45(diagonal, true));
            Pt l2 = curr + utils::dir_offset(utils::rot45(diagonal, false));
            bool filled_1 = cluster.hashes.contains(l1.hash());
            bool filled_2 = cluster.hashes.contains(l2.hash());

            if (filled_1 == filled_2 &&
                (!filled_1 || !cluster.hashes.contains(l0.hash()))) {
                vertices[curr.hash()]++;
            }
        }
    }

    int vertice_count = 0;
    for (auto [_, count] : vertices) {
        vertice_count += count;
    }

    return vertice_count;
}

uint64_t run(std::vector<std::string> const &lines, bool is_part1) {
    int size = lines.size();
    auto ungrouped_indices = std::unordered_set<int>();
    auto clusters = std::unordered_map<int, Cluster>();
    auto plant_types = std::vector<char>(size * size);
    auto collisions = std::vector<int>(size * size, 4);

    for (int row = 0; row < size; row++) {
        for (int x = 0; x < size; x++) {
            char ch = lines[row][x];
            int y = size - row - 1;
            Pt pos = {x, y};
            int index = pos.flat(size);
            ungrouped_indices.insert(pos.flat(size));
            plant_types[index] = ch;
        }
    }

    while (!ungrouped_indices.empty()) {
        int start_index = *std::begin(ungrouped_indices);
        Pt start_point = Pt::from_index(start_index, size);

        auto stack = std::stack<utils::Pt>();
        auto visited = std::unordered_set<int>();
        stack.push(start_point);

        while (!stack.empty()) {
            utils::Pt curr = stack.top();
            int curr_index = curr.flat(size);
            stack.pop();

            if (visited.contains(curr.hash())) continue;
            visited.insert(curr.hash());

            char curr_type = plant_types[curr.flat(size)];

            for (utils::Dir dir : utils::directions_cartesian) {
                utils::Pt neighbour = curr + utils::dir_offset(dir);
                if (neighbour.off_grid(size)) continue;
                char neighbour_type = plant_types[neighbour.flat(size)];

                if (neighbour_type == curr_type) {
                    collisions[curr_index]--;

                    if (!visited.contains(neighbour.hash())) {
                        stack.push(neighbour);
                    }
                }
            }

            ungrouped_indices.erase(curr_index);
            clusters[start_index].add(curr);
        }
    }

    int total = 0;

    if (is_part1) {
        for (auto [i, cluster] : clusters) {
            int perimeter = 0;
            for (Pt pt : cluster.points) {
                perimeter += collisions[pt.flat(size)];
            }
            total += perimeter * cluster.points.size();
        }
    } else {
        for (auto [i, cluster] : clusters) {
            int multiplier = count_cluster_vertices(cluster, size);
            total += multiplier * cluster.points.size();
        }
    }

    return total;
}
} // namespace day_12
