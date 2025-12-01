#include "utils.h"
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_set>

namespace day_19 {
std::vector<int> pathfind(std::string pattern, std::array<bool, 64 * 8> &edges,
                          int start = 0) {
    struct Node {
        int x;
        int h;
        Node *parent;

        int f() const { return x + h; }
    };

    auto arena = utils::Arena<Node, 64 * 8>();
    auto comp = [](const Node *a, const Node *b) { return a->f() > b->f(); };
    auto open =
        std::priority_queue<Node *, std::vector<Node *>, decltype(comp)>(comp);
    Node *goal = nullptr;

    open.push(arena.make({start, (int)pattern.size() - start, nullptr}));

    while (!open.empty()) {
        Node *curr = open.top();
        open.pop();

        if (curr->x == pattern.size()) {
            goal = curr;
            break;
        }

        for (int size = 1; size <= 8; size++) {
            int x = curr->x + size;
            if (!edges[curr->x * 8 + size - 1]) continue;

            int h = pattern.size() - x;
            Node *node = arena.make({x, h, curr});

            open.push(node);
        }
    }

    auto path = std::vector<int>();
    if (goal == nullptr) return path;

    Node *curr = goal;
    path.push_back(curr->x);
    while (curr->parent != nullptr) {
        curr = curr->parent;
        path.push_back(curr->x);
    }

    return path;
}

uint64_t count_paths(std::unordered_map<int, uint64_t> &memos,
                     std::array<bool, 64 * 8> &edges, int curr, int end) {
    if (curr >= end) return 1;

    if (memos.contains(curr)) return memos.at(curr);

    uint64_t sum = 0;
    for (int width = 1; width <= 8; width++) {
        if (!edges[curr * 8 + width - 1]) continue;
        sum += count_paths(memos, edges, curr + width, end);
    }

    memos[curr] = sum;

    return sum;
}

uint64_t run(std::vector<std::string> &lines, bool is_part1) {
    auto patterns_available = utils::split(lines[0], ", ");
    std::array<std::unordered_set<std::string>, 8> patterns_by_size = {};
    for (auto segment : patterns_available) {
        patterns_by_size[segment.size() - 1].insert(segment);
    }

    uint64_t matching = 0;
    for (int i = 2; i < lines.size(); i++) {
        std::string pattern = lines[i];
        auto edges = std::array<bool, 64 * 8>();

        for (int x = 0; x < pattern.size(); x++) {
            int max_size = 8;
            if (x + max_size > pattern.size()) max_size = pattern.size() - x;

            for (int size = 1; size <= max_size; size++) {
                std::string substr = pattern.substr(x, size);
                if (patterns_by_size[size - 1].contains(substr)) {
                    edges[x * 8 + size - 1] = true;
                }
            }
        }

        std::vector<int> path = pathfind(pattern, edges);
        if (is_part1) {
            if (!path.empty()) matching++;
        } else {
            auto memos = std::unordered_map<int, uint64_t>();
            uint64_t paths = count_paths(memos, edges, 0, pattern.size());
            matching += paths;
        }
    }

    return matching;
}
} // namespace day_19
