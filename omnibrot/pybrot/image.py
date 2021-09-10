"""
image.py
by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 & Mittagskogel
"""

# --- Imports --- #

import numpy as np
from matplotlib import cm
from abc import ABC, abstractmethod
from dataclasses import dataclass


# --- --- #


class Modifier(ABC):
    @abstractmethod
    def __call__(self, values: np.ndarray) -> np.ndarray:
        pass

    @property
    def meta(self) -> dict:
        return {"name": self.__class__.__name__}


@dataclass
class Replacement(Modifier):
    check: int
    new: int = 0

    def __call__(self, values: np.ndarray) -> np.ndarray:
        values[values == self.check] = self.new
        return values


@dataclass
class Logarithm(Modifier):
    f: float = 1024

    def __call__(self, values: np.ndarray) -> np.ndarray:
        # assumes normalized values between 0 and 1
        return np.log(1 + values * self.f) / np.log(1 + self.f)


@dataclass
class Sigmoid(Modifier):
    alpha: float = 0.2

    def __call__(self, values: np.ndarray) -> np.ndarray:
        sig = 1.0 / (1.0 + np.exp( -(values - 0.5) / self.alpha))
        return (sig - 0.5) * 1.0 / (1.0 - 2.0 / (1.0 + np.exp(0.5 / self.alpha))) + 0.5


def colormap(data, bit_depth, name=None):
    # Color mapping
    # See https://matplotlib.org/gallery/color/colormap_reference.html for stock colormaps.
    # Set to name to None for bw rendering
    if (name):
        colormap = cm.ScalarMappable(cmap=name).to_rgba(data, norm=False)
        return np.array([[int(value * (2**bit_depth-1)) for value in rgba[:-1]] for rgba in colormap])
    else:
        return colormap_grey(data, bit_depth)


@dataclass
class Normalization(Modifier):
    maximum: float = None
    minimum: float = None

    def __call__(self, values: np.ndarray) -> np.ndarray:
        maximum = values.max() if self.maximum is None else self.maximum
        minimum = values.min() if self.minimum is None else self.minimum
        assert(maximum != minimum)
        return (values - minimum) / (maximum - minimum)


def colormap_grey(data, bit_depth):
    return [int(value * (2**bit_depth-1)) for value in data]


def flatten(data):
    # keep old code
    return [[i for rgba in row for i in rgba[:-1]] for row in data]


