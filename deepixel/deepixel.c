/* deepixel.c
 * deePixel lib - source file
 * by Robert Babin
 *
 * calculate the mandelbrot series escape for one pair of coordinates
 */

#include <stdio.h> // printf
#include <stdbool.h> // boolean variables
#include <stdlib.h> // calloc

// ----- CONFIGURATION ----- //

// length of the High Precision Number in subunits
#define NUM_SIZE 4
// number of additional subunits during calculation
#define CALC_SIZE_ADD 2

// ----- CONSTANTS ----- //

#define BIN32 4294967295 // 2^32-1
#define BIN33 4294967296 // 2^32

// ----- STRUCTS <LOCAL> ----- //

// High Precision Number - HPN
struct HighPrecNum {
  bool neg;
  unsigned int* digits;
};

// Complex Number, consisting of 2 High Precision Numbers
struct ComplexNum {
  struct HighPrecNum real;
  struct HighPrecNum imag;
};

// ----- LOCAL FUNCTIONS ----- //

struct HighPrecNum hpn_new(){
  struct HighPrecNum hpn;
  hpn.neg = false;
  hpn.digits = (unsigned int*) calloc(NUM_SIZE, sizeof(unsigned int));
  return hpn;
}

char hpn_larger(struct HighPrecNum* s, struct HighPrecNum* l, struct HighPrecNum num1, char c1, struct HighPrecNum num2, char c2){
  // determine larger High Precision Number
  // based on absolute values (.digits), ignoring sign (.neg)
  for (int i = 0; i < NUM_SIZE; i++){
    if (num1.digits[i] > num2.digits[i]){
      *s = num2;
      *l = num1;
      return c1;
    }
    else if (num1.digits[i] < num2.digits[i]){
      *s = num1;
      *l = num2;
      return c2;
    }
    // default: s = num1, l = num2
    *s = num1;
    *l = num2;
    return '=';
  }
  return '=';
}

void hpn_from_double(struct HighPrecNum hpn, double number){
  printf("%f \n",number);
  if (number == 0){
    hpn.neg = false;
    for (int i = 0; i < NUM_SIZE; i++){
      hpn.digits[i] = 0;
    }
    return;
  }
  else if (number > 0){
    hpn.neg = false;
  }
  else {
    printf("neg \n");
    hpn.neg = true;
    printf("%d \n",hpn.neg);
    number *= -1;
  }
  for (int i = 0; i < NUM_SIZE; i++){
    hpn.digits[i] = (unsigned int) number;
    number -= hpn.digits[i];
    number *= BIN33;
  }
}

// ----- VISIBLE FUNCTIONS ----- //

int check_sizes(){
  if (sizeof(int) != 4 || sizeof(long) != 8){
    fprintf(stderr, "wrong integer sizes\nint: %ld/4\nlong: %ld/8\n",sizeof(int),sizeof(long));
    return(-1);
  }
  return(0);
}

void next_z_imag(struct ComplexNum posZo, struct ComplexNum posZ, struct ComplexNum posC){
  // posZo = old Z
  // posZ = preallocated slot for new Z - members of posZ.imag are modified
  // posC = C
  unsigned long buffer;
  unsigned long carry_s; // small carry - 64 bit long, usually filled 32 bit, same position as buffer
  unsigned long carry_l; // large carry - 64 bit long, usually filled 32 bit, shifted 32 bits left from buffer
  struct HighPrecNum* ptr_s = 0; // pointer to smaller number
  struct HighPrecNum* ptr_l = 0; // pointer to larger number
  char larger;

  // Z.i = 2 * Zo.r * Zo.i + C.i

  // --- I --- //
  // Z.i = 2 * Zo.r * Zo. i
  posZ.imag.neg = (posZo.imag.neg != posZo.real.neg);
  carry_l = 0;
  carry_s = 0;
  for (int p = NUM_SIZE+CALC_SIZE_ADD-1; p >= 0; p--){ // looping through result from behind
    for (int i = 0; i <= p; i++){ // loop through all possible inputs
      if (i >= NUM_SIZE || p-i >= NUM_SIZE){
        continue;
      }
      buffer = posZo.real.digits[i] * posZo.imag.digits[p-i];
      carry_l += (buffer >> 32) << 1;
      carry_s += (buffer & BIN32) << 1;
    }
    // --- II.a --- //
    // if Z.i and C.i have same sign
    // Z.i += C.i
    if (posZ.imag.neg == posC.imag.neg && p < NUM_SIZE){
      carry_s += posC.imag.digits[p];
    }
    // --- I cont. --- //
    carry_l += carry_s >> 32; // add overflow of carry_s to carry_l
    if (p < NUM_SIZE){
      posZ.imag.digits[p] = carry_s; // only last 32 bits are copied anyway
    }
    carry_s = carry_l;
    carry_l = 0;
  }
  // --- II.b --- //
  // if Z.i and C.i have opposite sign
  // Z.i += C.i
  if (posZ.imag.neg != posC.imag.neg){
    larger = hpn_larger(ptr_s, ptr_l, posZ.imag, 'Z', posC.imag, 'C');
    // if Z.i == C.i
    if (larger == '='){
      for (int p = 0; p < NUM_SIZE; p++){
        posZ.imag.digits[p] = 0;
      }
      posZ.imag.neg = false;
    }
    else {
      carry_l = 0;
      carry_s = 0;
      for (int p = NUM_SIZE-1; p >= 0; p--){ // looping from behind
        carry_s += ptr_s->digits[p];
	carry_l = carry_s >> 32;
	carry_s = carry_s & BIN32;
	if (ptr_l->digits[p] < carry_s){
	  carry_l += 1;
	}
	posZ.imag.digits[p] = ptr_l->digits[p] - carry_s;
	carry_s = carry_l;
      }
    }
    if (larger == 'C'){
      posZ.imag.neg = !posZ.imag.neg;
    }
  }
}

