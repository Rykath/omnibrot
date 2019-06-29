/* deepixel/testing/basics.c
 * test for deePixel
 * by Robert Babin
 */

#include <stdio.h>

#include "../deepixel.h"

#define EXCLUDED

int main(){
  double a, b;
  #ifndef EXCLUDED
  // === HighPrecisionNumbers === //
  // --- Addition & Subtraction --- //
  printf("HPN - Addition & Subtraction - 8 Tests \n");
  a = 1.3255037518;
  b = 1.2550952765;
  printf("T1-add  : %d\n",test_hpn_add(a,b));
  printf("T2-addR : %d\n",test_hpn_add(b,a));
  printf("T3-addN : %d\n",test_hpn_add(a*(-1),b*(-1)));
  printf("T4-addNR: %d\n",test_hpn_add(b*(-1),a*(-1)));
  printf("T5-sub  : %d\n",test_hpn_add(a,b*(-1)));
  printf("T6-subR : %d\n",test_hpn_add(b*(-1),a));
  printf("T7-subN : %d\n",test_hpn_add(a*(-1),b));
  printf("T8-subNR: %d\n",test_hpn_add(b,a*(-1)));
  // --- Multiplication --- //
  printf("HPN - Multiplication - 4 Tests \n");
  a = 1.3255037518;
  b = 1.2550952765;
  printf("T1-mlt  : %d\n",test_hpn_mult(a,b));
  printf("T2-mltR : %d\n",test_hpn_mult(b,a));
  printf("T3-mltN : %d\n",test_hpn_mult(a*(-1),b));
  printf("T4-mltNN: %d\n",test_hpn_mult(a*(-1),b*(-1)));
  #endif
  // === Next Iteration === //
  printf("Next Iteration \n");
  double cr =  1.3255037518;
  double ci =  1.2550952765;
  double zr = -0.8182940918;
  double zi =  0.6892104001;
  printf("T1: %d\n",test_next_iteration(zr,zi,cr,ci));
}
