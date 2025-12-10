from functools import cache
from aoc2d import Vec2, Segment

red_tiles = []
segments = []


miny, minx, maxy, maxx = float("inf"), float("inf"), 0, 0
MAX_DEPTH = 7  # Hand tuned


with open("day09.txt") as file:
    prev = None
    for line in file:
        split = line.strip().split(",")
        curr = Vec2(int(split[0]), int(split[1]))
        if curr.x > maxx:
            maxx = curr.x
        if curr.x < minx:
            minx = curr.x
        if curr.y < miny:
            miny = curr.y
        if curr.y > maxy:
            maxy = curr.y

        red_tiles.append(curr)
        if prev:
            segments.append(Segment(prev, curr))
        prev = curr

    segments.append(Segment(red_tiles[len(red_tiles) - 1], red_tiles[0]))


@cache
def in_polygon(x, y):
    collisions = 0
    pt = Vec2(x, y)
    for segment in segments:
        if segment.contains(pt):
            return True

        # Horizontal raytrace
        if segment.min_y <= pt.y < segment.max_y and pt.x < segment.min_x:
            collisions += 1

    return collisions % 2 == 1


@cache
def is_rect_in_polygon(l, t, r, b, d):
    if d == MAX_DEPTH:
        return True

    if (
        not in_polygon(l, t)
        or not in_polygon(l, b)
        or not in_polygon(r, t)
        or not in_polygon(r, b)
    ):
        return False

    w = r - l + 1
    h = b - t + 1
    cx = l + w // 2
    cy = t + h // 2
    quads = []

    if h > 2:
        quads.append((l, t, r, cy))
        quads.append((l, cy, r, b))

    if w > 2:
        quads.append((l, t, cx, b))
        quads.append((cx, t, r, b))

    for quad in quads:
        if not is_rect_in_polygon(*quad, d + 1):
            return False

    return True


max_area = 0
rect = None
for s1 in red_tiles:
    for s2 in red_tiles:
        if s1 == s2:
            continue

        b = max([s1.y, s2.y])
        t = min([s1.y, s2.y])
        r = max([s1.x, s2.x])
        l = min([s1.x, s2.x])
        area = (r - l + 1) * (b - t + 1)

        if area > max_area and is_rect_in_polygon(l, t, r, b, 0):
            rect = (l, t, r, b)
            max_area = area

l, t, r, b = rect
for y in range(miny - 1, maxy + 2, 1200):
    line = ""
    for x in range(minx - 1, maxx + 2, 1200):
        pt = Vec2(x, y)
        line += "=" if l <= x <= r and t <= y <= b else "#" if in_polygon(x, y) else "."
    print(line)

print(max_area)
