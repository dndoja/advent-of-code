#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <unordered_set>

#include "utils.h"

namespace day_18 {
using utils::Pt;

template <int size> struct MapInfo {
    bool obstacles[size * size] = {false};

    void print(Pt curr_loc, Pt goal_loc) {
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                Pt pt = {x, y};

                if (pt.hash() == curr_loc.hash()) {
                    std::cout << "@";
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
    int score;
    int heuristic;
    Node *parent;

    int f() const { return score + heuristic; }

    std::string to_string() {
        return std::to_string(loc.x) + ":" + std::to_string(loc.y) + " (" +
               std::to_string(score) + ")";
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

template <int map_size>
Path pathfind(MapInfo<map_size> &map, Pt from, Pt to, bool print = false) {
    NodeArena arena = {};
    Node *goal = nullptr;
    auto closed = std::unordered_set<int>();
    auto scores = std::unordered_map<int, int>();
    auto comp = [](const Node *a, const Node *b) { return a->f() > b->f(); };
    auto open =
        std::priority_queue<Node *, std::vector<Node *>, decltype(comp)>(comp);
    open.push(arena.make({from, 0, 0, nullptr}));

    while (!open.empty()) {
        Node *curr_ptr = open.top();
        Node curr = *curr_ptr;

        if (curr.loc.hash() == to.hash()) {
            goal = curr_ptr;
            break;
        }

        if (print) {
            system("clear");
            map.print(curr.loc, to);
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

        for (utils::Dir dir : utils::directions_cartesian) {
            Pt neighbour = curr.loc + utils::dir_offset(dir);
            if (neighbour.off_grid(map_size) ||
                map.obstacles[neighbour.flat(map_size)] ||
                closed.contains(neighbour.hash())) {
                continue;
            }

            int score = curr.score + 1;
            Node *child = arena.make({neighbour, score, 0, curr_ptr});

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

std::string run(std::vector<std::string> const &lines, bool is_part1) {
    constexpr int map_size = 71;
    constexpr Pt start_loc = {0, 0};
    constexpr Pt goal_loc = {70, 70};
    MapInfo<map_size> map = {};

    for (int i = 0; i < lines.size(); i++) {
        auto split = utils::split(lines[i], ",");
        Pt pt = {std::stoi(split[0]), std::stoi(split[1])};

        if (is_part1 && i == 1024) {
            Path path = pathfind(map, start_loc, goal_loc);
            return std::to_string(path.score);
        }

        map.obstacles[pt.flat(map_size)] = true;

        if (i <= 1024) continue;

        Path path = pathfind(map, start_loc, goal_loc);
        if (path.score == -1) {
            return std::to_string(pt.x) + "," + std::to_string(pt.y);
        }
    }

    return "";
}

} // namespace day_18
