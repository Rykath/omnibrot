#!./venv/bin/python3
"""
deepixel-test.py
test deepixel library - simple mandelbrot
 - with pybrot.image/png
by Mittagskogel
 & Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
"""

import pybrot
import pybrot.image as img

import numpy as np
import asdf
import sys

mode = ''
path_asdf = ''
path_asdf2 = ''
path_output = ''
try:
    mode = sys.argv[1]
    if mode == 'convert':
        path_asdf = sys.argv[2]
        path_output = sys.argv[3]
    elif mode == 'diff':
        path_asdf = sys.argv[2]
        path_asdf2 = sys.argv[3]
        path_output = sys.argv[4]
    else:
        raise ValueError
except (IndexError, ValueError):
    print('Wrong number of Arguments')
    print("Usage: <program> 'convert' <path_to_input_asdf> <path_to_output_png>")
    print("Usage: <program> 'diff' <path_to_input_asdf-1> <path_to_input_asdf-2> <path_to_output_asdf-2>")
    exit(1)

af = asdf.open(path_asdf)
data = np.asarray(af.tree['data-raw'], dtype=float)
maxIteration = int(af.tree['debug']['original-arguments']['max-Iter'])

data2 = []
if mode == 'diff':
    af2 = asdf.open(path_asdf2)
    data2 = np.asarray(af2.tree['data-raw'], dtype=float)
    maxIteration2 = int(af2.tree['debug']['original-arguments']['max-Iter'])

print('data collected')

# set non-escaping to 0
data[data == maxIteration] = 17905 #0
if mode == 'diff':
    data = data - data2
# normalize
data = img.normalize(data)
# intensity mapping
data = img.color_sigmoid(img.color_log(data, f=256), alpha=0.8)
# colormap
data = img.colormap(data, max_color=255, name='plasma')
datal = img.flatten(data) # array -> list

# export
img.write_raw_png(datal, path_output, data.shape, greyscale=False)

print('done - file written to: '+path_output)
