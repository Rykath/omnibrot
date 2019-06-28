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

// length of the High Precision Number in subunits
#define NUM_SIZE 4
// number of additional subunits during calculation
#define CALC_SIZE 2

// testing subunits
// std: PART_* defined
//      CHECK_* not defined
#define PART_IMAG_I
#define PART_IMAG_II
#define PART_REAL_I
#define PART_REAL_II
#define PART_REAL_III
#define CHECK_SEGFAULT
#define EXCLUDED

// ----- CONSTANTS ----- //
#define INT uint32_t  // unsigned base type
#define INT2 uint64_t // unsigned buffer to store INT, twice as big as INT
#define INT2S int64_t // signed buffer, same size as INT2
#define INT_BITS 32 // number of bits of INT
#define INT_FULL 4294967295 // 2^32-1 -- 32 bits of 1 = 0x FFFF FFFF
#define INT_SIZE  4294967296 // 2^32   -- number of values of 32 bits = 0x 1 0000 0000

// ----- STRUCTS <LOCAL> ----- //

// High Precision Number - HPN
struct HighPrecNum {
  bool neg;
  INT digits [NUM_SIZE];
};

struct HighPrecNumCarry {
  INT2* digits; // len: NUM_SIZE + CALC_SIZE
};

struct HPNSignedCarry { // High Precision Number Signed Carry
  INT2S digits [NUM_SIZE+CALC_SIZE];
};

// Complex Number, consisting of 2 High Precision Numbers
typedef struct ComplexNum {
  struct HighPrecNum real;
  struct HighPrecNum imag;
} ComplexNum;

// ----- LOCAL FUNCTIONS ----- //

void hpn_new(struct HighPrecNum* hpn){
  hpn->neg = false;
  hpn->digits = (INT*) calloc(NUM_SIZE, sizeof(INT));
  // <PERFORMANCE> calloc might be a little bit slower but initializes the memory
}

void hpnc_new(struct HighPrecNumCarry* hpnc){
  hpnc->digits = (INT2*) calloc(NUM_SIZE+CALC_SIZE,sizeof(INT2));
  // <PERFORMANCE> calloc might be a little bit slower but initializes the memory, init to 0 is important here! carry just adds to the value, no init to 0!
}

