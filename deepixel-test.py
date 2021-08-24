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
import numpy as np

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

print('Open file %s' % path_asdf, end='...   ', flush=True)
af = asdf.open(path_asdf, copy_arrays=True)
print('done')

print('Read file', end='...                ', flush=True)
data = np.array(af.tree['data-raw'])
maxIteration = int(af.tree['debug']['original-arguments']['max-Iter'])
minIteration = data.min()
samples = [len(data[0]), len(data)]
print('done')
print('  maxIteration: %d' % maxIteration)
print('  minIteration: %d' % minIteration)
print('  samples: %dx%d (%.2fMP)' % (samples[0], samples[1], samples[0] * samples[1] / 1000000.0))

# set non-escaping to 0
print('Zero unescaped samples', end='...   ', flush=True)
zero = np.array(range(maxIteration+1))
zero[-1] = minIteration
# apply lookup table to data
# same as [zero[value] for value in data]
data = zero[data]
print('done')

# remap
print('Remap samples', end='...            ', flush=True)
maxIteration = data.max()
lookup = np.linspace(0, 1, maxIteration-minIteration+1)
lookup = [pybrot.image.color_sigmoid(pybrot.image.color_log(value, f=maxIteration), alpha=0.2) for value in lookup]
print('done')
print('  maxIteration (true): %d' % maxIteration)

# colormap (Set name to None for greyscale mapping)
print('Apply color map', end='...          ', flush=True)
lookup = np.array(pybrot.image.colormap(lookup, bit_depth=8, name='plasma'))
print('done')

# apply lookup table to data
print('Apply lookup table', end='...       ', flush=True)
# apply lookup table to data
# same as [lookup[value] for value in data]
data = lookup[data-minIteration]
# flatten rgb value tuples in data
if(isinstance(lookup[0], np.ndarray)):
    data = data.reshape([samples[1], samples[0]*3])
print('done')

# export image
print('Export image to %s' % path_output, end='...   ', flush=True)
pybrot.image.write_raw_png(data, path_output, samples, greyscale=(not isinstance(lookup[0], np.ndarray)))
print('done')
