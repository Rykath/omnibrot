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
        
        # Check for color or bw image
        if(isinstance(self.data[0][0], list)):
            self.data = [[k for j in i for k in j[:-1]] for i in data]
            self._greyscale = False
        else:
            self._greyscale = True

    def write_png(self, path):
        with open(path, 'wb') as f:
            w = png.Writer(width=self._width, height=self._height, greyscale=self._greyscale)
            w.write(f, self.data)
