/* deepixel.hpp
 * deepixel - header file
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * Deepixel - Fixed Point High Precision Number
 *  - with overlying Complex Numbers
 *  - with optimized functions to calculate escaptimes and escapepaths
 */

// ===== Deepixel - Header ===== //
#ifndef DEEPIXEL_HPP
#define DEEPIXEL_HPP

// --- Includes --- //
#include <cstdint>

#include "../common/complex.hpp"

// --- Configuration --- //
#define NUM_SIZE 4    // length of the High Precision Number in subunits
#define CALC_SIZE 2   // number of additional subunits during calculation

// --- Types --- //
#define INT uint32_t  // unsigned base type
#define INT2 uint64_t // unsigned buffer to store INT, twice as big as INT
#define INT2S int64_t // signed buffer, same size as INT2
#define INT_BITS 32u // number of bits of INT
#define INT_FULL 4294967295u // 2^32-1 -- 32 bits of 1 = 0x FFFF FFFF
#define INT_SIZE  4294967296u // 2^32   -- number of values of 32 bits = 0x 1 0000 0000

typedef INT2 HPNCarry [NUM_SIZE+CALC_SIZE]; // High Precision Number (unsigned) Carry
typedef INT2S HPNSignedCarry [NUM_SIZE+CALC_SIZE]; // High Precision Number Signed Carry

// --- Names --- //
#define CFPHPN ComplexNumber<FPHPN> // Complex FPHPN

// === New Base Type === //

class FPHPN { // Fixed Point High Precision Number
public:
  bool neg;
  INT digits[NUM_SIZE];

  FPHPN();
  FPHPN(double);
  FPHPN(int);
  FPHPN(char*); // reverse of ret_hex and ret_hex_all() / reverse of str(FPHPN)

  FPHPN mult (const FPHPN&);
  FPHPN mult_lc (const FPHPN&); // alternate implementation of multiplication using a large carry

  char* ret_hex();
  char* ret_hex_all();

  operator double();
};

FPHPN operator+ (const FPHPN&, const FPHPN&);
FPHPN operator- (const FPHPN&, const FPHPN&);
inline FPHPN operator* (FPHPN A, const FPHPN& B){ return A.mult_lc(B);}

bool operator< (const FPHPN&, const FPHPN&);
inline bool operator> (const FPHPN& A, const FPHPN& B){ return B < A;}

inline char* str(FPHPN X){ return X.ret_hex_all();}

// === ComplexNumber === //

// --- Special --- //

CFPHPN next_iteration_opt(CFPHPN, CFPHPN); // optimized, low-level implementation

bool calc_esc_double(CFPHPN); // calc `esc`: |Z| > 2 - via conversion to double
bool calc_esc_opt(CFPHPN);    // calc `esc`: |Z| > 2 - via approximations and FPHPN operators
bool calc_esc_ref(CFPHPN);    // calc `esc`: norm > 4 - via norm

// --- Default --- //

template <> inline CFPHPN next_iteration(CFPHPN Z, CFPHPN C){ return next_iteration_opt(Z,C);}
template <> inline bool calc_esc(CFPHPN X){ return calc_esc_opt(X);}

#endif
