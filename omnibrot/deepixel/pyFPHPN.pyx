# distutils: language = c++
# distutils: sources = omnibrot/deepixel/deepixel.cpp

from libc.stdint cimport uint32_t as INT
NUM_SIZE = 4

from .deepixel cimport *
from omnibrot.common.complex cimport ComplexNumber


cdef FPHPN cFPHPN(object value) except *:
    if isinstance(value, pyFPHPN):
        return (<pyFPHPN> value).cobj
    elif isinstance(value, float):
        return FPHPN(<double> value)
    elif isinstance(value, int):
        return FPHPN(<int> value)
    elif isinstance(value, str):
        chars = value.encode("UTF-8")  # Python byte string
        return FPHPN(<char*> chars)
    else:
        raise TypeError(f"object of type {type(value)} cannot be converted to FPHPN") 


cdef ComplexNumber[FPHPN] cCFPHPN(object value) except *:
    if isinstance(value, pyCFPHPN):
        return (<pyCFPHPN> value).cobj
    elif isinstance(value, complex):
        return ComplexNumber[FPHPN](cFPHPN(value.real), cFPHPN(value.imag))
    elif isinstance(value, tuple) and len(value) == 2:
        return ComplexNumber[FPHPN](cFPHPN(value[0]), cFPHPN(value[1]))
    else:
        return ComplexNumber[FPHPN](cFPHPN(value))


cdef class pyFPHPN:
    def __init__(self, value = 0):
        self.cobj = cFPHPN(value)

    @staticmethod
    cdef pyFPHPN wrap(FPHPN value):
        result = pyFPHPN()
        result.cobj = value
        return result

    @property
    def neg(self):  # ~ sign
        return self.cobj.neg
    
    @property
    def digits(self):
        return [self[i] for i in range(len(self))]
    
    def __add__(self, other):
        return pyFPHPN.wrap(cFPHPN(self) + cFPHPN(other))
    
    def __radd__(self, other):
        return pyFPHPN.wrap(cFPHPN(other) + cFPHPN(self))

    def __sub__(self, other):
        return pyFPHPN.wrap(cFPHPN(self) - cFPHPN(other))
    
    def __rsub__(self, other):
        return pyFPHPN.wrap(cFPHPN(other) - cFPHPN(self))

    def __neg__(self):
        result: pyFPHPN = self.copy()
        result.cobj.neg = not self.cobj.neg
        return result
    
    def __mul__(self, other):
        return pyFPHPN.wrap(cFPHPN(self).mult(cFPHPN(other)))

    def __rmul__(self, other):
        return pyFPHPN.wrap(cFPHPN(other).mult(cFPHPN(self)))
    
    def copy(self) -> pyFPHPN:
        result = pyFPHPN()
        result.cobj = self.cobj
        return result
    
    def __str__(self):
        return self.cobj.ret_hex_all().decode()
    
    def __repr__(self):
        return f"<pyFPHPN {str(self)}>"
    
    def __float__(self):
        return <double> self.cobj
    
    def __getitem__(self, int key):
        if 0 <= key < NUM_SIZE:
            return self.cobj.digits[key]
        raise IndexError(f"index has to be in range 0 - {NUM_SIZE-1}")
    
    def __len__(self):
        return NUM_SIZE


cdef class pyCFPHPN:
    def __init__(self, real = 0, imag = 0):
        if imag == 0:
            self.cobj = cCFPHPN(real)
        else:
            self.cobj = ComplexNumber[FPHPN](cFPHPN(real), cFPHPN(imag))
    
    @staticmethod
    cdef pyCFPHPN wrap(ComplexNumber[FPHPN] value):
        result = pyCFPHPN()
        result.cobj = value
        return result
    
    @property
    def real(self):
        return pyFPHPN.wrap(self.cobj.real)
    
    @property
    def imag(self):
        return pyFPHPN.wrap(self.cobj.imag)
    
    def __add__(self, other):
        return pyCFPHPN.wrap(cCFPHPN(self) + cCFPHPN(other))
    
    def __radd__(self, other):
        return pyCFPHPN.wrap(cCFPHPN(other) + cCFPHPN(self))
    
    def __sub__(self, other):
        return pyCFPHPN.wrap(cCFPHPN(self) - cCFPHPN(other))

    def __rsub__(self, other):
        return pyCFPHPN.wrap(cCFPHPN(other) - cCFPHPN(self))
    
    def __mul__(self, other):
        return pyCFPHPN.wrap(cCFPHPN(self) * cCFPHPN(other))

    def __rmul__(self, other):
        return pyCFPHPN.wrap(cCFPHPN(other) * cCFPHPN(self))
    
    def copy(self) -> pyCFPHPN:
        return pyCFPHPN.wrap(self.cobj)
    
    def __repr__(self):
        return f"<pyCFPHPN {str(self.real)} {str(self.imag)} j>"
    
    def __complex__(self):
        return complex(<double> self.cobj.real, <double> self.cobj.imag)


