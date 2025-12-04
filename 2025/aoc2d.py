size = (0, 0)

adjecent_offsets = [
    (0, 1),
    (1, 1),
    (1, 0),
    (1, -1),
    (0, -1),
    (-1, -1),
    (-1, 0),
    (-1, 1),
]


def init(w, h):
    global size
    size = (w, h)


def flatten(x, y):
    return y * size[0] + x


def unflatten(i):
    return (i % size[0], i // size[0])


def neighbours(x, y):
    for dx, dy in adjecent_offsets:
        nx, ny = x + dx, y + dy
        if in_bounds(nx, ny):
            yield (nx, ny)


def in_bounds(x, y):
    return x >= 0 and x < size[0] and y >= 0 and y < size[1]
