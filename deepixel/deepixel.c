/* deepixel.c
 * deePixel lib - source file
 * by Robert Babin
 *
 * calculate the mandelbrot series escape for one pair of coordinates
 */

#include <stdio.h> // printf
#include <stdbool.h> // boolean variables
#include <stdlib.h> // calloc
#include <stdint.h> // int types
#include <math.h> // ceil

// ----- CONFIGURATION ----- //

#define NUM_SIZE 4    // length of the High Precision Number in subunits
#define CALC_SIZE 2   // number of additional subunits during calculation

// testing subunits
#define EXCLUDED

// ----- DEFINITIONS ----- //
#define INT uint32_t  // unsigned base type
#define INT2 uint64_t // unsigned buffer to store INT, twice as big as INT
#define INT2S int64_t // signed buffer, same size as INT2
#define INT_BITS 32u // number of bits of INT
#define INT_FULL 4294967295u // 2^32-1 -- 32 bits of 1 = 0x FFFF FFFF
#define INT_SIZE  4294967296u // 2^32   -- number of values of 32 bits = 0x 1 0000 0000

// ----- TYPES <LOCAL> ----- //

// High Precision Number - HPN
typedef struct HighPrecNumSt {
  bool neg;
  INT digits[NUM_SIZE];
} HighPrecNum; // High Precision Number

typedef INT2 HPNCarry [NUM_SIZE+CALC_SIZE]; // High Precision Number (unsigned) Carry

typedef INT2S HPNSignedCarry [NUM_SIZE+CALC_SIZE]; // High Precision Number Signed Carry

typedef struct ComplexNumSt { // Complex Number, consisting of 2 High Precision Numbers
  HighPrecNum real;
  HighPrecNum imag;
} ComplexNum;

// ----- LOCAL FUNCTIONS ----- //

