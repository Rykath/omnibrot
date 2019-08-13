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
data = [[float(i) for i in j] for j in af.tree['data-raw']]
maxIteration = int(af.tree['debug']['original-arguments']['max-Iter'])
samples = [len(data[0]), len(data)]

data2 = []
if mode == 'diff':
    af2 = asdf.open(path_asdf2)
    data2 = [[float(i) for i in j] for j in af2.tree['data-raw']]
    maxIteration2 = int(af2.tree['debug']['original-arguments']['max-Iter'])
    samples = [len(data2[0]), len(data2)]

print('data collected')

# set non-escaping to 0
data = [[pybrot.image.value_replace(value, maxIteration, 0) for value in row] for row in data]
if mode == 'diff':
    data = [[data[i][j]-data2[i][j] for j, _ in enumerate(row)] for i, row in enumerate(data)]
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
