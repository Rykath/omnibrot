/* deepixel/testing/basics.cpp
 * test for deepixel
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 */

#include <cstdio>
#include <cstdlib>

#include "deepixel.hpp"

using namespace std;

// Arguments:  samples  range center  maxI
//  ./mandel   512x512  4 4     0 0   255
int main(int argc, char** argv){
  int width, height, maxIter;
  double range_cr,range_ci,center_cr,center_ci;
  if (argc != 7) {
    width = 16;
    height = 16;
    range_cr = 4;
    range_ci = 4;
    center_cr = 0;
    center_ci = 0;
    maxIter = 255;
  }
  else{
    sscanf(argv[1],"%dx%d",&width,&height);
    range_cr = strtod(argv[2], nullptr);
    range_ci = strtod(argv[3], nullptr);
    center_cr = strtod(argv[4], nullptr);
    center_ci = strtod(argv[5], nullptr);
    maxIter = strtol(argv[6], nullptr, 10);
  }

  CFPHPN D; //Z_0 = 0
  CFPHPN spacing (range_cr/width, range_ci/height);
  CFPHPN C0 (center_cr-range_cr/2, center_ci-range_ci/2);
  C0.real = C0.real + spacing.real * FPHPN(0.5);
  C0.imag = C0.imag + spacing.imag * FPHPN(0.5);

  CFPHPN C = C0;

  for (int h=0; h<height; h++){
    C.real = C0.real;
    for (int w=0; w<width; w++){
      //printf("%3d %+.4lf %+.4lf | ",escapetime(D,C,maxIter), C.real.ret_double(), C.imag.ret_double());
      printf("%d ",escapetime(C,D,maxIter));
      C.real = C.real + spacing.real;
    }
    printf("\n");
    C.imag = C.imag + spacing.imag;
  }
}