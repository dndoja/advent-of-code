#include "utils.h"
#include <iostream>
#include <thread>

namespace utils {

void wait_for_input() { std::cin.get(); }

void sleep30() { std::this_thread::sleep_for(std::chrono::milliseconds(30)); }

int manhattan(Pt const &a, Pt const &b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

int Pt::hash() const {
    char sign_x = x < 0 ? 1 : 0;
    char sign_y = y < 0 ? 1 : 0;
    return abs(x) | abs(y) << 12 | sign_x << 29 | sign_y << 30;
}

int Pt::flat(int width) const { return y * width + x; }

bool Pt::off_grid(int width, int height) const {
    if (height == -1) height = width;
    return x < 0 || x >= width || y < 0 || y >= height;
}

Pt Pt::from_index(int i, int grid_size) {
    return {i % grid_size, i / grid_size};
}

Pt Pt::operator+(const Pt &other) const { return {x + other.x, y + other.y}; }
Pt Pt::operator*(int mul) const { return {x * mul, y * mul}; }
std::string Pt::to_string() const {
    return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
};

Pt dir_offset(Dir dir) {
    constexpr Pt direction_offsets[8] = {
        {0, 1},   // North
        {1, 1},   // North-East
        {1, 0},   // East
        {1, -1},  // South-East
        {0, -1},  // South
        {-1, -1}, // South-West
        {-1, 0},  // West
        {-1, 1},  // North-West
    };

    return direction_offsets[dir];
}

Dir rot45(Dir dir, bool clockwise) {
    int next = dir + (clockwise ? 1 : -1);
    if (next < 0) return directions[7];
    if (next > 7) return directions[0];
    return directions[next];
}

Dir rot90(Dir dir, bool clockwise) {
    int mul = clockwise ? 1 : -1;
    int i_rotated = (dir + 2 * mul) % 8;
    return directions[i_rotated];
}

void print_ints(std::vector<int> const *vec) {
    for (int i : *vec) {
        std::cout << i << " ";
    }
    std::cout << "\n";
}

std::vector<std::string> split(std::string str, std::string delimiter) {
    std::vector<std::string> split = std::vector<std::string>();

    if (delimiter.length() == 0) {
        split.push_back(str);
        return split;
    }

    std::string buffer;
    int i = 0;
    while (i < str.length()) {
        char ch = str[i];

        if (ch != delimiter[0]) {
            buffer += ch;
            i++;
            continue;
        }

        std::string lookahead_buf;
        int offset = 0;
        while (offset < delimiter.length()) {
            char ch = str[i + offset];
            if (ch == delimiter[offset]) {
                lookahead_buf += ch;
                offset++;
            } else {
                offset++;
                break;
            }
        }

        if (lookahead_buf.length() == delimiter.length()) {
            if (buffer.length() > 0) {
                split.push_back(buffer);
                buffer = "";
            }
        } else {
            buffer += lookahead_buf;
            buffer += str[i + offset - 1];
        }

        i += offset;
    }

    if (buffer.length() > 0) {
        split.push_back(buffer);
    }

    return split;
}
} // namespace utils
