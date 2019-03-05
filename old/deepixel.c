#include <stdio.h>
#include <math.h>

/* deePixel by Robert Babin
 * calculate the mandelbrot series escape for one pair of coordinates
 *
 * settings and coordinates are read from a config file
 */

#define NUM_SIZE 4
#define CALC_PLUS 2
#define BIN32 4294967295
#define BIN33 4294967296

struct HighPrecNum {
  bool neg;
  unsigned int* digits;
};

struct ComplexNum {
  HighPrecNum real;
  HighPrecNum imag;
};

int calcIter(ComplexNum posC, ComplexNum* ptrZ, ComplexNum* ptrZo){
  // variables
  ComplexNum posC;
  ComplexNum posZ = *ptrZ;
  ComplexNum posZo = *ptrZ;

  unsigned long buffer;
  unsigned long carry_s;
  unsigned long carry_l;
  ComplexNum* pos_s;
  ComplexNum* pos_l;
  char bigger;

  // TEST SIZES
  if (sizeof(int) != 4 or sizeof(long) != 8){
    fprintf(stderr, "wrong integer sizes\nint: %ld/4\nlong: %ld/8\n",sizeof(int),sizeof(long));
    return(-1);
  }
  
  // ZI
  // zi = 2*zr*zi + ci
  posZ.imag.neg = (posZo.imag.neg != posZo.real.neg);
  carry_l = 0;
  carry_s = 0;
  for (int zi_pos = NUM_SIZE+CALC_PLUS-1; zi_pos >= 0; zi_pos--){
    for (int zi_old_pos = 0; zi_old_pos <= zi_pos; zi_old_pos++){
      // 2*zr*zi
      buffer = posZo.imag.digits[zi_old_pos] * posZo.real.digits[zi_pos-zi_old_pos];
      carry_l += (buffer >> 32) << 1;
      carry_s += (buffer & BIN32) << 1;
    }
    if (zi_pos < NUM_SIZE){
      // +ci
      if (posZ.imag.neg == posC.imag.neg){
        carry_s += posC.imag.digits[zi_pos];
      }
    }
    carry_l += carry_s >> 32;
    if (zi_pos < NUM_SIZE){
      posZ.imag.digits[zi_pos] = carry_s;
    }
    carry_s = carry_l;
    carry_l = 0;
  }
  if (posZ.imag.neg != posC.imag.neg){
    bigger = '='; // the bigger variable
    for (int zi_pos = 0; zi_pos <= NUM_SIZE-1; zi_pos++){
      if (posZ.imag.digits[zi_pos] < posC.imag.digits[zi_pos]){
        bigger = 'C';
        break;
      }
      else if (posZ.imag.digits[zi_pos] > posC.imag.digits[zi_pos]){
        bigger = 'Z';
        break;
      }
    }
    // -ci
    if (bigger == '='){
      for (int zi_pos = 1; zi_pos <= NUM_SIZE; zi_pos++){
        posZ.imag.digits[zi_pos-1] = 0;
      }
    }
    else if (bigger == 'Z'){ // Z > C
      carry_l = 0;
      carry_s = 0;
      for (int zi_pos = NUM_SIZE; zi_pos > 0; zi_pos--){
        carry_s += posC.imag.digits[zi_pos-1];
	carry_l = carry_s >> 32;
	carry_s = carry_s & BIN32;
        if (posZ.imag.digits[zi_pos-1] < carry_s){
          carry_l += 1;
	}
        posZ.imag.digits[zi_pos-1] -= carry_s;
	carry_s = carry_l;
      }
    }
    else if (bigger == 'C'){
      posZ.imag.neg = !posZ.imag.neg;
      carry_l = 0;
      carry_s = 0;
      for (int zi_pos = NUM_SIZE; zi_pos > 0; zi_pos--){
        carry_s += posZ.imag.digits[zi_pos-1];
	carry_l = carry_s >> 32;
	carry_s = carry_s & BIN32;
	if (posC.imag.digits[zi_pos-1] < carry_s){
	  carry_l += 1;
	}
	posZ.imag.digits[zi_pos-1] = posC.imag.digits[zi_pos-1] - carry_s;
	carry_s = carry_l;
      }
    }
  }
  // ZR
  // zr = zr^2 - zi^2 + cr
  bigger = '=';
  // check if zr or zi are bigger
  for (int zr_pos = 0; zr_pos <= NUM_SIZE-1; zr_pos++){
    if (posZo.real.digits[zr_pos] < posZo.imag.digits[zr_pos]){
      bigger = 'I';
      break;
    }
    else if (posZo.real.digits[zr_pos] > posZo.imag.digits[zr_pos]){
      bigger = 'R';
      break;
    }
  }
  // if zr_old == zi_old -> zr_new = cr
  if (bigger == '='){
    for (int zr_pos = 0; zr_pos <= NUM_SIZE-1; zr_pos++){
      posZ.real.digits[zr_pos] = posC.real.digits[zr_pos];
      posZ.real.neg = posC.real.neg;
    }
  }
  else{
    // make a pointer to the larger and smaller number and determine resulting sign
    if (bigger == 'R'){
      pos_l = &(posZo.real);
      pos_s = &(posZo.imag);
      posZ.real.neg = false;
    }
    else {
      pos_l = &(posZo.imag);
      pos_s = &(posZo.real);
      posZ.real.neg = true;
    }
    for (int zr_pos = NUM_SIZE+CALC_PLUS-1; zr_pos >= 0; zr_pos--){
      for (int zr_old_pos = 0; zr_old_pos <= zr_pos/2; zr_pos++){
        buffer = pos_l.digits[zr_old_pos]*pos_l.digits[zr_pos-zr_old_pos];
	if (2*zr_old_pos == zr_pos){
	  carry_l += (buffer >> 32);
	  carry_s += (buffer & BIN32);
	}
	else { // x2
          carry_l += (buffer >> 32) << 1;
	  carry_s += (buffer & BIN32) << 1;
	}
	// cr has same sign as zr^2-zi^2
	if (zr_pos < NUM_SIZE && posZ.real.neg == posC.real.neg){ // +cr
	  carry_s += posC.real.digits[zr_pos];
	}
      }
      carry_l += carry_s >> 32;
      if (zr_pos < NUM_SIZE){
        posZ.real.digits[zr_pos] = carry_s;
      }
      carry_s = carry_l;
      carry_l = 0;
    }
    // cr has opposite sign as zr^2-zi^2
    if (posZ.real.neg != posC.real.neg){ // -cr
      // determine bigger number and resulting sign
      bigger = '=';
      for (int zr_pos = 0; zr_pos <= NUM_SIZE-1; zr_pos++){
        if (posZ.real.digits[zr_pos] < posC.real.digits[zr_pos]){
	  bigger = 'C';
	  break;
	}
	else if (posZ.real.digits[zr_pos] > posC.real.digits[zr_pos]){
	  bigger = 'Z';
	  break;
	}
      }
      if (bigger == '='){
        for (int zr_pos = 0; zr_pos <= NUM_SIZE-1; zr_pos++){
	  posZ.real.digits[zr_pos] = 0;
	}
	posZ.real.neg = false;
      }
    }
  }
  return(0);
}
