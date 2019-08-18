#!./venv/bin/python3
"""
deepixel-test.py
test deepixel library - simple mandelbrot
 - with pybrot.image/png
by Mittagskogel
 & Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
"""

import pybrot

import asdf
import sys
import numpy as np

# Arguments: <path_asdf> <path_output>
path_asdf = ''
path_output = ''
try:
    path_asdf = sys.argv[1]
    path_output = sys.argv[2]
except (IndexError, ValueError):
    print('Wrong number of Arguments')
    print('Usage: <program> <path_to_input_asdf> <path_to_output_png>')
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
zero = np.array([value for value in range(maxIteration+1)])
zero[-1] = minIteration
data = zero[data]
print('done')

# normalize
print('Normalize samples', end='...        ', flush=True)
maxIteration = data.max()
lookup = list(range(minIteration, maxIteration+1))
lookup = pybrot.image.normalize(lookup, minimum=minIteration, maximum=maxIteration)
print('done')
print('  maxIteration (true): %d' % maxIteration)

# remap
print('Remap samples', end='...            ', flush=True)
lookup = [pybrot.image.color_sigmoid(pybrot.image.color_log(value, f=maxIteration), alpha=0.2) for value in lookup]
print('done')

# colormap (Set name to None for greyscale mapping)
print('Apply color map', end='...          ', flush=True)
lookup = pybrot.image.colormap(lookup, bit_depth=8, name='plasma')
print('done')

# apply lookup table to data
print('Apply lookup table', end='...       ', flush=True)
lookup = np.array(lookup)
data = lookup[data-minIteration]
if(isinstance(lookup[0], np.ndarray)):
    data = data.reshape([samples[1], samples[0]*3])
print('done')

# export image
print('Export image to %s' % path_output, end='...   ', flush=True)
pybrot.image.write_raw_png(data, path_output, samples, greyscale=(not isinstance(lookup[0], np.ndarray)))
print('done')
