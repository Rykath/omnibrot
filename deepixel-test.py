"""
deepixel-test.py
test deepixel library - simple mandelbrot
 - with pybrot.image/png
by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
"""

import pybrot

import subprocess
import math
import time

from matplotlib import cm


# TODO: remove
def color_linear(raw):
    if(raw == maxIteration):
        return 0
    else:
        return raw


def color_log(raw):
    if(raw == maxIteration):
        return 0
    else:
        #return math.log(raw, maxIteration)
        f = 1024.0
        return math.log(1 + raw*f, 1 + f)

def color_sigmoid(raw):
    if(raw == maxIteration):
        return 0
    else:
        alpha = 0.2
        sig = 1.0 / (1.0 + math.pow(math.e, -(raw-0.5)/alpha))
        return (sig - 0.5) * 1.0 / (1.0 - 2.0 / (1.0 + math.pow(math.e, 0.5 / alpha))) + 0.5

# TODO: fix color_sin_pi
def color_sin_pi(raw):
    if(raw == maxIteration):
        return 0
    else:
        return color_linear(raw)+int(255/math.pi*math.sin(raw/1024*math.pi))



path_deepixel = './build/deepixel-mandelbrot'
path_output = './results/deepixel_4+2_'

""" = DESCRIPTION =
 * c++ program takes input as arguments (cr, ci, maxIter)
 * collect escapetime from stdout
 * output everything as png
"""

width = 512
height = width
maxColor = 255
maxIteration = 1024
center = -0.5+0.0j
size = 3.0+3.0j
spacing = size.real/width + 1j*size.imag/height
datestr = time.strftime("%Y%m%d%H%M%S", time.gmtime())

# Normalize
normalize = True

# Intensity mapping
im = lambda x: color_sigmoid(color_log(x))

# Color mapping
# See https://matplotlib.org/gallery/color/colormap_reference.html for stock colormaps.
# Set to None for bw rendering
cm_map = cm.get_cmap('viridis')



path_output += '{}sq_{}i_{}_{}_{}sq_{}.png'.format(width, maxIteration, center.real, center.imag, size.real, datestr)

dataIn = subprocess.run([path_deepixel, '{}x{}'.format(width, height), str(size.real), str(size.imag), str(center.real),
                         str(center.imag), str(maxIteration)], encoding="utf-8", stdout=subprocess.PIPE).stdout

if(normalize):
    fdata = [[0 if int(i) == maxIteration else int(i) for i in line.split()] for line in dataIn.strip().split('\n')]
    maximum = max([max(i) for i in fdata])
    minimum = min([min(i) for i in fdata])
    fdata = [[im((j-minimum)/(maximum-minimum)) for j in i] for i in fdata]
else:
    fdata = [[im(0 if int(i) == maxIteration else int(i)/maxIteration) for i in line.split()] for line in dataIn.strip().split('\n')]

if(cm_map):
    # Apply colormap.
    fdata = cm_map(fdata)
    
    # Convert color data to suitable array for pypng output.
    data = [[[int(k*maxColor) for k in j] for j in i] for i in fdata]
else:
    data = [[int(j*maxColor) for j in i] for i in fdata]

print('data collected')

img = pybrot.image.RawImage(data)
img.write_png(path_output)

print('done - file written to: '+path_output)
