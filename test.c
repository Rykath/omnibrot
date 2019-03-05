/* test.c
 * test for deePixel
 * by Robert Babin
 */

#include <stdio.h>

#include "deepixel.h"

int main(){
  if (check_sizes()){
    return -1;
  }
  calc_escape_iteration(10,-0.9,-1.1,20,3.9,3.1,20,5,5);
  return 0;
}
