#include <iostream>
#include <string>
#include <sys/_types/_int64_t.h>
#include <thread>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "utils.h"

namespace day_6 {
using namespace utils;

constexpr bool log = false;

struct Move {
    Pt loc;
    Dir dir;

    int hash() { return dir << 24 | loc.hash(); }
};

void print_matrix_(int size, bool obstacles[], std::unordered_set<int> visited,
                   Pt curr_pos, Dir curr_dir, Pt fake_obstacle_loc) {
    system("clear");
    for (int i_row = 0; i_row < size; i_row++) {
        for (int x = 0; x < size; x++) {
            int y = size - i_row - 1;
            Pt pos = {x, y};
            int i = pos.flat(size);
            if (curr_pos.flat(size) == i) {
                switch (curr_dir) {
                case utils::NORTH:
                    std::cout << "^";
                    break;
                case utils::EAST:
                    std::cout << ">";
                    break;
                case utils::SOUTH:
                    std::cout << "V";
                    break;
                case utils::WEST:
                    std::cout << "<";
                    break;
                default:
                    break;
                }
            } else if (visited.contains(pos.hash())) {
                std::cout << "X";
            } else if (obstacles[i]) {
                std::cout << "#";
            } else if (fake_obstacle_loc.flat(size) == i) {
                std::cout << "*";
            } else {
                std::cout << ".";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int run(std::vector<std::string> const &lines, bool is_part1) {
    int size = lines.size();
    bool obstacles[size * size];
    Pt start_pos;
    Dir start_dir;

    for (int row = 0; row < size; row++) {
        for (int x = 0; x < size; x++) {
            char ch = lines[row][x];
            int y = size - row - 1;
            Pt pos = {x, y};
            bool is_obstacle = false;

            switch (ch) {
            case '#':
                is_obstacle = true;
                break;
            case '<':
                start_dir = WEST;
                start_pos = pos;
                break;
            case '>':
                start_dir = EAST;
                start_pos = pos;
                break;
            case '^':
                start_dir = NORTH;
                start_pos = pos;
                break;
            case 'V':
                start_dir = SOUTH;
                start_pos = pos;
                break;
            default:
                break;
            }

            obstacles[pos.flat(size)] = is_obstacle;
        }
    }

    Pt curr_pos = start_pos;
    Dir curr_dir = start_dir;
    auto visited = std::unordered_set<int>();
    auto moves = std::vector<Move>();
    visited.insert(curr_pos.hash());

    while (true) {
        Pt next_pos = curr_pos + dir_offset(curr_dir);

        if (next_pos.off_grid(size)) break;

        if (obstacles[next_pos.flat(size)]) {
            curr_dir = rot90(curr_dir);
            continue;
        }

        moves.push_back({curr_pos, curr_dir});
        curr_pos = next_pos;
        visited.insert(next_pos.hash());
    }

    if (is_part1) return visited.size();

    auto looping_obstacles = std::unordered_set<int>();

    for (Move replayed : moves) {
        Pt new_obstacle_loc = replayed.loc + dir_offset(replayed.dir);

        if (log) {
            print_matrix_(size, obstacles, std::unordered_set<int>(),
                          replayed.loc, replayed.dir, new_obstacle_loc);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        if (new_obstacle_loc.off_grid(size)) continue;
        if (new_obstacle_loc.hash() == start_pos.hash()) continue;

        Pt curr = start_pos;
        Dir curr_dir = start_dir;
        auto seen_obstacles = std::unordered_set<int>();
        auto visited = std::unordered_set<int>();

        while (true) {
            Pt next = curr + dir_offset(curr_dir);

            if (next.off_grid(size)) break;

            if (next.hash() == new_obstacle_loc.hash() ||
                obstacles[next.flat(size)]) {

                int obstacle_id = Move{next, curr_dir}.hash();

                if (seen_obstacles.contains(obstacle_id)) {
                    looping_obstacles.insert(new_obstacle_loc.hash());
                    break;
                }

                seen_obstacles.insert(obstacle_id);
                curr_dir = rot90(curr_dir);
                continue;
            }

            curr = next;
            visited.insert(curr.hash());

            if (log) {
                print_matrix_(size, obstacles, visited, replayed.loc,
                              replayed.dir, new_obstacle_loc);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }

        if (log) {
            std::cout << "\nDone, next\n";
            std::string _;
            std::cin >> _;
        }
    }

    return looping_obstacles.size();
}
} // namespace day_6