void next_z_real(struct ComplexNum posZo, struct ComplexNum posZ, struct ComplexNum posC){
  // posZo = old Z
  // posZ = preallocated slot for new Z - members of posZ.real are modified
  // posC = C
  unsigned long buffer;
  unsigned long carry_s; // small carry
  unsigned long carry_l; // large carry
  struct HighPrecNum* ptr_s = 0; // pointer to smaller number
  struct HighPrecNum* ptr_l = 0;; // pointer to larger number
  char larger;

  // Z.r = Zo.r^2 - Zo.i^2 + C.r

  // --- I --- //
  // Z.r = Zo.larger^2
  larger = hpn_larger(ptr_s, ptr_l, posZo.real, 'r', posZo.imag, 'i');
  if (larger == '='){
    // --- II.a --- //
    // Z.r = C.r
    for (int p = 0; p < NUM_SIZE; p++){
      posZ.real.digits[p] = posC.real.digits[p];
    }
    posZ.real.neg = posC.real.neg;
    // --- I cont. --- //
  }
  else {
    if (larger == 'r'){
      posZ.real.neg = false;
    }
    else {
      posZ.real.neg = true;
    }
    carry_s = 0;
    carry_l = 0;
    for (int p = NUM_SIZE+CALC_SIZE_ADD-1; p >= 0; p--){
      // larger^2
      for (int i = 0; i <= p/2; i++){
        if (i >= NUM_SIZE || p-i >= NUM_SIZE){
	  continue;
	}
	buffer = ptr_l->digits[i] * ptr_l->digits[p-i];
        if (2*i == p){
	  carry_l += buffer >> 32;
          carry_s += buffer & BIN32;
	}
	else { // *2
	  carry_l += (buffer >> 32) << 1;
	  carry_s += (buffer & BIN32) << 1;
	}
      }
      // --- II.b --- //
      // C.r has same sign as larger
      if (posC.real.neg == ptr_l->neg && p < NUM_SIZE){
        carry_s += posC.real.digits[p];
      }
      // --- I cont. --- //
      carry_l += carry_s >> 32;
      posZ.real.digits[p] = carry_s;
      carry_s = carry_l;
    }
    // --- III --- //
    // Z.r -= Zo.smaller^2
    carry_s = 0;
    carry_l = 0;
    for (int p = NUM_SIZE+CALC_SIZE_ADD-1; p >= 0; p--){
      for (int i = 0; i <= p/2; i++){
        if (i >= NUM_SIZE || p-i >= NUM_SIZE){
	  continue;
	}
	buffer = ptr_s->digits[i] * ptr_s->digits[p-i];
	if (2*i == p){
	  carry_l += buffer >> 32;
	  carry_s += buffer & BIN32;
	}
	else { // *2
	  carry_l += (buffer >> 32) << 1;
	  carry_s += (buffer & BIN32) << 1;
	}
      }
      // --- II.c --- //
      // C.r has same sign as smaller
      if (posC.real.neg == ptr_s->neg && p < NUM_SIZE){
        carry_s += posC.real.digits[p];
      }
      // --- III cont. --- //
      carry_l += carry_s >> 32;
      carry_s = carry_s & BIN32;
      if (posZ.real.digits[p] < carry_s){
        carry_l += 1;
      }
      posZ.real.digits[p] -= carry_s;
      carry_s = carry_l;
      carry_l = 0;
    }
    if (carry_s != 0){ // Zo.smaller^2 + C.r > Zo.larger^2
      posZ.real.neg = !posZ.real.neg; // switch sign
      buffer = 1;
      for (int p = NUM_SIZE-1; p >= 0; p--){
        buffer += ~posZ.real.digits[p];
        posZ.real.digits[p] = buffer; // clipping to 32 bits
	buffer = buffer >> 32;
      }
    }
  }
}

int calc_escape_iteration(int max_iteration, double startCr, double endCr, int lenCr, double startCi, double endCi, int lenCi, int cr, int ci){
  int iteration = 0;
  struct HighPrecNum posCr = hpn_new();
  hpn_from_double(posCr, startCr + cr*(endCr-startCr)/lenCr);
  struct HighPrecNum posCi = hpn_new();
  hpn_from_double(posCi, startCi + ci*(endCi-startCi)/lenCi);
  printf("%d %u %u - %d %u %u \n", posCr.neg, posCr.digits[0], posCr.digits[1], posCi.neg, posCi.digits[0], posCi.digits[1]);
  if (posCr.neg == true){
    printf("ok \n");
  }
  return iteration;
}