char hpn_larger(struct HighPrecNum** s, struct HighPrecNum** l, struct HighPrecNum num1, char c1, struct HighPrecNum num2, char c2){
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

void hpn_from_double(struct HighPrecNum* hpnptr, double number){
  if (number == 0){
    hpnptr->neg = false;
    for (int i = 0; i < NUM_SIZE; i++){
      hpnptr->digits[i] = 0;
    }
    return;
  }
  else if (number > 0){
    hpnptr->neg = false;
  }
  else {
    hpnptr->neg = true;
    number *= -1;
  }
  for (int i = 0; i < NUM_SIZE; i++){
    hpnptr->digits[i] = (INT) number;
    number -= hpnptr->digits[i];
    number *= INT_SIZE;
  }
}

// ----- VISIBLE FUNCTIONS ----- //

int check_sizes(){
  // not neccessary anymore?
  if (sizeof(int) != 4 || sizeof(long) != 8){
    fprintf(stderr, "wrong integer sizes\nint: %ld/4\nlong: %ld/8\n",sizeof(int),sizeof(long));
    return(-1);
  }
  return(0);
}

// === RESTART - Jun 2019 === //

struct ComplexNum next_iteration(struct ComplexNum Z, struct ComplexNum C){
  INT2 bufferR, bufferI, bufferRI;
  struct HPNSignedCarry cR, cI;
  bool negRI = Z.real.neg ^ Z.imag.neg;
  
  for (int sum = NUM_SIZE+CALC_SIZE-1; sum >= 0; sum--){        //  loop carry
    for (int i = 0; i <= sum; i++){                             //    loop multiplication
      if (sum-i_ >= NUM_SIZE || i >= NUM_SIZE){ continue; }     //      catch borders
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
        if (negRI){ carryI[sum] -= bufferRI << 1;}
        else {      carryI[sum] += bufferRI << 1;}
      }
      else {
        if (negRI){
          carryI[sum] -= (bufferRI & INT_FULL) << 1;
          carryI[sum-1] -= (bufferRI >> INTBITS) << 1;
        }
        else {
          carryI[sum] += (bufferRI & INT_FULL) << 1;
          carryI[sum-1] += (bufferRI >> INTBITS) << 1;
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
    printf("sum: %s: %+16X %+16X\n",sum,carryR[sum],carryI[sum]);
  }
  return Z;
}

int test_next_iteration(double Zr, double Zi, double Cr, double Ci){
  // format assuming INT_SIZE = 32 bits
  printf("TEST next_iteration\n");
  struct ComplexNum Z, C;
  hpn_from_double(&Z.real,Zr);
  hpn_from_double(&Z.imag,Zi);
  hpn_from_double(&C.real,Cr);
  hpn_from_double(&C.imag,Ci);
  next_iteration(Z,C);
  printf("TEST END\n");
  return 0;
}

// === RESTART - END === //

// --- CALCULATION --- //

void hpn_add(struct HighPrecNum hpnA, struct HighPrecNum hpnB, struct HighPrecNum* hpnCptr){
  // C = A + B
	if (hpnA.neg == hpnB.neg){
		// --- Addition --- //
		hpnCptr->neg = hpnA.neg;
    INT2 buffer = 0;
    for (int i=NUM_SIZE-1; i>=0; i--){
      buffer += hpnA.digits[i] + hpnB.digits[i];
      hpnCptr->digits[i] = buffer;
      buffer = buffer >> INT_BITS;
    }
	}
	else{
	  // --- Subtraction --- //
    struct HighPrecNum* hpnLptr;
    struct HighPrecNum* hpnSptr;
    char larger = hpn_larger(&hpnSptr, &hpnLptr, hpnA, 'A', hpnB, 'B');
    if (larger == '='){
      hpnCptr->neg = false;
      for (int i=0; i<NUM_SIZE; i++){
        hpnCptr->digits[i] = 0;
      }
      return;
    }
    hpnCptr->neg = hpnLptr->neg;
    INT2 buffer;
    INT2 carry = 0;
    for (int i=NUM_SIZE-1; i>=0; i--){
      buffer = carry + hpnSptr->digits[i];
      carry = (hpnLptr->digits[i] < buffer);
      hpnCptr->digits[i] = hpnLptr->digits[i] - buffer;
    }
  }
}

int test_hpn_add(double dA, double dB){
  // format assuming INT_SIZE = 32 bits
  printf("TEST hpn_add\n");
  printf(" C = A + B\n");
  struct HighPrecNum hpnA;
  struct HighPrecNum hpnB;
  struct HighPrecNum hpnC;
  double dC = dA + dB;
  hpn_new(&hpnA);
  hpn_new(&hpnB);
  hpn_new(&hpnC);
  hpn_from_double(&hpnA,dA);
  hpn_from_double(&hpnB,dB);
  printf(" dA : %f\n",dA);
  printf("hpnA: %d %2X.%8X.%8X\n",hpnA.neg,hpnA.digits[0],hpnA.digits[1],hpnA.digits[2]);
  printf(" dB : %f\n",dB);
  printf("hpnB: %d %2X.%8X.%8X\n",hpnB.neg,hpnB.digits[0],hpnB.digits[1],hpnB.digits[2]);
  printf(" dC : %f\n",dC);
  hpn_add(hpnA,hpnB,&hpnC);
  printf("hpnC: %d %2X.%8X.%8X\n",hpnC.neg,hpnC.digits[0],hpnC.digits[1],hpnC.digits[2]);
  printf("TEST END\n");
  return 0;
}

void hpn_mult(struct HighPrecNum hpnA, struct HighPrecNum hpnB, struct HighPrecNum* hpnCptr){
  // C = A * B
  hpnCptr->neg = hpnA.neg != hpnB.neg;
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
      hpnCptr->digits[sum] = carry0 & INT_FULL;
    }
    carry1 += carry0 >> INT_BITS;
    carry0 = carry1;
    carry1 = carry0 >> INT_BITS;
    carry0 = carry0 & INT_FULL;
  }
}

void hpn_mult_lc(struct HighPrecNum hpnA, struct HighPrecNum hpnB, struct HighPrecNum* hpnCptr){
  // C = A * B
  hpnCptr->neg = hpnA.neg != hpnB.neg;
  INT2 buffer;
  struct HighPrecNumCarry hpncarry;
  hpnc_new(&hpncarry);
  INT2* carry = hpncarry.digits;
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
      hpnCptr->digits[sum] = carry[sum] & INT_FULL;
    }
    if (sum != 0){
      carry[sum-1] += carry[sum] >> INT_BITS;
      carry[sum] = carry[sum] & INT_FULL;
    }
  }
}

