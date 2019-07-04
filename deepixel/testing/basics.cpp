/* deepixel/testing/basics.cpp
 * test for deepixel
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 */

#include <cstdio>

#include "Xtremebrot/deepixel/deepixel.hpp"

using namespace std;

int main(){
  double cr,ci;
  int i;
  std::scanf("%lf %lf %d",&cr,&ci,&i);
  CFPHPN D = CFPHPN(0,0);
  CFPHPN C = CFPHPN(cr,ci);
  std::printf("escape after: %d\n",escapetime(C,D,i));
}
