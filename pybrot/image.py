"""
image.py
by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 & Mittagskogel
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

def color_log(value, f=1024):
    return math.log(1 + value*f, 1 + f)


def color_sigmoid(value, alpha=0.2):
    sig = 1.0 / (1.0 + math.pow(math.e, -(value-0.5)/alpha))
    return (sig - 0.5) * 1.0 / (1.0 - 2.0 / (1.0 + math.pow(math.e, 0.5 / alpha))) + 0.5


def colormap(data, bit_depth, name=None):
    # Color mapping
    # See https://matplotlib.org/gallery/color/colormap_reference.html for stock colormaps.
    # Set to name to None for bw rendering
    if (name):
        colormap = cm.ScalarMappable(cmap=name).to_rgba(data, norm=False)
        return [[int(value * (2**bit_depth-1)) for value in rgba[:-1]] for rgba in colormap]
    else:
        return colormap_grey(data, bit_depth)


def colormap_grey(data, max_color):
    return [int(value * (2**bit_depth-1)) for value in data]


def normalize(data, minimum, maximum):
    return [(value-minimum)/(maximum-minimum) for value in data]


# --- Export Functions --- #


def write_raw_png(data, path, size, greyscale=False):
    with open(path, 'wb') as f:
        w = png.Writer(width=size[0], height=size[1], greyscale=greyscale)
        w.write(f, data)

