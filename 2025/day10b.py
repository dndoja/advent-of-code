from itertools import product
import re


class RationalTerm:
    def __init__(self, numerator, unit=1, denominator=1):
        self.unit = unit
        if numerator % denominator != 0:
            self.numerator = numerator
            self.denominator = denominator
        else:
            self.numerator = numerator // denominator
            self.denominator = 1

    def __str__(self):
        prefix = ""
        if self.numerator / self.denominator == -1:
            prefix = "-"
        elif self.numerator / self.denominator != 1:
            if self.denominator == 1:
                prefix = str(self.numerator)
            else:
                prefix = f"{self.numerator}/{self.denominator}"

        if prefix and prefix != "-" and isinstance(self.unit, str):
            prefix = f"({prefix})"

        unit = ""
        if isinstance(self.unit, str):
            unit = self.unit
            if prefix and prefix[0] == "(":
                unit = f"({self.unit})"
            elif prefix and prefix != "-":
                unit = f"*{self.unit}"

        return f"{prefix}{unit}"

    def __repr__(self):
        return self.__str__()

    def __mul__(self, numerator):
        return RationalTerm(
            self.numerator * numerator,
            denominator=self.denominator,
            unit=self.unit,
        )

    def __add__(self, other):
        return RationalTerm(
            self.numerator * other.denominator + other.numerator * self.denominator,
            denominator=self.denominator * other.denominator,
            unit=self.unit,
        )

    def __truediv__(self, value):
        return RationalTerm(
            self.numerator,
            denominator=self.denominator * value,
            unit=self.unit,
        )


def printm(matrix, pivot_row=-1, pivot_col=-1):
    for row in range(len(matrix)):
        line = ""
        for col in range(len(matrix[0])):
            if row == pivot_row and col == pivot_col:
                line += "#{0:#3d} ".format(matrix[row][col])
            elif col == len(matrix[0]) - 1:
                line += str(matrix[row][col])
            else:
                line += "{0:#4d} ".format(matrix[row][col])
        print("\n" + line)
    print("\n")


def matrix_echelon_form(matrix):
    pivot_row = 0
    pivots = {}
    pivots_by_col = {}

    def eliminate_row(pivot_row, target_row, col):
        # Done like this to eliminate floating operations
        p = matrix[pivot_row][col]
        a = matrix[target_row][col]

        for col in range(col, len(matrix[0])):
            matrix[target_row][col] = (
                p * matrix[target_row][col] - a * matrix[pivot_row][col]
            )

    for col in range(len(matrix[0])):
        if not matrix[pivot_row][col]:
            for row in range(pivot_row + 1, len(matrix)):
                if matrix[row][col]:
                    matrix[pivot_row], matrix[row] = matrix[row], matrix[pivot_row]
                    break

        if matrix[pivot_row][col] < 0:
            matrix[pivot_row] = [
                matrix[pivot_row][i] * -1 for i in range(len(matrix[0]))
            ]

        for row in range(pivot_row + 1, len(matrix)):
            if matrix[row][col]:
                eliminate_row(matrix, pivot_row, row, col)

        if matrix[pivot_row][col]:
            if pivot_row not in pivots:
                pivots[pivot_row] = col
                pivots_by_col[col] = pivot_row
            if pivot_row < len(matrix) - 1:
                pivot_row += 1
            else:
                break

    return matrix


def solve(matrix, button_max_presses):
    printm(matrix)

    matrix = matrix_echelon_form(matrix)
    table = {1: [RationalTerm(1)]}
    pivot_cols = []

    for row in range(len(matrix)):
        for col in range(len(matrix[0])):
            if matrix[row][col]:
                pivot_cols.append(col)
                break

    params = [f"x{col}" for col in range(len(matrix[0]) - 1) if col not in pivot_cols]
    eqs = []

    printm(matrix)
    print("Free variables:", params)

    for row in reversed(matrix):
        lhs = None
        rhs = [RationalTerm(row[-1], 1)]
        for i in range(len(row) - 1):
            if row[i] == 0:
                continue
            elif not lhs:
                lhs = RationalTerm(row[i], unit=f"x{i}")
            else:
                symbol = f"x{i}"
                if symbol in table:
                    for term in table[symbol]:
                        rhs.append(term * -row[i])
                else:
                    rhs.append(RationalTerm(-row[i], unit=symbol))

        if not lhs:
            continue

        for i in range(len(rhs)):
            if rhs[i]:
                for k in range(i + 1, len(rhs)):
                    if rhs[k] and rhs[i].unit == rhs[k].unit:
                        rhs[i] = rhs[i] + rhs[k]
                        rhs[k] = None

        rhs = [t for t in rhs if t]
        rhs = [t / lhs.numerator for t in rhs]
        lhs = lhs / lhs.numerator
        table[lhs.unit] = rhs
        eqs.append((lhs, rhs))
        print(lhs, "=", rhs)

    def evaluate():
        total = 0
        valid = True
        for lhs, rhs in eqs:
            rhval = 0
            rhval = RationalTerm(0)
            for term in rhs:
                rhval += term * table[term.unit][0].numerator

            rhval *= lhs.denominator
            rhval /= lhs.numerator
            if (
                rhval.numerator % rhval.denominator != 0
                or rhval.numerator * rhval.denominator < 0
            ):
                valid = False

            total += rhval.numerator // rhval.denominator

        return (total, valid)

    # My brain cannot handle anymore math so I'm just brute forcing this part
    permutations = [range(0, button_max_presses[int(p[1:])] + 1) for p in params]
    solution = float("inf")

    for combo in product(*permutations):
        permutation = dict(zip(params, combo))

        params_sum = 0
        for unit in permutation:
            table[unit] = [RationalTerm(permutation[unit])]
            params_sum += permutation[unit]

        sum, valid = evaluate()
        sum += params_sum
        if valid and sum < solution:
            solution = sum

    return solution


with open("day10.txt") as file:
    result = 0
    curr = 0
    for line in file:
        joltage_reqs = [
            int(j) for j in line[line.find("{") + 1 : len(line) - 2].split(",")
        ]

        buttons = [
            [int(ch) for ch in button.split(",")]
            for button in re.findall(r"\(([^)]*)\)", line)
        ]
        matrix = [[0] * len(buttons) + [req] for req in joltage_reqs]

        for i_btn in range(len(buttons)):
            for i_slot in buttons[i_btn]:
                matrix[i_slot][i_btn] = int(1)

        button_max_presses = []
        for button in buttons:
            min = float("inf")
            for i in button:
                if joltage_reqs[i] < min:
                    min = joltage_reqs[i]
            button_max_presses.append(min)
        print("\nMachine", curr)
        print(button_max_presses)
        curr += 1
        result += solve(matrix, button_max_presses)

    print(result)
