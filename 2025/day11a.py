paths = {}

with open("day11.txt") as file:
    for line in file:
        src = line[:3]
        dst = line[5:].strip().split(" ")
        paths[src] = dst


stack = ["you"]
result = 0

while len(stack):
    curr = stack.pop()

    if curr == "out":
        result += 1
        continue

    for next in paths[curr]:
        stack.append(next)


print(result)
