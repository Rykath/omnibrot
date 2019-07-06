/* deepixel.cpp
 * deepixel - source file
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * Deepixel - Fixed Point High Precision Number
 *  - with overlying Complex Numbers
 *  - with optimized functions to calculate escaptimes and escapepaths
 */

#include "deepixel.hpp"

#include <cstdio> // printf
#include <cstdbool> // boolean variables
#include <cstdlib> // calloc
#include <cstdint> // int types
#include <cmath> // ceil
#include <cstring> // memset

// ----- Constants ----- //

#define CONST_2SQRT2 2.82843 // 2*sqrt(2) rounded up

// ===== FPHPN ===== //

char fphpn_larger(FPHPN** s, FPHPN** l, FPHPN num1, char c1, FPHPN num2, char c2){
  // determine larger FPHPN
  // based on absolute values (.digits), ignoring sign (.neg)
  for (int i = 0; i < NUM_SIZE; i++){
    if (num1.digits[i] > num2.digits[i]){
      *s = &num2;
      *l = &num1;
      return c1;
    }
    else if (num1.digits[i] < num2.digits[i]){
      *s = &num1;
      *l = &num2;
      return c2;
    }
  }
  // default: s = num1, l = num2
  *s = &num1;
  *l = &num2;
  return '=';
}

// --- Constructors --- //

FPHPN::FPHPN(){
  neg = false;
  std::memset(digits, 0, sizeof(digits));
}

FPHPN::FPHPN(double number){
  if (number == 0){
    neg = false;
    std::memset(digits, 0, sizeof(digits));
    return;
  }
  else if (number > 0){
    neg = false;
  }
  else {
    neg = true;
    number *= -1;
  }
  for (int i = 0; i < NUM_SIZE; i++){
    digits[i] = (INT) number;
    number -= digits[i];
    number *= INT_SIZE;
  }
}

// --- Calculation & Operators --- //

FPHPN FPHPN::operator+(const FPHPN& other){
  FPHPN res;
  // C = A + B
	if (neg == other.neg){
		// --- Addition --- //
		res.neg = neg;
    INT2 buffer = 0;
    for (int i=NUM_SIZE-1; i>=0; i--){
      buffer += digits[i];
      buffer += other.digits[i];
      res.digits[i] = buffer;
      buffer = buffer >> INT_BITS;
    }
	}
	else{
	  // --- Subtraction --- //
    FPHPN* hpnLptr;
    FPHPN* hpnSptr;
    char larger = fphpn_larger(&hpnSptr, &hpnLptr, *this, 'A', other, 'B');
    if (larger == '='){
      res.neg = false;
      for (int i=0; i<NUM_SIZE; i++){
        res.digits[i] = 0;
      }
      return res;
    }
    res.neg = hpnLptr->neg;
    INT2 buffer;
    INT2 carry = 0;
    for (int i=NUM_SIZE-1; i>=0; i--){
      buffer = carry + hpnSptr->digits[i];
      carry = (hpnLptr->digits[i] < buffer);
      res.digits[i] = hpnLptr->digits[i] - buffer;
    }
  }
	return res;
}

FPHPN FPHPN::operator-(const FPHPN& other){
  // depending on: operator+
  FPHPN sub = other;
  sub.neg = not sub.neg;
  return (*this + sub);
}

FPHPN FPHPN::mult(const FPHPN& other){
  FPHPN res;
  // C = A * B
  res.neg = neg != other.neg;
  INT2 buffer;
  INT2 carry0 = 0; // carry0 aligned with buffer
  INT2 carry1 = 0; // left shift INT_BITS from carry0
  for (int sum=NUM_SIZE+CALC_SIZE-1; sum >= 0; sum--){
    for (int i=0; i<=sum; i++){
      if (sum-i >= NUM_SIZE || i >= NUM_SIZE){
        continue;
      }
      buffer = digits[i];
      buffer *= other.digits[sum-i];
      carry0 += buffer & INT_FULL;   // bitwise AND -- lower INT
      carry1 += buffer >> INT_BITS;  // higher INT
    }
    if (sum < NUM_SIZE){
      res.digits[sum] = carry0 & INT_FULL;
    }
    carry1 += carry0 >> INT_BITS;
    carry0 = carry1;
    carry1 = carry0 >> INT_BITS;
    carry0 = carry0 & INT_FULL;
  }
  return res;
}

