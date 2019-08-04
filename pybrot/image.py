"""
image.py
by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
"""

# --- Imports --- #

import png
import math
from matplotlib import cm


# --- Class --- #


class Image:
    """
    do not modify attributes directly!
    """
    def __init__(self, raw):
        # raw: Assume two-dimensional array
        self.data = raw
        self.height = len(raw)
        self.width = len(raw[0])
        self.greyscale = False  # True signifies RGB quads instead of scalars
        self.stack = []  # Modifier Stack
        self.max_color = 1


# --- Low Level Functions --- #


def value_replace(value, check, new=0):
    if value == check:
        return new
    return value


def color_log(value, f=1024):
    return math.log(1 + value*f, 1 + f)


def color_sigmoid(value, alpha=0.2):
    sig = 1.0 / (1.0 + math.pow(math.e, -(value-0.5)/alpha))
    return (sig - 0.5) * 1.0 / (1.0 - 2.0 / (1.0 + math.pow(math.e, 0.5 / alpha))) + 0.5


def colormap(data, max_color, name=None):
    # Color mapping
    # See https://matplotlib.org/gallery/color/colormap_reference.html for stock colormaps.
    # Set to name to None for bw rendering
    # Apply colormap & Convert color data to suitable array for pypng output.
    return [[[int(k*max_color) for k in j] for j in i] for i in cm.get_cmap(name)(data)]


def colormap_grey(data, max_color):
    return [[int(j*max_color) for j in i] for i in data]


def normalize(data):
    maximum = max([max(i) for i in data])
    minimum = min([min(i) for i in data])
    return [[(j-minimum)/(maximum-minimum) for j in i] for i in data]


def normalize_reference(data, reference):
    # usually: reference = max_iteration
    return [[i/reference for i in row] for row in data]


def flatten(data):
    return [[i for rgba in row for i in rgba[:-1]] for row in data]

# --- Export Functions --- #


def write_raw_png(data, path, size, greyscale=False):
    with open(path, 'wb') as f:
        w = png.Writer(width=size[0], height=size[1], greyscale=greyscale)
        w.write(f, data)

