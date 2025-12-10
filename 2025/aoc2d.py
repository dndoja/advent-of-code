class Vec2:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return f"({self.x}, {self.y})"

    def __repr__(self):
        return str(self)


class Vec3:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __str__(self):
        return f"({self.x}, {self.y}, {self.z})"

    def __repr__(self):
        return str(self)


class Segment:
    def __init__(self, a, b):
        self.a = a
        self.b = b
        self.min_x = min(a.x, b.x)
        self.max_x = max(a.x, b.x)
        self.min_y = min(a.y, b.y)
        self.max_y = max(a.y, b.y)

        self.isVertical = False

        if a.x == b.x:
            self.isVertical = True
        else:
            self.slope = (b.y - a.y) / (b.x - a.x)
            self.yIntercept = a.y - self.slope * a.x

    def y(self, x):
        return self.slope * x + self.yIntercept

    def __str__(self):
        return f"[{self.a}; {self.b}]"

    def __repr__(self):
        return str(self)

    def contains(self, pt):
        min_x = min(self.a.x, self.b.x)
        max_x = max(self.a.x, self.b.x)
        min_y = min(self.a.y, self.b.y)
        max_y = max(self.a.y, self.b.y)
        c = min_x <= pt.x <= max_x and min_y <= pt.y <= max_y

        return c


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
