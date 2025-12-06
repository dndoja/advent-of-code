rots = []

with open("day01.txt") as file:
    for line in file:
        rots.append((1 if line[0:1] == "R" else -1, int(line[1:])))

curr = 50
result = 0

for rot_coeff, rot_amount in rots:
    curr = (curr + rot_coeff * rot_amount) % 100
    if curr == 0:
        result += 1

print(result)
