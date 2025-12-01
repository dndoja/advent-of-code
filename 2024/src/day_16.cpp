#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>

#include "utils.h"

namespace day_16 {
using utils::Dir;
using utils::Pt;

std::string dir_str(Dir dir) {
    switch (dir) {
    case utils::NORTH:
        return "^";
    case utils::EAST:
        return ">";
    case utils::SOUTH:
        return "v";
    case utils::WEST:
        return "<";
    default:
        return "X";
    }
}

struct MapInfo {
    int size;
    std::vector<bool> obstacles;
    Pt goal_loc;
    Pt start_loc;
    Dir start_dir;

    void print(Pt curr_loc, Dir curr_dir) {
        for (int y = size - 1; y >= 0; y--) {
            for (int x = 0; x < size; x++) {
                Pt pt = {x, y};

                if (pt.hash() == curr_loc.hash()) {
                    std::cout << dir_str(curr_dir);
                } else if (pt.hash() == goal_loc.hash()) {
                    std::cout << "$";
                } else if (obstacles[pt.flat(size)]) {
                    std::cout << "#";
                } else {
                    std::cout << ".";
                }
            }

            std::cout << std::endl;
        }
    }
};

struct Path {
    std::vector<Pt> locs;
    int score = -1;
};

struct Node {
    Pt loc;
    Dir dir;
    int score, heuristic, steps, turns;
    Node *parent;

    int total() { return steps + turns * 1000; }
    int f() const { return score + heuristic; }

    std::string to_string() {
        return dir_str(dir) + " " + std::to_string(loc.x) + ":" +
               std::to_string(loc.y) + " (" + std::to_string(score) + ")";
    }
};

struct NodeArena {
    Node nodes[150 * 150];
    int tail = -1;

    Node *make(Node node) {
        nodes[++tail] = node;
        return &(nodes[tail]);
    }
};

int get_turns(Dir d1, Dir d2) {
    if (d1 == d2) return 0;

    if (d1 == Dir::NORTH && d2 == Dir::SOUTH) return 2;
    if (d1 == Dir::SOUTH && d2 == Dir::NORTH) return 2;
    if (d1 == Dir::EAST && d2 == Dir::WEST) return 2;
    if (d1 == Dir::WEST && d2 == Dir::EAST) return 2;

    return 1;
}

Path pathfind(MapInfo &map, Pt skipped, bool print = false) {
    NodeArena arena = {};
    Node *goal = nullptr;
    auto closed = std::unordered_set<int>();
    auto scores = std::unordered_map<int, int>();
    auto comp = [](const Node *a, const Node *b) { return a->f() > b->f(); };
    auto open =
        std::priority_queue<Node *, std::vector<Node *>, decltype(comp)>(comp);
    open.push(arena.make({map.start_loc, map.start_dir, 0, 0, 0, 0, nullptr}));

    while (!open.empty()) {
        Node *curr_ptr = open.top();
        Node curr = *curr_ptr;

        if (curr.loc.hash() == map.goal_loc.hash()) {
            goal = curr_ptr;
            break;
        }

        if (print) {
            system("clear");
            map.print(curr.loc, curr.dir);
            std::cout << curr_ptr << " " << curr.loc.x << ":" << curr.loc.y
                      << std::endl;
            std::cout << curr.score << " " << curr.heuristic << std::endl;
            std::cout << "________________" << std::endl;
            for (int i = 0; i < arena.tail; i++) {
                std::cout << &arena.nodes[i] << " "
                          << arena.nodes[i].to_string() << std::endl;
            }
        }

        open.pop();
        closed.insert(curr.loc.hash());

        for (Dir dir : utils::directions_cartesian) {
            Pt neighbour = curr.loc + utils::dir_offset(dir);
            if (map.obstacles[neighbour.flat(map.size)] ||
                closed.contains(neighbour.hash())) {
                continue;
            }
            if (neighbour.hash() == skipped.hash()) continue;

            int turns = get_turns(dir, curr.dir);
            int score = curr.score + 1 + turns * 1000;
            Node *child = arena.make({
                neighbour,
                dir,
                score,
                0,
                curr.steps + 1,
                curr.turns + turns,
                curr_ptr,
            });

            auto prev_score = scores.find(neighbour.hash());
            if (prev_score == scores.end() || score < prev_score->second) {
                open.push(child);
                scores[neighbour.hash()] = score;
            }
        }

        if (print) {
            std::cout << open.size() << std::endl;
            utils::wait_for_input();
        }
    }

    Path path = {};
    if (goal == nullptr) return path;

    Node *curr = goal;
    path.locs.push_back(curr->loc);
    path.score = curr->score;
    while (curr->parent != nullptr) {
        curr = curr->parent;
        path.locs.push_back(curr->loc);
    }

    return path;
}

int run(std::vector<std::string> const &lines, bool is_part1) {
    int size = lines.size();
    MapInfo map = {
        size, std::vector<bool>(size * size, false), {0, 0}, {0, 0}, Dir::EAST,
    };

    for (int row = 0; row < size; row++) {
        for (int x = 0; x < size; x++) {
            char ch = lines[row][x];
            int y = size - row - 1;
            Pt pos = {x, y};
            bool is_obstacle = false;

            if (ch == '#') {
                map.obstacles[pos.flat(size)] = true;
            } else if (ch == 'S') {
                map.start_loc = pos;
            } else if (ch == 'E') {
                map.goal_loc = pos;
            }
        }
    }

    Path best_path = pathfind(map, {-1, -1});
    if (is_part1) return best_path.score;

    auto best_locs = std::unordered_set<int>();
    auto skips = std::queue<Pt>();
    auto tried_skips = std::unordered_set<int>();
    for (auto skip_candidate : best_path.locs) {
        skips.push(skip_candidate);
    }

    while (!skips.empty()) {
        Pt skip = skips.front();
        skips.pop();

        if (tried_skips.contains(skip.hash())) continue;
        tried_skips.insert(skip.hash());

        Path path = pathfind(map, skip);
        bool is_best = path.score != -1 && path.score <= best_path.score;

        for (Pt loc : path.locs) {
            if (!tried_skips.contains(loc.hash())) skips.push(loc);
            if (is_best) best_locs.insert(loc.hash());
        }
    }

    return best_locs.size();
}

} // namespace day_16
