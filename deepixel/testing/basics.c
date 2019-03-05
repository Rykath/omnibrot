/* deepixel/testing/basics.c
 * test for deePixel
 * by Robert Babin
 */

#include <stdio.h>

#include "../deepixel.h"

int main(){
  // === HighPrecisionNumbers === //
  // --- Addition & Subtraction --- //
  printf("HPN - Addition & Subtraction\n");
  double a = 1.3255037518;
  double b = 1.2550952765;
  printf("T1-add  : %d\n",test_hpn_add(a,b));
  printf("T2-addR : %d\n",test_hpn_add(b,a));
  printf("T3-addN : %d\n",test_hpn_add(a*(-1),b*(-1)));
  printf("T4-addNR: %d\n",test_hpn_add(b*(-1),a*(-1)));
  printf("T5-sub  : %d\n",test_hpn_add(a,b*(-1)));
  printf("T6-subR : %d\n",test_hpn_add(b*(-1),a));
  printf("T7-subN : %d\n",test_hpn_add(a*(-1),b));
  printf("T8-subNR: %d\n",test_hpn_add(b,a*(-1)));
}
