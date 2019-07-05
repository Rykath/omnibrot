/* deepixel/testing/basics.cpp
 * test for deepixel
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 */

#include <cstdio>
#include <cstdlib>

#include "Xtremebrot/deepixel/deepixel.hpp"

using namespace std;

int main(int argc, char** argv){
  if (argc != 4){
    printf("error\n");
    return -1;
  }
  double cr = std::strtod(argv[1], nullptr);
  double ci = std::strtod(argv[2], nullptr);
  int i = std::strtol(argv[3], nullptr, 10);
  CFPHPN D = CFPHPN(0,0);
  CFPHPN C = CFPHPN(cr,ci);
  //std::printf("%lf %lf %d\n",cr,ci,i);
  std::printf("%d\n",escapetime(C,D,i));
}
