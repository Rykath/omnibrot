# distutils: language = c++
# distutils: sources = omnibrot/deepixel/deepixel.cpp omnibrot/recipes/mandel.cpp
# distutils: include_dirs = omnibrot/deepixel

import numpy as np

from .mandel cimport mandel
from omnibrot.common.complex cimport ComplexNumber
from omnibrot.deepixel.pyFPHPN cimport FPHPN, cFPHPN, pyFPHPN, cCFPHPN, pyCFPHPN

def pyMandel(center, ranges, shape, maxIter, flavor="default"):
    data = np.ndarray(shape[::-1], dtype=long)
    meta = {"type": "pyMandel", "center": center, "range": ranges, "shape": shape, "maximum iteration": maxIter, "flavor": flavor}

    cdef long [:, ::1] view = data

    if flavor == "default":
        meta["algorithm"] = "FPHPN"
        mandel(&view[0, 0], 
            cCFPHPN(center), 
            cCFPHPN(ranges), 
            ComplexNumber[int](shape[0], shape[1]), 
            <long> maxIter)
    elif flavor == "fast":
        meta["algorithm"] = "double"
        mandel(&view[0, 0], 
            ComplexNumber[double](center.real, center.imag), 
            ComplexNumber[double](ranges.real, ranges.imag), 
            ComplexNumber[int](shape[0], shape[1]), 
            <long> maxIter)
    elif flavor == "relative":
        meta["algorithm"] = "relative(FPHPN, double)"
        ranges = complex(ranges)
        mandel(&view[0, 0], 
            cCFPHPN(center), 
            ComplexNumber[double](ranges.real, ranges.imag), 
            ComplexNumber[int](shape[0], shape[1]), 
            <long> maxIter)
    else:
        raise ValueError(f"unknown value '{flavor}' of flavor")

    return data, meta



