# Omnibrot
## Requirements
* cmake
  * C/C++
* python 3
  * pypng
  * matplotlib
  * asdf

## Current Toolchain & Workflow

* recipes/mandelbrot
  * calculate a simple Mandelbrot with High Precision
  * Output raw data in ASDF format
* deepixel-test.py
  * read raw data & metadata from ASDF format
  * apply coloration
  * export in png format

## Components
* *common*
  * *asdf* - helper functions to write in simple ASDF format
  * *complex* - complex number class template
* *deepixel* - high precision calculation
  * *deepixel* - high precision fixed point arithmetic
* *pybrot* - python utilities
  * *image* - helper functions to easily colorize and export an image
* *recipes*
  * *mandelbrot* - simple mandelbrot using *deepixel* `FPHPN` and exporting ASDF
* *test-deepixel* - simple recolor and export from ASDF to png
