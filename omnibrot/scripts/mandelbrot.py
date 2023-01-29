#!/usr/bin/env python3

import sys
from argparse import ArgumentParser
import re

from omnibrot.pybrot import flow
from omnibrot.recipes.pyMandel import pyMandel
from omnibrot.deepixel.pyFPHPN import pyCFPHPN

def mandelbrot(args):
    parser = ArgumentParser(description="omnibrot: compute a Mandelbrot")
    parser.add_argument("--center", "-c", default="-0.5")
    parser.add_argument("--scale", "-x", default=2.5, type=float)
    parser.add_argument("--shape", "-s", default=1024)
    parser.add_argument("--iterations", "-i", default=2**17-1, type=int)
    parser.add_argument("--algorithm", default="relative")
    parser.add_argument("--img-f", default=1024)
    parser.add_argument("--img-a", default=0.2)
    parser.add_argument("filename")

    conf = parser.parse_args(args)
    matches = re.findall(r"(([+-]\ ?[\da-fA-F]*(\.[\da-fA-F]{8})+)|([+-]?\d*\.\d+)|([+-]?\d+))", conf.center)
    assert len(matches) in (1, 2)
    for i, m in enumerate(matches):
        if re.fullmatch(r"([+-]?\d*\.\d+)|([+-]?\d+)", m[0]):
            matches[i] = float(m[0])
        else:
            matches[i] = m[0]
    conf.center = pyCFPHPN(matches[0], matches[1])
    if "x" in conf.shape:
        conf.shape = [int(s) for s in conf.shape.split("x")]
    else:
        conf.shape = (int(conf.shape), int(conf.shape))
    conf.scale = conf.scale * (conf.shape[0]/conf.shape[1] + 1j)

    data, meta = pyMandel(conf.center, conf.scale, conf.shape, conf.iterations, conf.algorithm)
    mod, modm = flow.mandelbrot_remapping(data, conf.iterations, f=conf.img_f, alpha=conf.img_a)
    flow.colored_png(mod, conf.filename)

if __name__ == "__main__":
    sys.exit(mandelbrot(sys.argv[1:]))
