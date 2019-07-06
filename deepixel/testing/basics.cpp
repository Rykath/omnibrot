/* deepixel/testing/basics.cpp
 * test for deepixel
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 */

#include <cstdio>
#include <cstdlib>

#include "Xtremebrot/deepixel/deepixel.hpp"

using namespace std;

int escapetime_single_2stdo(int argc, char** argv){
  double cr, ci;
  int i;
  if (argc != 4){
    //printf("error\n");
    //return -1;
    cr = 1.113;
    ci = 1.04;
    i = 3;
  }
  else{
  cr = std::strtod(argv[1], nullptr);
  ci = std::strtod(argv[2], nullptr);
  i = std::strtol(argv[3], nullptr, 10);}
  CFPHPN D = CFPHPN(0,0);
  CFPHPN C = CFPHPN(cr,ci);
  CFPHPN Zo = D.next_iter_opt(C).next_iter_opt(C).next_iter_opt(C);
  std::printf("%s | %s / %lf %lf\n", Zo.real.ret_hex_all(),Zo.imag.ret_hex_all(),Zo.real.ret_double(),Zo.imag.ret_double());
  CFPHPN Zr = D.next_iter_ref(C).next_iter_ref(C).next_iter_ref(C);
  std::printf("%s | %s / %lf %lf\n", Zr.real.ret_hex_all(),Zr.imag.ret_hex_all(),Zr.real.ret_double(),Zr.imag.ret_double());
  //std::printf("%lf %lf %d\n",cr,ci,i);
  std::printf("%d\n",escapetime(C,D,i));
}

int main(int argc, char** argv){
  FPHPN A (0.875);
  FPHPN B (0.250);
  printf("%lf \n",(A+B).ret_double());
}