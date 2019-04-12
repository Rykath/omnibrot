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

// ----- CONSTANTS ----- //

#define INT uint32_t
#define INT2 uint64_t // buffer to store INT, twice as big as INT
#define INT_BITS 32 // number of bits of INT
#define INT_FULL 4294967295 // 2^32-1 -- 32 bits of 1 = 0x FFFF FFFF
#define INT_SIZE  4294967296 // 2^32   -- number of values of 32 bits = 0x 1 0000 0000

// ----- STRUCTS <LOCAL> ----- //

// High Precision Number - HPN
struct HighPrecNum {
  bool neg;
  INT* digits; // len: NUM_SIZE
};

struct HighPrecNumCarry {
  INT2* digits; // len: NUM_SIZE + CALC_SIZE
};

// Complex Number, consisting of 2 High Precision Numbers
struct ComplexNum {
  struct HighPrecNum real;
  struct HighPrecNum imag;
};

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


