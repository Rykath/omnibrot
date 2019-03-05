#include <stdio.h>
#include <math.h>

/* deePixel by Robert Babin
 * calculate the mandelbrot series escape for one pair of coordinates
 *
 * settings and coordinates are read from a config file
 */

#define NUM_SIZE 4
#define BIN32 4294967295
#define BIN33 4294967296

struct HighPrecNum {
  bool neg;
  unsigned int* digits;
  unsigned short int head;
};

struct ComplexNum {
  HighPrecNum* real;
  HighPrecNum* imag;
};

int main(int argc, char** argv){
  // variables
  ComplexNum posC;
  ComplexNum posZ;
  ComplexNum posZo; // Z_old

  unsigned long buffer;
  //bool buffer_neg; always same
  unsigned long carry_s;
  //bool carry_s_neg; unused?
  unsigned long carry_l;
  bool carry_l_neg;


  posC.real.digits = calloc(length, sizeof(int));
  posC.imag.digits = calloc(length, sizeof(int));
  posZ.real.digits = calloc(length, sizeof(int));
  posZ.imag.digits = calloc(length, sizeof(int));
  posZo.real.digits = calloc(length, sizeof(int));
  posZo.imag.digits = calloc(length, sizeof(int));

  // TEST
  if (sizeof(int) != 4 or sizeof(long) != 8){
    fprintf(stderr, "wrong integer sizes\nint: %ld/4\nlong: %ld/8\n",sizeof(int),sizeof(long));
    return(-1);
  }

  // zi
  posZ.imag.neg = (posZo.imag.neg != posZo.real.neg);
  carry_l = 0;
  carry_s = 0;
  carry_l_neg = false;
  for (int zi_pos = NUM_SIZE+2; zi_pos > 0; zi_pos--){
    for (int zi_old_pos = 1; zi_old_pos < zi_pos; zi_old_pos++){
      // 2*zr*zi
      buffer = posZo.imag.digits[zi_old_pos-1] * posZo.real.digits[zi_pos-zi_old_pos-1];
      carry_l += (buffer >> 32) << 1;
      carry_s += (buffer & BIN32) << 1;
    }
    if (carry_l_neg){ // remainder from last subtraction
      if (carry_s > 0){
        carry_s -= 1;
      }
      else{
        carry_s == BIN32; // = BIN33-1
	if (carry_l > 0){
	  carry_l -= 1;
        }
	else{
	  carry_l_neg = true; // carry_l = -1
	}
      }
    }
    if (zi_pos <= NUM_SIZE){
      // +ci
      if (posZ.imag.neg == posC.imag.neg){
        carry_s += posC.imag.digits[zi_pos-1];
      }
      else{
        if (carry_s >= posC.imag.digits[zi_pos-1]){
          carry_s -= posC.imag.digits[zi_pos-1];
	}
	else{
          carry_s += BIN33 - posC.imag.digits[zi_pos-1];
	  if (carry_l > 0){
	    carry_l -= 1;
	  }
	  else{
	    carry_l_neg = true; // carry_l = -1
	  }
	}
      }
    }
    carry_l += carry_s >> 32;
    if (zi_pos <= NUM_SIZE){
      posZ.imag.digits[zi_pos-1] = carry_s;
    }
    carry_s = carry_l;
    carry_l = 0;
  } // head
  posZ.imag.head = 2 * posZo.imag.head * posZ.real.head + carry_s;
  if (posZ.imag.neg == posC.imag.neg){
    posZ.imag.head += posC.imag.head;
  }
  else{
    if (posZ.imag.head >= 

  return(0);
}
