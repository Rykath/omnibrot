"""
deepixel-test.py
test deepixel library - simple mandelbrot
 - with pybrot.image/png
by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
"""

import pybrot

import subprocess
import math

path_deepixel = './build/deepixel-mandelbrot'
path_output = './results/deepixel_4+2_'

""" = DESCRIPTION =
 * c++ program takes input as arguments (cr, ci, maxIter)
 * collect escapetime from stdout
 * output everything as png
"""

width = 1024
height = width
maxColor = 255
maxIteration = 1024
center = -1.484935+0j
size = 0.000005+0.000005j
spacing = size.real/width + 1j*size.imag/height
datestr = '190709'

path_output += '{}sq_{}i_{}_{}_{}sq_{}'.format(width, maxIteration, center.real, center.imag, size.real, datestr)


def color_linear(raw):
    return int(raw*maxColor/maxIteration)


def color_log(raw):
    return int(math.log(raw, maxIteration)*maxColor)


def color_sin_pi(raw):
    return color_linear(raw)+int(255/math.pi*math.sin(raw/1024*math.pi))


dataIn = subprocess.run([path_deepixel, '{}x{}'.format(width, height), str(size.real), str(size.imag), str(center.real),
                         str(center.imag), str(maxIteration)], encoding="utf-8", stdout=subprocess.PIPE).stdout

data = [[color_log(int(i)) for i in line.split()] for line in dataIn.strip().split('\n')]

print('data collected')

img = pybrot.image.RawImage(data)
img.write_png(path_output)

print('done - file written to: '+path_output)
