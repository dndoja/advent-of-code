#include "utils.h"
#include <iostream>
#include <queue>

namespace day_21 {
using utils::Dir;
using utils::Pt;

// clang-format off
constexpr char numpad[12] = {
    '7', '8', '9', 
    '4', '5', '6',
    '1', '2', '3',
    ' ', '0', 'A',
};

constexpr char keypad[6] = {
    ' ', '^', 'A', 
    '<', 'v', '>',
};

enum KeypadButton {
    L='<',
    R='>',
    T='^',
    B='v',
    A='A',
};
// clang-format on

constexpr std::vector<std::vector<KeypadButton>> keypad_path(KeypadButton from,
                                                             KeypadButton to) {
    if (from == to) return {{}};

    if (from == '^') {
        switch (to) {
        case 'A':
            return {{R}};
        case 'v':
            return {{B}};
        case '<':
            return {{B, L}};
        case '>':
            return {{B, R}, {R, B}};
        case '^':
            return {{}};
        };
    }

    if (from == 'v') {
        switch (to) {
        case '^':
            return {{T}};
        case '<':
            return {{L}};
        case '>':
            return {{R}};
        case 'A':
            return {{T, R}, {R, T}};
        case 'v':
            return {{}};
        };
    }

    if (from == '<') {
        switch (to) {
        case 'v':
            return {{R}};
        case '^':
            return {{R, T}};
        case '>':
            return {{R, R}};
        case 'A':
            return {{R, R, T}};
        case '<':
            return {{}};
        };
    }

    if (from == '>') {
        switch (to) {
        case 'v':
            return {{L}};
        case 'A':
            return {{T}};
        case '^':
            return {{L, T}, {T, L}};
        case '<':
            return {{L, L}};
        case '>':
            return {{}};
        };
    }

    if (from == 'A') {
        switch (to) {
        case '^':
            return {{L}};
        case '>':
            return {{B}};
        case 'v':
            return {{B, L}, {L, B}};
        case '<':
            return {{B, L, L}};
        case 'A':
            return {{}};
        };
    }

    return {{}};
}

constexpr Pt keypad_loc(KeypadButton button) {
    switch (button) {
    case L:
        return {0, 1};
    case R:
        return {2, 1};
    case T:
        return {1, 0};
    case B:
        return {1, 1};
    case A:
        return {2, 0};
    };
};

constexpr Pt numpad_loc(char ch) {
    switch (ch) {
    case 'A':
        return {2, 3};
    case '0':
        return {1, 3};
    case '1':
        return {0, 2};
    case '2':
        return {1, 2};
    case '3':
        return {2, 2};
    case '4':
        return {0, 1};
    case '5':
        return {1, 1};
    case '6':
        return {2, 1};
    case '7':
        return {0, 0};
    case '8':
        return {1, 0};
    case '9':
        return {2, 0};
    default:
        throw std::invalid_argument("Invalid Char");
    }
}

constexpr KeypadButton dir_button(Dir dir) {
    switch (dir) {
    case utils::NORTH:
        return KeypadButton::B;
    case utils::EAST:
        return KeypadButton::R;
    case utils::SOUTH:
        return KeypadButton::T;
    case utils::WEST:
        return KeypadButton::L;
    default:
        throw std::invalid_argument("Invalid direction passed");
    }
}

void get_paths_(std::vector<KeypadButton> const &buttons, int index,
                KeypadButton prev, std::vector<KeypadButton> const &curr_path,
                std::vector<std::vector<KeypadButton>> &result) {
    if (index == buttons.size()) {
        result.push_back(curr_path);
        return;
    }

    KeypadButton curr = buttons[index];
    for (auto path : keypad_path(prev, curr)) {
        std::vector<KeypadButton> next_path = {};
        next_path.insert(next_path.end(), curr_path.begin(), curr_path.end());
        next_path.insert(next_path.end(), path.begin(), path.end());
        next_path.push_back(A);
        get_paths_(buttons, index + 1, curr, next_path, result);
    }
}

std::vector<std::vector<KeypadButton>>
get_paths(std::vector<KeypadButton> const &buttons) {
    std::vector<std::vector<KeypadButton>> result = {};
    std::vector<KeypadButton> curr_path = {};

    get_paths_(buttons, 0, A, curr_path, result);

    return result;
}

uint64_t get_op_count(std::vector<KeypadButton> buttons, int level,
                      std::unordered_map<uint64_t, uint64_t> &cache) {
    if (level == 0) return buttons.size();

    uint64_t hash = 0;
    for (uint i = 0; i < buttons.size(); i++) {
        hash |= keypad_loc(buttons[i]).flat(3) << (i * 3);
    }
    hash = hash << 5 | level;

    uint64_t cached = cache[hash];
    if (cached != 0) return cached;

    uint64_t sum = 0;
    std::vector<KeypadButton> subsequence = {};

    for (auto button : buttons) {
        subsequence.push_back(button);

        if (button != A) continue;

        auto possible_paths = get_paths(subsequence);
        uint64_t min = LONG_MAX;
        for (auto path : possible_paths) {
            uint64_t min_presses = get_op_count(path, level - 1, cache);
            if (min_presses < min) min = min_presses;
        }

        sum += min;
        subsequence.clear();
    }

    return cache[hash] = sum;
}

std::vector<std::vector<KeypadButton>> get_numpad_paths(char from, char to) {
    struct Node {
        Pt loc;
        Dir dir = Dir::SOUTH_EAST;
        Node *parent = nullptr;
    };

    Pt goal_loc = numpad_loc(to);
    auto arena = utils::Arena<Node, 1000>();
    auto queue = std::queue<Node *>();
    auto roots = std::vector<Node *>();
    queue.push(arena.make({numpad_loc(from)}));

    while (!queue.empty()) {
        Node *curr = queue.front();
        queue.pop();

        int dist_to_goal = utils::manhattan(curr->loc, goal_loc);
        if (dist_to_goal == 0) {
            roots.push_back(curr);
            continue;
        }

        for (Dir dir : utils::directions_cartesian) {
            Pt next = curr->loc + utils::dir_offset(dir);

            if (next.off_grid(3, 4)) continue;
            if (next.x == 0 && next.y == 3) continue;

            if (utils::manhattan(next, goal_loc) < dist_to_goal) {
                queue.push(arena.make({next, dir, curr}));
            }
        }
    }

    auto paths = std::vector<std::vector<KeypadButton>>();
    for (auto root : roots) {
        Node *curr = root;
        paths.push_back({});

        while (curr->parent != nullptr) {
            paths.back().push_back(dir_button(curr->dir));
            curr = curr->parent;
        }

        std::reverse(paths.back().begin(), paths.back().end());
        paths.back().push_back(A);
    }

    return paths;
}

uint64_t run(std::vector<std::string> &lines, bool is_part1) {
    int depth = is_part1 ? 2 : 25;
    uint64_t total = 0;

    for (int i = 0; i < lines.size(); i++) {
        auto cache = std::unordered_map<uint64_t, uint64_t>();
        auto line = lines[i];
        char prev = 'A';
        uint64_t line_ops = 0;

        for (char next : line) {
            auto paths = get_numpad_paths(prev, next);
            uint64_t min_ops = LONG_MAX;
            for (auto path : paths) {
                uint64_t taps = get_op_count(path, depth, cache);
                if (taps < min_ops) min_ops = taps;
            }

            prev = next;
            line_ops += min_ops;
        }

        total += line_ops * std::stoi(line.substr(0, 3));
    }

    return total;
}
} // namespace day_21
