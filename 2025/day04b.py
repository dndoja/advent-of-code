import aoc2d as xy

result = 0
grid = None

with open("day04.txt") as file:
    height = 0
    flatdata = []
    for line in file:
        height += 1
        for char in line.strip():
            flatdata.append(char == "@")
    grid = xy.Grid(flatdata, height)


loop = True
while loop:
    loop = False
    for i in range(len(grid)):
        if grid[i]:
            x, y = xy.unflatten(i)
            neighbours = 0
            for nx, ny in xy.neighbours(x, y):
                if grid[xy.flatten(nx, ny)]:
                    neighbours += 1

            if neighbours < 4:
                grid[i] = False
                loop = True
                result += 1

print(result)
