from collections import deque
from typing import NamedTuple
import re


class MachineConfig(NamedTuple):
    lights_mask: int
    button_masks: []


configs: [MachineConfig] = []


with open("day10.txt") as file:
    for line in file:
        lights_mask = 0
        for i in range(1, len(line)):
            if line[i] == "]":
                break

            if line[i] == "#":
                lights_mask |= 1 << (i - 1)

        button_masks = []
        for button in re.findall(r"\(([^)]*)\)", line):
            mask = 0
            chars = button.split(",")
            for ch in chars:
                mask |= 1 << int(ch)
            button_masks.append(mask)

        configs.append(MachineConfig(lights_mask, button_masks))


def solve(machine):
    queue = deque([(0, 0)])
    visited = set()

    while len(queue):
        (curr_state, curr_level) = queue.popleft()

        if curr_state == machine.lights_mask:
            return curr_level

        for i in range(len(machine.button_masks)):
            next_state = curr_state ^ machine.button_masks[i]
            if next_state not in visited or next_state == machine.lights_mask:
                queue.append((next_state, curr_level + 1))


result = 0

for i in range(len(configs)):
    print(f"Machine {i}")
    for k in range(len(configs[i].button_masks)):
        print(k, "{0:b}".format(configs[i].button_masks[k]))
    print("")
    result += solve(configs[i])
    print("\n")

print(result)
