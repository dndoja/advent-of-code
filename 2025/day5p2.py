result = 0
ranges = []

with open("day5.txt") as file:
    for line in file:
        split = line.split("-")
        if len(split) == 2:
            ranges.append((int(split[0]), int(split[1])))

ranges = sorted(ranges, key=lambda range: range[0])

for i in range(len(ranges)):
    if i < len(ranges) - 1:
        if ranges[i][1] >= ranges[i + 1][0]:
            if ranges[i][1] >= ranges[i + 1][1]:
                ranges[i + 1] = (ranges[i + 1][0], ranges[i][1])
            ranges[i] = (ranges[i][0], ranges[i + 1][0] - 1)

    if ranges[i][1] >= ranges[i][0]:
        result += ranges[i][1] - ranges[i][0] + 1

print(result)
