result = 0
operands = []
operators = []

with open("day06.txt") as file:
    for line in file:
        chars = list(line.strip().split(" "))
        col = 0
        for char in chars:
            if char.isdecimal():
                if col >= len(operands):
                    operands.append([])
                print(col, len(operands))
                operands[col].append(int(char))
                col += 1
            elif len(char) > 0:
                operators.append(char)

for i in range(len(operators)):
    def op(a, b):
        return a + b if operators[i] == "+" else a * b

    acc = operands[i][0]
    for k in range(1, len(operands[i])):
        acc = op(acc, operands[i][k])

    result += acc


print(result)
