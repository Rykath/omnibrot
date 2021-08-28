# distutils: language = c++

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


