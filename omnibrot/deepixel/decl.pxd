# distutils: language = c++
# distutils: sources = ./deepixel.cpp
# distutils: include_dirs = ./

from libc.stdint cimport uint32_t as INT

cdef extern from "deepixel.hpp":
    cdef cppclass FPHPN:
        char neg
        INT* digits
    
        FPHPN() except +
        #FPHPN(int) except +
        #FPHPN(double) except +
        #FPHPN(char*) except +
        
        #FPHPN mult (const FPHPN&)
        #FPHPN mult_lc (const FPHPN&)
        
        #char* ret_hex()
        #char* ret_hex_all()
        
        #operator double()
    
    #FPHPN operator+ (const FPHPN&, const FPHPN&)
    #FPHPN operator- (const FPHPN&, const FPHPN&)

    #bool operator< (const FPHPN&, const FPHPN&)
        
