import math

result = 0
batteries = []
with open("day03.txt") as file:
    for line in file:
        batteries.append([int(char) for char in line.strip()])


for battery in batteries:
    left = -1

    for iter in range(0, 12):
        left += 1
        for i in range(left, len(battery) - (12 - iter) + 1):
            if battery[i] > battery[left]:
                left = i

        result += battery[left] * int(math.pow(10, 12 - iter - 1))

print(result)
