from functools import cache
from collections import deque

paths = {}

with open("day11.txt") as file:
    for line in file:
        src = line[:3]
        dst = line[5:].strip().split(" ")
        paths[src] = dst


@cache
def is_reachable(start, end):
    stack = deque([start])
    visited = set()

    while len(stack):
        curr = stack.popleft()

        if curr == end:
            return True

        if curr not in paths:
            return False

        for next in paths[curr]:
            if next not in visited:
                visited.add(next)
                stack.append(next)

    return False


@cache
def count_paths(start, end):
    if start == end:
        return 1

    count = 0
    for next in paths[start]:
        if is_reachable(start, end):
            count += count_paths(next, end)

    return count


print(count_paths("svr", "fft") * count_paths("fft", "dac") * count_paths("dac", "out"))
