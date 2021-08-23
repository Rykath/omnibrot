from libc.stdint cimport uint32_t as INT

from omnibrot.common.complex cimport ComplexNumber

cdef extern from "deepixel.hpp":
    cdef cppclass FPHPN:
        bint neg
        INT* digits

        FPHPN() except +
        FPHPN(int) except +
        FPHPN(double) except +
        FPHPN(char*) except +

        char* ret_hex()
        char* ret_hex_all()
        FPHPN mult(FPHPN)
        FPHPN mult_lc(FPHPN)
        double operator()

    FPHPN operator+(FPHPN, FPHPN)
    FPHPN operator-(FPHPN, FPHPN)

    ComplexNumber[FPHPN] next_iteration_opt(ComplexNumber[FPHPN], ComplexNumber[FPHPN])
    bint calc_esc_opt(ComplexNumber[FPHPN])


cdef FPHPN cFPHPN(object) except +
cdef ComplexNumber[FPHPN] cCFPHPN(object) except +


cdef class pyFPHPN:
    cdef FPHPN cobj

    @staticmethod
    cdef pyFPHPN wrap(FPHPN)


cdef class pyCFPHPN:
    cdef ComplexNumber[FPHPN] cobj

    @staticmethod
    cdef pyCFPHPN wrap(ComplexNumber[FPHPN])