FPHPN FPHPN::mult_lc(const FPHPN& other){
  FPHPN res;
  // C = A * B
  res.neg = neg != other.neg;
  INT2 buffer;
  HPNCarry carry = {0};
  for (int sum=NUM_SIZE+CALC_SIZE-1; sum >= 0; sum--){
    for (int i=0; i<=sum; i++){
      if (sum-i >= NUM_SIZE || i >= NUM_SIZE){
        continue;
      }
      buffer = digits[i];
      buffer *= other.digits[sum-i];
      if (sum != 0){
        carry[sum] += buffer & INT_FULL;   // bitwise AND -- lower INT
        carry[sum-1] += buffer >> INT_BITS;  // higher INT
      }
      else{
        carry[sum] += buffer;
      }
    }
    if (sum < NUM_SIZE){
      res.digits[sum] = carry[sum] & INT_FULL;
    }
    if (sum != 0){
      carry[sum-1] += carry[sum] >> INT_BITS;
      carry[sum] = carry[sum] & INT_FULL;
    }
  }
  return res;
}

// --- Return Functions --- //

double FPHPN::ret_double(){
  double d = 0;
  double a;
  for (int i = 0; i < NUM_SIZE; i++){
    a = digits[i];
    for (int ii = 0; ii < i; ii++){
      a /= INT_SIZE;
    }
    d += a;
  }
  if (neg){
    d *= -1;
  }
  return d;
}

char* FPHPN::ret_hex(){
  // Return first 3 digits in hexadecimal text representation
  // Assuming INT_SIZE=32u, NUM_SIZE >= 3
  char* s = (char*) malloc(sizeof(char)*21);
  std::sprintf(s,"+%2X.%08X.%08X",digits[0],digits[1],digits[2]);
  if (neg){
    s[0] = '-';
  }
  return s;
}

char* FPHPN::ret_hex_all(){
  // Return all digits in hexadecimal text representation
  // Assuming INT_SIZE=32u
  // Related: ret_hex()
  char* s = (char*) malloc(sizeof(char)*(3+(NUM_SIZE-1)*9));
  std::sprintf(s, "+%2X",digits[0]);
  if (neg){
    s[0] = '-';
  }
  for (int i = 1; i < NUM_SIZE; i++){
    std::sprintf(s, "%s.%08X",s,digits[i]);
  }
  return s;
}

// ===== CFPHPN ===== //

// --- Constructors --- //

CFPHPN::CFPHPN(){
  real = FPHPN();
  imag = FPHPN();
  esc = false;
}

CFPHPN::CFPHPN(double r, double i){
  real = FPHPN(r);
  imag = FPHPN(i);
  esc = (r*r + i*i > 4);
}

CFPHPN::CFPHPN(FPHPN r, FPHPN i){
  real = r;
  imag = i;
  esc = (r*r + i*i).digits[0] >= 4;
}

// --- Calculation --- //

