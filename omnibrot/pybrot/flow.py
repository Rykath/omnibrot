from . import image

from matplotlib import cm
from PIL import Image


def mandelbrot_remapping(data, maximum_iteration, f = 1024, alpha = 0.2):
    stack = [
        #image.Replacement(maximum_iteration, 0),  # set non-escaping to 0
        image.Normalization(),  # default: normalize to [0, 1]
        image.Logarithm(f),
        image.Sigmoid(alpha),
    ]
    meta = []
    for modifier in stack:
        data = modifier(data)
        meta.append(modifier.meta)
    return data, meta


def colored_png(data, path, cmap_name = "cividis"):
    im = Image.fromarray(cm.get_cmap(cmap_name)(data, bytes=True))
    im.save(path)

