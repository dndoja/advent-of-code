#include <iostream>
#include <queue>
#include <set>
#include <string>

#include "utils.h"

namespace day_15 {
using utils::Dir;
using utils::Pt;

enum Object { none, wall, box_l, box_r };

struct MapInfo {
    int width;
    int height;
    Pt initial_robot_loc;
    std::vector<Object> &objects;

    Object object_at(Pt &loc) { return objects[loc.flat(width)]; }
    void clear(Pt loc) { objects[loc.flat(width)] = Object::none; }
    void set_obj(Pt loc, Object obj) { objects[loc.flat(width)] = obj; }

    void move(Pt from, Pt to) {
        objects[to.flat(width)] = objects[from.flat(width)];
        objects[from.flat(width)] = Object::none;
    }

    void print(Pt robot_loc, Dir dir = Dir::NORTH_WEST, bool is_part1 = false) {
        std::string box_l_str = is_part1 ? "O" : "[";
        system("clear");

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (robot_loc.x == x && robot_loc.y == y) {
                    switch (dir) {
                    case utils::NORTH:
                        std::cout << "v";
                        break;
                    case utils::EAST:
                        std::cout << ">";
                        break;
                    case utils::SOUTH:
                        std::cout << "^";
                        break;
                    case utils::WEST:
                        std::cout << "<";
                        break;
                    default:
                        std::cout << "@";
                        break;
                    }

                    continue;
                }

                Pt loc = {x, y};
                switch (object_at(loc)) {
                case none:
                    std::cout << ".";
                    break;
                case wall:
                    std::cout << "#";
                    break;
                case box_l:
                    std::cout << box_l_str;
                    break;
                case box_r:
                    std::cout << "]";
                    break;
                }
            }
            std::cout << std::endl;
        }
    }
};

bool move_big_box_(std::vector<std::tuple<int, bool>> &boxes_out, Pt box_loc,
                   Dir dir, MapInfo &map) {
    Object obj = map.object_at(box_loc);
    Pt offset = utils::dir_offset(dir);

    Pt left, right;
    if (obj == Object::box_l) {
        left = box_loc;
        right = {box_loc.x + 1, box_loc.y};
    } else {
        left = {box_loc.x - 1, box_loc.y};
        right = box_loc;
    }

    Pt left_fw = left + offset;
    Pt right_fw = right + offset;

    if (offset.x < 0) {
        boxes_out.push_back({right.flat(map.width), true});
        boxes_out.push_back({left.flat(map.width), false});
    } else {
        boxes_out.push_back({left.flat(map.width), false});
        boxes_out.push_back({right.flat(map.width), true});
    }

    bool valid_left = false;
    if (dir == Dir::EAST) {
        valid_left = true;
    } else {
        switch (map.object_at(left_fw)) {
        case none:
            valid_left = true;
            break;
        case wall:
            valid_left = false;
            break;
        case box_l:
        case box_r:
            valid_left = move_big_box_(boxes_out, left_fw, dir, map);
            break;
        }
    }

    bool valid_right = false;
    if (dir == Dir::WEST) {
        valid_right = true;
    } else {
        switch (map.object_at(right_fw)) {
        case none:
            valid_right = true;
            break;
        case wall:
            valid_right = false;
            break;
        case box_l:
        case box_r:
            valid_right = move_big_box_(boxes_out, right_fw, dir, map);
            break;
        }
    }

    return valid_left && valid_right;
}

void run_pt2_moves_(MapInfo &map, std::queue<Dir> &moves) {
    Pt robot_loc = map.initial_robot_loc;
    while (!moves.empty()) {
        Dir dir = moves.front();
        Pt dir_offset = utils::dir_offset(dir);
        moves.pop();

        Pt next = robot_loc + dir_offset;
        Object object = map.object_at(next);

        map.print(robot_loc, dir, false);
        utils::wait_for_input();

        if (object == Object::wall) continue;

        if (object == Object::none) {
            robot_loc = next;
            continue;
        }

        auto moved_boxes = std::vector<std::tuple<int, bool>>();
        bool can_move = move_big_box_(moved_boxes, next, dir, map);
        if (!can_move) continue;

        for (auto [box_index, _] : moved_boxes) {
            map.objects[box_index] = Object::none;
        }

        for (auto [box_index, right] : moved_boxes) {
            Pt loc = Pt::from_index(box_index, map.width) + dir_offset;
            map.set_obj(loc, right ? Object::box_r : Object::box_l);
        }

        robot_loc = next;
    }
}

void run_pt1_moves_(MapInfo &map, std::queue<Dir> &moves) {
    Pt robot_loc = map.initial_robot_loc;
    while (!moves.empty()) {
        Dir dir = moves.front();
        Pt offset = utils::dir_offset(dir);
        moves.pop();

        // map.print(robot_loc, dir, true);
        // utils::wait_for_input();

        bool moved = false;
        for (int step = 1;; step++) {
            Pt loc = robot_loc + Pt{offset.x * step, offset.y * step};
            Object object = map.object_at(loc);

            if (object == Object::box_l) continue;

            if (object == Object::none) {
                moved = true;
                for (int reverse = step; reverse > 1; reverse--) {
                    Pt src = robot_loc + Pt{offset.x * (reverse - 1),
                                            offset.y * (reverse - 1)};
                    Pt dest =
                        robot_loc + Pt{offset.x * reverse, offset.y * reverse};
                    map.move(src, dest);
                }
            }

            break;
        }

        if (moved) robot_loc = robot_loc + offset;
    }
}

int run(std::vector<std::string> const &lines, bool is_part1) {
    int height = lines[0].size();
    int width = height * (is_part1 ? 1 : 2);
    auto objects = std::vector(width * height, none);
    auto moves = std::queue<Dir>();
    MapInfo map = {width, height, {0, 0}, objects};

    for (int y = 0; y < lines.size(); y++) {
        std::string line = lines[y];

        if (line.starts_with('#')) {
            if (is_part1) {
                for (int x = 0; x < line.size(); x++) {
                    Pt loc = {x, y};
                    switch (line[x]) {
                    case '#':
                        objects[loc.flat(width)] = wall;
                        break;
                    case 'O':
                        objects[loc.flat(width)] = box_l;
                        break;
                    case '@':
                        map.initial_robot_loc = loc;
                    };
                }
            } else {
                for (int i = 0; i < line.size(); i++) {
                    int x = i * 2;
                    Pt loc1 = {x, y};
                    Pt loc2 = {x + 1, y};
                    switch (line[i]) {
                    case '#':
                        objects[loc1.flat(width)] = wall;
                        objects[loc2.flat(width)] = wall;
                        break;
                    case 'O':
                        objects[loc1.flat(width)] = box_l;
                        objects[loc2.flat(width)] = box_r;
                        break;
                    case '@':
                        map.initial_robot_loc = loc1;
                    };
                }
            }
        } else {
            for (char ch : line) {
                switch (ch) {
                case '>':
                    moves.push(Dir::EAST);
                    break;
                case '<':
                    moves.push(Dir::WEST);
                    break;
                case 'v':
                    moves.push(Dir::NORTH);
                    break;
                case '^':
                    moves.push(Dir::SOUTH);
                    break;
                }
            }
        }
    }

    if (is_part1) {
        run_pt1_moves_(map, moves);
    } else {
        run_pt2_moves_(map, moves);
    }

    int sum = 0;
    for (int i = 0; i < map.objects.size(); i++) {
        Object object = map.objects[i];
        if (object != Object::box_l) continue;
        Pt loc = Pt::from_index(i, map.width);
        sum += loc.y * 100 + loc.x;
    }

    return sum;
}
} // namespace day_15
