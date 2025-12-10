from aoc2d import Vec2

red_tiles = []


with open("day09.txt") as file:
    for line in file:
        split = line.strip().split(",")
        red_tiles.append(Vec2(int(split[0]), int(split[1])))


max_area = 0
for s1 in red_tiles:
    for s2 in red_tiles:
        area = (abs(s1.x - s2.x) + 1) * (abs(s1.y - s2.y) + 1)
        if area > max_area:
            max_area = area

print(max_area)
