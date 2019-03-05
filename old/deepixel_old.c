#include <stdio.h>
#include <math.h>

/* deePixel by Robert Babin
 * calculate the mandelbrot series escape for one pair of coordinates
 *
 * settings and coordinates are read from a config file
 */

/* assumptions:
 * int: 4 bytes - 32 bits
 * long: 8 bytes - 64 bits
 */

int main(int argc, char** argv){
  
  int length = 4;
  
  unsigned int* cr;
  bool cr_neg;
  unsigned int* ci;
  bool ci_neg;
  unsigned int* zr;
  bool zr_neg;
  unsigned int* zi;
  bool zi_neg;
  unsigned int* zr_old;
  bool zr_old_neg;
  unsigned int* zi_old;
  bool zi_old_neg;

  unsigned long buffer;
  bool buffer_neg;
  unsigned long carry; // offset to buffer: 16 bit
  bool carry_neg;

  cr = calloc(length, sizeof(int));
  ci = calloc(length, sizeof(int));
  zr = calloc(length, sizeof(int));
  zi = calloc(length, sizeof(int));
  zr_old = calloc(length, sizeof(int));
  zi_old = calloc(length, sizeof(int));

  // TEST
  if (cr == NULL or ci == NULL or zr == NULL or zi == NULL){
    fprintf(stderr, "allocation (calloc) of cr/ci/zr/zi failed\n");
    return(-1);
  }
  if (sizeof(int) != 4 or sizeof(long) != 8){
    fprintf(stderr, "wrong integer sizes\nint: %ld/4\nlong: %ld/8\n",sizeof(int),sizeof(long));
    return(-1);
  }

  // CALCULATION
  // z(n+1) = z(n)^2 + c
  /* DOES NOT WORK
   * zr,zi,zr_sq,zi_sq
   * zi = 2 * zr * zi + ci
   * zr = zr_sq - zi_sq + cr
   * zr_sq = zr^2
   * zi_sq = zi^2
   */
  
  // zi
  // zi = 2*zr*zi + ci
  zi_neg = zr_old_neg != zi_old_neg;
  for (int l=length-1; l > 0; l--){
    carry = 0
    if (l != length-1){
      zi[l+1] = 0
    }
    for (int r=0; r < l; r++){
      // zr * zi
      buffer = zr_old[r] * zi_old[l-r];
      carry += buffer >> 16; // first 16 bits
      if (l != length-1){
        zi[l+1] += buffer & 65535; // last 16 bits
      }
    }
    if (l != length-1){
      carry += zi[l+1] >> 16;
      zi[l+1] = zi[l+1] & 65535;
      zi[l+1] += (carry & ~65535) << 16 & 4294967295;
    }
  }
  return(0);
}
