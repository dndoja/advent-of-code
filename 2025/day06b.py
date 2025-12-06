result = 0
operands = []
operators = []
lines = []

with open("day06.txt") as file:
    for line in file:
        lines.append(list(line))

height = len(lines)
width = len(lines[0])
expr_index = 0
operand_index = 0

for x in range(width):
    found_number = False
    for y in range(height):
        ch = lines[y][x]
        if ch.isdecimal():
            if expr_index >= len(operands):
                operands.append([])
            if operand_index >= len(operands[expr_index]):
                operands[expr_index].append("")
            operands[expr_index][operand_index] += ch
            found_number = True
        elif ch == "*" or ch == "+":
            operators.append(ch)

    if found_number:
        operand_index += 1
    else:
        expr_index += 1
        operand_index = 0


for i in range(len(operators)):

    def op(a, b):
        return a + b if operators[i] == "+" else a * b

    acc = int(operands[i][0])
    for k in range(1, len(operands[i])):
        acc = op(acc, int(operands[i][k]))

    result += acc

print(result)