int test_hpn_mult(double dA, double dB){
  // format assuming INT_SIZE - 32 bits
  printf("TEST hpn_mult >> C = A * B\n");
  struct HighPrecNum hpnA;
  struct HighPrecNum hpnB;
  struct HighPrecNum hpnC;
  double dC = dA * dB;
  hpn_new(&hpnA);
  hpn_new(&hpnB);
  hpn_new(&hpnC);
  hpn_from_double(&hpnA,dA);
  hpn_from_double(&hpnB,dB);
  printf(" A | %11.8f | %d %2X.%8X.%8X\n",dA,hpnA.neg,hpnA.digits[0],hpnA.digits[1],hpnA.digits[2]);
  printf(" B | %11.8f | %d %2X.%8X.%8X\n",dB,hpnB.neg,hpnB.digits[0],hpnB.digits[1],hpnB.digits[2]);
  hpn_mult(hpnA,hpnB,&hpnC);
  printf(" C | %11.8f | %d %2X.%8X.%8X\n",dC,hpnC.neg,hpnC.digits[0],hpnC.digits[1],hpnC.digits[2]);
  hpn_mult_lc(hpnA,hpnB,&hpnC);
  printf("Clc| %11.8f | %d %2X.%8X.%8X\n",dC,hpnC.neg,hpnC.digits[0],hpnC.digits[1],hpnC.digits[2]);
  printf("TEST END\n");
  return 0;
}

#ifndef EXCLUDED
bool hpn_square1(int sum, struct HighPrecNum hpnA, struct HighPrecNumCarry hpncarry*, bool subtract){
  INT2 buffer;
  bool x2;
  for (int i=0; i<=sum/2; i++){
    if (sum-i) >= NUM_SIZE || i >= NUM_SIZE){
      continue;
    }
    buffer = hpnA.digits[i];
    buffer *= hpnA.digits[sum-i];
    x2 = (2*i != sum);
    if (sum == 0){
      carry[sum] += buffer << x2;
    }
    else{
      
      carry[sum] += (buffer & INT_FULL) << x2;
      carry[sum-1] += (buffer >> INT_BITS) << x2;
    }
  }
}

void next_cr(struct ComplexNum C, struct ComplexNum Z, struct ComplexNum Zn){
  // Zni = Zr^2 - Zi^2 + Ci
  struct HighPrecNumCarry carry;
  for (int sum=NUM_SIZE+CALC_SIZE-1; sum >= 0; sum--){
    
  }
}

int next_iter(struct ComplexNum C, struct ComplexNum Z, struct ComplexNum Zn){
  //= ALGORITHM =//
  // if zr2 + zi2 > 4: retval = 1 // approximation: just looking at the first 2 digits each
  // znr = zr2 - zi2 + cr
  // zni = 2*zr*zi + ci
  //= INIT =//
  retval = 0;
  struct HighPrecNumCarry hpnc_zr2;
  hpnc_new(&hpnc_zr2);
  INT2* zr2 = hpnc_zr2.digits;
  struct HighPrecNumCarry hpnc_zi2;
  hpnc_new(&hpnc_zi2);
  INT2* zi2 = hpnc_zi2.digits;
  INT2 buffer;
  INT2 buffer2;
  bool shift;
  //= CALC =//
  // ZR2, ZI2
  for (int sum=NUM_SIZE+CALC_SIZE-1; sum >= 0; sum--){
    for (int i=0; i<=ceil(sum/2); i++){
      if (sum-i >= NUM_SIZE || i >= NUM_SIZE){
        continue;
      }
      buffer = Z->real.digits[i];
      buffer *= Z->real.digits[sum-i];
      buffer2 = Z->imag.digits[i];
      buffer2 *= Z->imag.digits[sum-i];
      shift = (sum-i != i);
      if (sum != 0){
        zr2[sum] += (buffer & INT_FULL) << shift;   // bitwise AND -- lower INT
        zr2[sum-1] += (buffer >> INT_BITS) << shift;  // higher INT
        zi2[sum] += (buffer2 & INT_FULL) << shfit;
        zi2[sum-1] += (buffer2 >> INT_BITS) << shfit;
      }
      else{
        zr2[sum] += buffer << shift;
        zi2[sum] += buffer2 << shift;
      }
    }
    if (sum != 0){
      zr2[sum-1] += zr2[sum] >> INT_BITS;
      zr2[sum] = zr2[sum] & INT_FULL;
      zi2[sum-1] += zi2[sum] >> INT_BITS;
      zi2[sum] = zi2[sum] & INT_FULL;
    }
  }
  // ZR2 + ZI2 > 4
  retval = (zr2[0]+zi2[0] + (zr2[1]+zi2[1]) >> INT_BITS > 4);
  // TODO
  // TODO
}
#endif
