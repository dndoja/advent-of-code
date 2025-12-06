rots = []

with open("day01.txt") as file:
    for line in file:
        rots.append((1 if line[0:1] == "R" else -1, int(line[1:])))

curr = 50
result = 0

for rot_coeff, rot_amount in rots:
    next = curr + rot_coeff * (rot_amount % 100)
    result += abs(int(rot_amount / 100)) + (1 if curr != 0 and next <= 0 or next >= 100 else 0)
    curr = next % 100

print(result)
