# distutils: language = c++
""" Cython declarations for mandel.cpp """

from omnibrot.common.complex cimport ComplexNumber
from omnibrot.deepixel.deepixel cimport FPHPN

cdef extern from "mandel.hpp":
    #void mandel[INT, BaseType] (INT*, ComplexNumber[BaseType], ComplexNumber[BaseType], ComplexNumber[int], INT)
    void mandel(long*, ComplexNumber[FPHPN], ComplexNumber[FPHPN], ComplexNumber[int], long)
    #void mandel[INT, CenterBaseType, RelativeBaseType] (INT*, ComplexNumber[CenterBaseType], ComplexNumber[RelativeBaseType], ComplexNumber[int], INT)
    void mandel(long*, ComplexNumber[FPHPN], ComplexNumber[double], ComplexNumber[int], long)

