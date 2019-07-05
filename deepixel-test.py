"""
deepixel-test.py
test deepixel library - simple mandelbrot
 - with pybrot.image/png
by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
"""

import pybrot
import subprocess

path_deepixel = '.cmake.lnk/deepixel-test-basics'
path_output = '.test/deepixel-test.png'

""" = DESCRIPTION =
 * c++ program takes input as arguments (cr, ci, maxIter)
 * collect escapetime from stdout
 * output everything as png
"""

width = 512
height = width
maxColor = 255
maxIteration = 1024
center = 0+0j
size = 4+4j
spacing = size.real/width + 1j*size.imag/height


def color(raw):
    return int(raw*maxColor/maxIteration)


data = []

for h in range(height):
    data.append([])
    if h in range(1, height, height//10):
        print('calculating row {} of {}'.format(h, height))
    for w in range(width):
        c = center + (w-width//2)*spacing.real + 1j*(h-height//2)*spacing.imag
        data[-1].append(color(int(subprocess.run([path_deepixel, str(c.real), str(c.imag), str(maxIteration)],
                                                 stdout=subprocess.PIPE).stdout)))

print('data collected')

img = pybrot.image.RawImage(data)
img.write_png(path_output)

print('done - file written to: '+path_output)