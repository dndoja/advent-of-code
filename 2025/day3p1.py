result = 0
batteries = []
with open("day3.txt") as file:
    for line in file:
        batteries.append([int(char) for char in line.strip()])

for battery in batteries:
    left = 0
    for i in range(left + 1, len(battery) - 1):
        if battery[i] > battery[left]:
            left = i

    largest1 = battery[left]
    left += 1
    largest2 = battery[left]

    while left < len(battery):
        if battery[left] > largest2:
            largest2 = battery[left]
        left += 1

    result += largest1 * 10 + largest2

print(result)
