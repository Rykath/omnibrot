"""
image.py
by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
"""


import png


class RawImage:
    def __init__(self, data):
        self.data = data
        self._width = len(data[0])
        self._height = len(data)

    def write_png(self, path):
        with open(path, 'wb') as f:
            w = png.Writer(width=self._width, height=self._height, greyscale=True)
            w.write(f, self.data)

