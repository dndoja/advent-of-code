#ifndef AOC_UTILS_H_
#define AOC_UTILS_H_

#include <iostream>
#include <string>
#include <vector>

namespace utils {
struct Pt {
    int x, y;

    int hash() const;
    int flat(int grid_size) const;
    bool off_grid(int width, int height = -1) const;
    Pt operator+(const Pt &other) const;
    Pt operator*(int mul) const;
    std::string to_string() const;

    static Pt from_index(int i, int grid_size);
};

template <typename T, size_t size> struct Arena {
    T items[size];
    int tail = -1;

    T *make(T item) {
        if (tail < size - 1) {
            ++tail;
        } else {
            tail = 0;
        }

        items[tail] = item;
        return &(items[tail]);
    }
};

template <typename T> struct DynamicArena {
    DynamicArena<T>(int size) : items(std::vector<T>(size)) {}

    std::vector<T> items;
    int tail = -1;

    T *make(T item) {
        if (tail < items.size() - 1) {
            ++tail;
        } else {
            tail = 0;
        }

        items[tail] = item;
        return &(items[tail]);
    }
};

enum Dir {
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST,
    NORTH_WEST,
};

constexpr Dir directions[8] = {
    NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST,
};

const std::string directions_names[8] = {
    "N", "NE", "E", "SE", "S", "SW", "W", "NW",
};

constexpr Dir directions_cartesian[4] = {NORTH, EAST, SOUTH, WEST};
constexpr Dir directions_diagonal[4] = {
    NORTH_EAST,
    SOUTH_EAST,
    SOUTH_WEST,
    NORTH_WEST,
};

Pt dir_offset(Dir dir);
int manhattan(Pt const &a, Pt const &b);
Dir rot45(Dir dir, bool clockwise);
Dir rot90(Dir dir, bool clockwise = true);

void print_ints(std::vector<int> const *vec);
std::vector<std::string> split(std::string str, std::string delimiter);
void wait_for_input();
void sleep30();
} // namespace utils
// namespace utils
#endif