char hpn_larger(HighPrecNum** s, HighPrecNum** l, HighPrecNum num1, char c1, HighPrecNum num2, char c2){
  // determine larger High Precision Number
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

HighPrecNum hpn_from_double(double number){
  HighPrecNum hpn;
  if (number == 0){
    hpn.neg = false;
    for (int i = 0; i < NUM_SIZE; i++){
      hpn.digits[i] = 0;
    }
    return hpn;
  }
  else if (number > 0){
    hpn.neg = false;
  }
  else {
    hpn.neg = true;
    number *= -1;
  }
  for (int i = 0; i < NUM_SIZE; i++){
    hpn.digits[i] = (INT) number;
    number -= hpn.digits[i];
    number *= INT_SIZE;
  }
  return hpn;
}

ComplexNum cplx_from_double(double r, double i){
  ComplexNum c = {hpn_from_double(r), hpn_from_double(i)};
  return c;
}

double hpn_to_double(HighPrecNum hpn){
  double d = 0;
  double a;
  for (int i = 0; i < NUM_SIZE; i++){
    a = hpn.digits[i];
    for (int ii = 0; ii < i; ii++){
      a /= INT_SIZE;
    }
    d += a;
  }
  if (hpn.neg){
    d *= -1;
  }
  return d;
}

void test_header(char* name, char* desc){
  printf("= TEST %s =\n",name);
  printf("%s\n",desc);
}

void test_hpn(const char* name, HighPrecNum hpn, double d){
  if (isnan(d)){
    d = hpn_to_double(hpn);
  }
  printf("%s: %d %2X.%8X.%8X = %+11.8f\n",name,hpn.neg,hpn.digits[0],hpn.digits[1],hpn.digits[2],d);
}

void test_end(){
  printf("=== TEST END ===\n");
}

#ifndef EXCLUDED
// ----- VISIBLE FUNCTIONS ----- //

int check_sizes(){
  // not neccessary anymore?
  if (sizeof(int) != 4 || sizeof(long) != 8){
    fprintf(stderr, "wrong integer sizes\nint: %ld/4\nlong: %ld/8\n",sizeof(int),sizeof(long));
    return(-1);
  }
  return(0);
}
#endif

// ----- CALCULATION & BASE FUNCTIONS ----- //

HighPrecNum hpn_add(HighPrecNum hpnA, HighPrecNum hpnB){
  HighPrecNum hpnC;
  // C = A + B
	if (hpnA.neg == hpnB.neg){
		// --- Addition --- //
		hpnC.neg = hpnA.neg;
    INT2 buffer = 0;
    for (int i=NUM_SIZE-1; i>=0; i--){
      buffer += hpnA.digits[i] + hpnB.digits[i];
      hpnC.digits[i] = buffer;
      buffer = buffer >> INT_BITS;
    }
	}
	else{
	  // --- Subtraction --- //
    HighPrecNum* hpnLptr;
    HighPrecNum* hpnSptr;
    char larger = hpn_larger(&hpnSptr, &hpnLptr, hpnA, 'A', hpnB, 'B');
    if (larger == '='){
      hpnC.neg = false;
      for (int i=0; i<NUM_SIZE; i++){
        hpnC.digits[i] = 0;
      }
      return hpnC;
    }
    hpnC.neg = hpnLptr->neg;
    INT2 buffer;
    INT2 carry = 0;
    for (int i=NUM_SIZE-1; i>=0; i--){
      buffer = carry + hpnSptr->digits[i];
      carry = (hpnLptr->digits[i] < buffer);
      hpnC.digits[i] = hpnLptr->digits[i] - buffer;
    }
  }
	return hpnC;
}

// --- TEST --- //
int test_hpn_add(double dA, double dB){
  test_header("hpn_add", "C = A + B");
  HighPrecNum hpnA = hpn_from_double(dA);
  HighPrecNum hpnB = hpn_from_double(dB);
  test_hpn("hpnA",hpnA,dA);
  test_hpn("hpnB",hpnB,dB);
  test_hpn("hpnC",hpn_add(hpnA,hpnB),NAN);
  printf("ref : %f\n",dA+dB);
  test_end();
  return 0;
}

HighPrecNum hpn_mult(HighPrecNum hpnA, HighPrecNum hpnB){
  HighPrecNum hpnC;
  // C = A * B
  hpnC.neg = hpnA.neg != hpnB.neg;
  INT2 buffer;
  INT2 carry0 = 0; // carry0 aligned with buffer
  INT2 carry1 = 0; // left shift INT_BITS from carry0
  for (int sum=NUM_SIZE+CALC_SIZE-1; sum >= 0; sum--){
    for (int i=0; i<=sum; i++){
      if (sum-i >= NUM_SIZE || i >= NUM_SIZE){
        continue;
      }
      buffer = hpnA.digits[i];
      buffer *= hpnB.digits[sum-i];
      carry0 += buffer & INT_FULL;   // bitwise AND -- lower INT
      carry1 += buffer >> INT_BITS;  // higher INT
    }
    if (sum < NUM_SIZE){
      hpnC.digits[sum] = carry0 & INT_FULL;
    }
    carry1 += carry0 >> INT_BITS;
    carry0 = carry1;
    carry1 = carry0 >> INT_BITS;
    carry0 = carry0 & INT_FULL;
  }
  return hpnC;
}

HighPrecNum hpn_mult_lc(HighPrecNum hpnA, HighPrecNum hpnB){
  HighPrecNum hpnC;
  // C = A * B
  hpnC.neg = hpnA.neg != hpnB.neg;
  INT2 buffer;
  HPNCarry carry = {0};
  for (int sum=NUM_SIZE+CALC_SIZE-1; sum >= 0; sum--){
    for (int i=0; i<=sum; i++){
      if (sum-i >= NUM_SIZE || i >= NUM_SIZE){
        continue;
      }
      buffer = hpnA.digits[i];
      buffer *= hpnB.digits[sum-i];
      if (sum != 0){
        carry[sum] += buffer & INT_FULL;   // bitwise AND -- lower INT
        carry[sum-1] += buffer >> INT_BITS;  // higher INT
      }
      else{
        carry[sum] += buffer;
      }
    }
    if (sum < NUM_SIZE){
      hpnC.digits[sum] = carry[sum] & INT_FULL;
    }
    if (sum != 0){
      carry[sum-1] += carry[sum] >> INT_BITS;
      carry[sum] = carry[sum] & INT_FULL;
    }
  }
  return hpnC;
}

// --- TEST --- //
int test_hpn_mult(double dA, double dB){
  test_header("hpn_mult & *_lc"," C = A * B");
  HighPrecNum hpnA = hpn_from_double(dA);
  HighPrecNum hpnB = hpn_from_double(dB);
  test_hpn("hpnA ",hpnA,dA);
  test_hpn("hpnB ",hpnB,dB);
  test_hpn("hpnC1",hpn_mult(hpnA,hpnB),NAN);
  test_hpn("hpnC2",hpn_mult_lc(hpnA,hpnB),NAN);
  printf("ref  : %f\n",dA*dB);
  test_end();
  return 0;
}

ComplexNum next_iteration(ComplexNum Z, ComplexNum C){
  INT2 bufferR, bufferI, bufferRI;
  HPNSignedCarry carryR = {0};
  HPNSignedCarry carryI = {0};
  bool x2;
  ComplexNum Zn;

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
          carryR[sum] += bufferR * x2;
          carryR[sum] -= bufferI * x2;
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
  for (int i = NUM_SIZE-1; i >= 0; i--){ // error below
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
  return Zn;
}

ComplexNum next_iteration_split(ComplexNum Z, ComplexNum C){
  HighPrecNum Zr2 = hpn_mult(Z.real,Z.real);
  HighPrecNum Zi2 = hpn_mult(Z.imag,Z.imag);
  Zi2.neg = true;
  ComplexNum Zn = {hpn_add(hpn_add(Zr2,Zi2),C.real),
                   hpn_add(hpn_mult(hpn_mult(Z.real,Z.imag),hpn_from_double(2)),C.imag)};
  return Zn;
}

// --- TEST --- //
int test_next_iteration(double Zr, double Zi, double Cr, double Ci){
  test_header("next_iteration"," Zn = Z^2 + C");
  ComplexNum Z = cplx_from_double(Zr,Zi);
  ComplexNum C = cplx_from_double(Cr,Ci);
  test_hpn("Z_r ",Z.real,Zr);
  test_hpn("Z_i ",Z.imag,Zi);
  test_hpn("C_r ",C.real,Cr);
  test_hpn("C_i ",C.imag,Ci);
  ComplexNum Zn = next_iteration(Z,C);
  test_hpn("Zn_r",Zn.real,NAN);
  test_hpn("Zn_i",Zn.imag,NAN);
  ComplexNum Zs = next_iteration_split(Z,C);
  test_hpn("Zs_r",Zs.real,NAN);
  test_hpn("Zs_i",Zs.imag,NAN);
  printf("ref : %f %f\n", Zr*Zr-Zi*Zi+Cr, 2*Zr*Zi+Ci);
  test_end();
  return 0;
}
