from aoc2d import Vec3

result = 1
boxes = []


def dist(p1, p2):
    return (p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2 + (p1.z - p2.z) ** 2


with open("day08.txt") as file:
    for line in file:
        split = line.strip().split(",")
        boxes.append(Vec3(int(split[0]), int(split[1]), int(split[2])))

added_pairs = set()
pairs = []

for i in range(len(boxes)):
    for k in range(len(boxes)):
        if i == k:
            continue

        pair_id = min([i, k]) | max([i, k]) << 16
        if pair_id not in added_pairs:
            added_pairs.add(pair_id)
            pairs.append(
                (
                    boxes[i],
                    boxes[k],
                    dist(boxes[i], boxes[k]),
                )
            )

pairs = sorted(pairs, key=lambda v: v[2])
circuits = []

for i in range(1000):
    (wire_l, wire_r, _) = pairs[i]
    circuit_l = None
    circuit_r = None

    for k in range(len(circuits)):
        if wire_l in circuits[k]:
            circuit_l = k
        if wire_r in circuits[k]:
            circuit_r = k

        if circuit_l and circuit_r:
            break

    if not circuit_l and not circuit_r:
        circuits.append(set([wire_l, wire_r]))
    elif circuit_l and not circuit_r:
        circuits[circuit_l].add(wire_r)
    elif circuit_r and not circuit_l:
        circuits[circuit_r].add(wire_l)
    elif circuit_l != circuit_r:
        circuits[circuit_l] |= circuits[circuit_r]
        circuits[circuit_r] = set()


circuits = sorted(circuits, key=lambda c: len(c), reverse=True)
for i in range(3):
    result *= len(circuits[i])

print(result)
