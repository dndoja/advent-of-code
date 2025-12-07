import aoc2d as xy

result = 0
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

prev_beams = [False] * grid.width
prev_beams[start[0]] = True

for y in range(start[1] + 1, grid.height):
    beams = [False for _ in range(grid.width)]
    for x in range(0, grid.width):
        if prev_beams[x]:
            if grid[xy.flatten(x, y)] == "^":
                split = False
                if x > 0 and not beams[x - 1]:
                    beams[x - 1] = True
                    split = True
                if x < grid.width - 1 and not beams[x + 1]:
                    beams[x + 1] = True
                    split = True
                if split:
                    result += 1
            else:
                beams[x] = True
    prev_beams = beams


print(result)
