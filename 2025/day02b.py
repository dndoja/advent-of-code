import math

seqs = []
result = 0

with open("day02.txt") as file:
    for pair in file.read().split(","):
        split = pair.split("-")
        seqs.append((int(split[0]), int(split[1])))


def is_repeating(num, digits, parts):
    if digits % parts != 0:
        return False

    mul = int(math.pow(10, digits / parts))
    prev = None

    for i in range(0, parts):
        part = int(num // math.pow(mul, i) % mul)
        if prev is not None and part != prev:
            return False
        else:
            prev = part

    return prev != 0


for start, finish in seqs:
    for num in range(start, finish + 1):
        digits = math.ceil(math.log10(num))
        if digits > 1:
            for i in range(2, digits + 1):
                if is_repeating(num, digits, i):
                    result += num
                    break

print(result)
