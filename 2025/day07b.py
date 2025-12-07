import aoc2d as xy

start = (0, 0)
grid = None

with open("day07.txt") as file:
    height = 0
    flatdata = []

    for line in file:
        height += 1
        for char in line.strip():
            if char == "S":
                i_start = len(flatdata)
            flatdata.append(char)

    grid = xy.Grid(flatdata, height)
    start = xy.unflatten(i_start)


cache = [None] * len(grid)


def beam(x, y):
    if y >= grid.height:
        return 1

    i = xy.flatten(x, y)
    if cache[i]:
        return cache[i]

    timelines = 0
    if grid[i] == "^":
        if x > 0:
            timelines += beam(x - 1, y + 1)
        if x < grid.width - 1:
            timelines += beam(x + 1, y + 1)
    else:
        timelines += beam(x, y + 1)

    cache[i] = timelines

    return timelines


print(beam(start[0], start[1]))
