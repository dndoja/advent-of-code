result = 0
ranges = []
ingredients = []

with open("day05.txt") as file:
    for line in file:
        split = line.split("-")
        if len(split) == 2:
            ranges.append((int(split[0]), int(split[1])))
        elif len(line.strip()) > 0:
            ingredients.append(int(line))

ranges = sorted(ranges, key = lambda range: range[0])

for ingredient in ingredients:
    for range in ranges:
        if range[0] <= ingredient <= range[1]:
            result += 1
            break

print(result)