CFPHPN CFPHPN::next_iter_opt(CFPHPN C){
  INT2 bufferR, bufferI, bufferRI;
  HPNSignedCarry carryR = {0};
  HPNSignedCarry carryI = {0};
  bool x2;
  CFPHPN Zn;
  CFPHPN Z = *this;

  bool negRI = Z.real.neg ^ Z.imag.neg;

  for (int sum = NUM_SIZE+CALC_SIZE-1; sum >= 0; sum--){        //  loop carry
    for (int i = 0; i <= sum; i++){                             //    loop multiplication
      if (sum-i >= NUM_SIZE || i >= NUM_SIZE){ continue; }      //      catch borders
      if (i <= sum/2){                                          //      reduced loop for squaring Zr and Zi - calculate Zr
        bufferR = Z.real.digits[i];
        bufferR *= Z.real.digits[sum-i];
        bufferI = Z.imag.digits[i];
        bufferI *= Z.imag.digits[sum-i];
        x2 = (2*i != sum);                                      //      binomial formula, x2 for all non-squares
        if (sum == 0){
          carryR[sum] += bufferR << x2;
          carryR[sum] -= bufferI << x2;
        }
        else {
          carryR[sum] += (bufferR & INT_FULL) << x2;
          carryR[sum] -= (bufferI & INT_FULL) << x2;
          carryR[sum-1] += (bufferR >> INT_BITS) << x2;
          carryR[sum-1] -= (bufferI >> INT_BITS) << x2;
        }
      }
      bufferRI = Z.real.digits[i];                              //      multiplying Zr and Zi - calculate Zi
      bufferRI *= Z.imag.digits[sum-i];
      if (sum == 0){
        if (negRI){ carryI[sum] -= bufferRI << 1u;}
        else {      carryI[sum] += bufferRI << 1u;}
      }
      else {
        if (negRI){
          carryI[sum] -= (bufferRI & INT_FULL) << 1u;
          carryI[sum-1] -= (bufferRI >> INT_BITS) << 1u;
        }
        else {
          carryI[sum] += (bufferRI & INT_FULL) << 1u;
          carryI[sum-1] += (bufferRI >> INT_BITS) << 1u;
        }
      }
    }
    if (sum < NUM_SIZE){                                        //    Adding C
      if (C.real.neg){
        carryR[sum] -= C.real.digits[sum];
      }
      else {
        carryR[sum] += C.real.digits[sum];
      }
      if (C.imag.neg){
        carryI[sum] -= C.imag.digits[sum];
      }
      else {
        carryI[sum] += C.imag.digits[sum];
      }
    }
    if (sum != 0) {                                              // Reducing Carry and converting to positive
      if (carryR[sum] < 0) {
        carryR[sum - 1] -= (INT2) labs(carryR[sum]) >> INT_BITS;
        carryR[sum] = (INT2) labs(carryR[sum]) & INT_FULL;
        if (carryR[sum] > 0) {
          carryR[sum - 1] -= 1;
          carryR[sum] = INT_SIZE - carryR[sum];
        }
      } else {
        carryR[sum - 1] += (INT2) carryR[sum] >> INT_BITS;
        carryR[sum] = (INT2) carryR[sum] & INT_FULL;
      }
      if (carryI[sum] < 0) {
        carryI[sum - 1] -= (INT2) labs(carryI[sum]) >> INT_BITS;
        carryI[sum] = (INT2) labs(carryI[sum]) & INT_FULL;
        if (carryI[sum] > 0) {
          carryI[sum - 1] -= 1;
          carryI[sum] = INT_SIZE - carryI[sum];
        }
      } else {
        carryI[sum - 1] += (INT2) carryI[sum] >> INT_BITS;
        carryI[sum] = (INT2) carryI[sum] & INT_FULL;
      }
    }
  }
  for (int i = 0; i < NUM_SIZE; i++){   // Inverting if negative and converting to Zn
    if (carryR[i] != 0){
      Zn.real.neg = (carryR[i] < 0);
      break;
    }
    if (i == NUM_SIZE-1){
      Zn.real.neg = false;
    }
  }
  for (int i = 0; i < NUM_SIZE; i++){
    if (carryI[i] != 0){
      Zn.imag.neg = (carryI[i] < 0);
      break;
    }
    if (i == NUM_SIZE-1){
      Zn.imag.neg = false;
    }
  }
  for (int i = NUM_SIZE-1; i >= 0; i--){
    if (Zn.real.neg && carryR[i] > 0){      // no need to check for != 0, [0] is always < 0
      carryR[i] = -1*carryR[i];
      carryR[i-1] += 1;
      carryR[i] += INT_FULL;
    }
    Zn.real.digits[i] = labs(carryR[i]);
    if (Zn.imag.neg && carryI[i] > 0){
      carryI[i] = -1*carryI[i];
      carryI[i-1] += 1;
      carryI[i] += INT_FULL;
    }
    Zn.imag.digits[i] = labs(carryI[i]);
  }
  // check escaped: abs(z) >= 2
  //Zn.esc = Zn.real.ret_double()*Zn.real.ret_double() + Zn.imag.ret_double()*Zn.imag.ret_double() > 4;

  if (Zn.real.digits[0]+Zn.imag.digits[0]+(((INT2)Zn.real.digits[1]+Zn.imag.digits[1]) >> INT_BITS) >= CONST_2SQRT2){
    Zn.esc = true; // with 'Mittelungleichung'
  }
  else if (Zn.real.digits[0] < 1 && Zn.imag.digits[0] < 1){
    Zn.esc = false;
  }
  else{ // harsh approximation
    Zn.esc = (Zn.real.digits[0]*Zn.imag.digits[0] + (((INT2)Zn.real.digits[0]*Zn.imag.digits[1] +
            Zn.real.digits[1]*Zn.imag.digits[0]) >> INT_BITS) > 2);
  }
  return Zn;
}

CFPHPN CFPHPN::next_iter_ref(CFPHPN C){
  CFPHPN Z = *this;
  FPHPN Zr2 = Z.real * Z.real;
  FPHPN Zi2 = Z.imag * Z.imag;
  CFPHPN Zn;
  Zn.real = (Zr2-Zi2)+C.real;
  Zn.imag = (Z.real*Z.imag*FPHPN(2))+C.imag;
  Zn.esc = ((Zn.real*Zn.real + Zn.imag*Zn.imag).digits[0] >= 4);
  return Zn;
}

// --- Return Functions --- //


// === FUNCTIONS === //

int escapetime(CFPHPN C, CFPHPN D, int maxI){
  CFPHPN Z = D;
  for (int iter=0; iter<maxI; iter++){
    Z = Z.next_iteration(C);
    if (Z.esc){
      return iter;
    }
  }
  return maxI;
}

