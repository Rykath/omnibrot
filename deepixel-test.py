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

af = asdf.open(path_asdf)
data = [[float(i) for i in j] for j in af.tree['data-raw']]
maxIteration = int(af.tree['debug']['original-arguments']['max-Iter'])
samples = [len(data[0]), len(data)]

print('data collected')

# set non-escaping to 0
data = [[pybrot.image.value_replace(value, maxIteration, 0) for value in row] for row in data]
# normalize
data = pybrot.image.normalize(data)
# intensity mapping
data = [[pybrot.image.color_sigmoid(pybrot.image.color_log(value, f=256), alpha=0.8) for value in row] for row in data]

# colormap
data = pybrot.image.colormap(data, max_color=255, name='viridis')
data = pybrot.image.flatten(data)

# export
pybrot.image.write_raw_png(data, path_output, samples, greyscale=False)

print('done - file written to: '+path_output)
