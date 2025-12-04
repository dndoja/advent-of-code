import math

result = 0
seqs = []
with open("day2.txt") as file:
    for pair in file.read().split(","):
        split = pair.split("-")
        seqs.append((int(split[0]), int(split[1])))


for start, finish in seqs:
    for num in range(start, finish + 1):
        digits = math.ceil(math.log10(num))
        if digits > 0 and digits % 2 == 0:
            mul = int(math.pow(10, digits / 2))
            right = int(num % mul) * mul
            left = num - int(num % mul)
            if right == left:
                result += num

print(result)
