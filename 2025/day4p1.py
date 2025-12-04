import aoc2d as xy

result = 0
grid = []

with open("day4.txt") as file:
    height = 0
    width = 0
    for line in file:
        height += 1
        width = len(line.strip())
        for char in line.strip():
            grid.append(char == "@")
    xy.init(width, height)


for i in range(len(grid)):
    if grid[i]:
        x, y = xy.unflatten(i)
        neighbours = 0
        for nx, ny in xy.neighbours(x, y):
            if grid[xy.flatten(nx, ny)]:
                neighbours += 1

        if neighbours < 4:
            result += 1

print(result)
