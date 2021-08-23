# distutils: language = c++

cdef extern from "../common/complex.hpp":
    cdef cppclass ComplexNumber[BaseType]:
        BaseType real
        BaseType imag
        
        ComplexNumber() except +
        ComplexNumber(BaseType) except +
        ComplexNumber(BaseType, BaseType) except +
    
    ComplexNumber[BaseType] operator+[BaseType] (ComplexNumber[BaseType], ComplexNumber[BaseType])
    ComplexNumber[BaseType] operator-[BaseType] (ComplexNumber[BaseType], ComplexNumber[BaseType])
    ComplexNumber[BaseType] operator*[BaseType] (ComplexNumber[BaseType], ComplexNumber[BaseType])
    
    BaseType norm[BaseType] (ComplexNumber[BaseType])
    ComplexNumber[BaseType] pow[BaseType] (ComplexNumber[BaseType], unsigned int)
    
    ComplexNumber[BaseType] next_iteration_ref[BaseType] (ComplexNumber[BaseType], ComplexNumber[BaseType])
    ComplexNumber[BaseType] next_iteration[BaseType] (ComplexNumber[BaseType], ComplexNumber[BaseType])
    bint calc_esc_ref[BaseType] (ComplexNumber[BaseType])
    bint calc_esc[BaseType] (ComplexNumber[BaseType])
    int escapetime[BaseType] (ComplexNumber[BaseType], ComplexNumber[BaseType], int)

