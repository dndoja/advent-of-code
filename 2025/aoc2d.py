class Vec2:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return f"({self.x}, {self.y})"

class Vec3:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __str__(self):
        return f"({self.x}, {self.y}, {self.z})"


class Grid:
    def __init__(self, flat_data, height, set_global=True):
        self.data = flat_data
        self.width = len(flat_data) // height if height > 0 else 0
        self.height = height
        if set_global:
            global __grid__
            __grid__ = self

    def __getitem__(self, index):
        return self.data[index]

    def __setitem__(self, index, val):
        self.data[index] = val

    def __len__(self):
        return len(self.data)


__grid__ = Grid([], 0)


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


def flatten(x, y):
    return y * __grid__.width + x


def unflatten(i):
    return (i % __grid__.width, i // __grid__.width)


def neighbours(x, y):
    for dx, dy in adjecent_offsets:
        nx, ny = x + dx, y + dy
        if in_bounds(nx, ny):
            yield (nx, ny)


def in_bounds(x, y):
    return x >= 0 and x < __grid__.width and y >= 0 and y < __grid__.height
