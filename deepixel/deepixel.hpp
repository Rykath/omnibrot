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
#define FPHPN FPHPN // Fixed Point High Precision Number
#define CFPHPN CFPHPN // Complex Fixed Point High Precision Number

// === Classes === //

class FPHPN {
public:
  bool neg;
  INT digits[NUM_SIZE];

  FPHPN();
  FPHPN(double);

  FPHPN mult (const FPHPN&);
  FPHPN mult_lc (const FPHPN&); // alternate implementation of multiplication using a large carry

  FPHPN operator+ (const FPHPN&);
  FPHPN operator- (const FPHPN&);
  FPHPN operator* (const FPHPN& other){ return mult_lc(other);};

  double ret_double();
  char* ret_hex();
  char* ret_hex_all();
};

class CFPHPN {
public:
  FPHPN real;
  FPHPN imag;
  bool esc;

  CFPHPN();
  CFPHPN(double, double);
  CFPHPN(FPHPN, FPHPN);

  CFPHPN next_iter_opt(CFPHPN); // optimized, low-level implementation
  CFPHPN next_iter_ref(CFPHPN); // reference, implementation using FPHPN operators

  CFPHPN next_iteration(CFPHPN C){ return next_iter_opt(C);};
};

// === Functions === //

int escapetime(CFPHPN, CFPHPN, int);

#endif
