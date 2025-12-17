shape_variants = []
result = 0

# Well this turned out to be fucking stupid
with open("day12.txt") as file:
    for line in file:
        line = line.strip()
        if not line:
            continue

        if len(line) == 2:
            shape_variants.append([[]])
        elif len(line) == 3:
            shape_variants[-1][0].append([line[x] == "#" for x in range(3)])
        else:
            split = line.split(" ")
            w, h = map(int, split[0][0:-1].split("x"))
            shape_counts = list(map(int, split[1:]))
            if sum(shape_counts) * 9 <= w * h:
                result += 1

print(result)
