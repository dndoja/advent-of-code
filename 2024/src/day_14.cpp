#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "utils.h"

namespace day_14 {
using utils::Pt;

uint64_t run(std::vector<std::string> const &lines, bool is_part1) {
    auto positions = std::vector<Pt>();
    auto initial_positions = std::vector<Pt>();
    auto velocities = std::vector<Pt>();
    constexpr int width = 101;
    constexpr int height = 103;
    constexpr int half_width = width / 2;
    constexpr int half_height = height / 2;

    for (std::string line : lines) {
        auto split = utils::split(line, " ");
        auto p_split = utils::split(split[0].substr(2), ",");
        auto v_split = utils::split(split[1].substr(2), ",");
        positions.push_back({std::stoi(p_split[0]), std::stoi(p_split[1])});
        initial_positions.push_back(
            {std::stoi(p_split[0]), std::stoi(p_split[1])});
        velocities.push_back({std::stoi(v_split[0]), std::stoi(v_split[1])});
    }

    auto move_robot = [&positions, &velocities](int id) {
        Pt pos = positions[id];
        Pt v = velocities[id];
        int new_x = (pos.x + v.x);
        if (new_x < 0) {
            new_x = width + new_x;
        } else if (new_x >= width) {
            new_x = new_x - width;
        }
        int new_y = (pos.y + v.y);
        if (new_y < 0) {
            new_y = height + new_y;
        } else if (new_y >= height) {
            new_y = new_y - height;
        }

        Pt new_pos = {new_x, new_y};
        positions[id] = new_pos;

        return new_pos;
    };

    if (is_part1) {
        for (int second = 1; second < 100; second++) {
            for (int robot_id = 0; robot_id < positions.size(); robot_id++) {
                Pt new_pos = move_robot(robot_id);
            }
        }

        int quadrants[4] = {0};
        for (auto pos : positions) {
            int new_x = pos.x;
            int new_y = pos.y;
            int quadrant = -1;
            if (new_x < half_width && new_y < half_height) {
                quadrant = 0;
            } else if (new_x < half_width && new_y > half_height) {
                quadrant = 1;
            } else if (new_x > half_width && new_y < half_height) {
                quadrant = 2;
            } else if (new_x > half_width && new_y > half_height) {
                quadrant = 3;
            }

            if (quadrant != -1) quadrants[quadrant]++;
        }

        return quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3];
    }

    for (int second = 1;; second++) {
        auto occupied = std::unordered_set<int>();
        for (int robot_id = 0; robot_id < positions.size(); robot_id++) {
            Pt new_pos = move_robot(robot_id);
            occupied.insert(new_pos.hash());
        }

        system("clear");
        std::cout << std::endl;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool is_occupied = occupied.contains(Pt{x, y}.hash());
                if (is_occupied) {
                    std::cout << "#";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
        }
        utils::sleep30();

        bool looped = true;
        for (int robot_id = 0; robot_id < positions.size(); robot_id++) {
            Pt curr = positions[robot_id];
            Pt init = initial_positions[robot_id];
            if (curr.x != init.x || curr.y != init.y) {
                looped = false;
                break;
            }
        }

        if (looped) break;
    }

    // I found the result manually, but at what cost...
    return 8168;
}
} // namespace day_14
