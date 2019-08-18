/* Test
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * using deepixel
 *
 * Arguments: None
 */

#include <cstdio>

#include "../deepixel/relative.hpp"
#include "../deepixel/deepixel.hpp"
#include "../common/complex.hpp"

#define TEST test_relative_norm

// --- HELPER --- //

void print_ApproxFunction_Cd4(ApproxFunction<ComplexNumber<double>,4> AF){
  printf("AF: %f %f .. %f %f .. %f %f .. %f %f\n",AF.parameters[0].real,AF.parameters[0].imag,AF.parameters[1].real,
          AF.parameters[1].imag,AF.parameters[2].real,AF.parameters[2].imag,AF.parameters[3].real,
          AF.parameters[3].imag);
}

// --- TESTS --- //

void test_complex(){
  ComplexNumber<double> t = ComplexNumber<double>(-1.29384,-0.38492);
  printf("pow 1: %f %f = %f %f\n",t.real,t.imag,pow(t,1).real,pow(t,1).imag);
  printf("pow 2: %f %f = %f %f\n",(t*t).real,(t*t).imag,pow(t,2).real,pow(t,2).imag);
}

void test_FPHPN(){
  double d = 1.29384;
  double d2 = d-0.1;
  double d3 = -1*d;
  int i = 4;
  FPHPN fp = d;
  FPHPN fp2 = d2;
  FPHPN fp3 = d3;
  FPHPN fpi = i;
  printf("d: %f %f %f\n",d,d2,d3);
  printf("fp: %s %s %s\n",str(fp),str(fp2),str(fp3));
  printf("i: %d\n",i);
  printf("fpi: %s\n",str(fpi));
  printf("conv-d : %f \n",d);
  printf("conv-fp: %f | %s\n",double(fp),str(fp));
  printf("conv-i : %d \n",i);
  printf("conv-fp: %d | %s\n",int(fpi),str(fpi));
  printf("op-d  <: %d, %d %d %d %d %d %d\n",d<d,  d<d2,  d2<d,  d<d3,  d3<d,  d2<d3,  d3<d2);
  printf("op-fp <: %d, %d %d %d %d %d %d\n",fp<fp,fp<fp2,fp2<fp,fp<fp3,fp3<fp,fp2<fp3,fp3<fp2);
}

void test_relative_approx(){
  ApproxFunction<ComplexNumber<double>,16> AF;
  AF.parameters[0] = 1;
  ComplexNumber<double> Zcen0 = ComplexNumber<double>(0.59384,0.38492);
  ComplexNumber<double> offset0 = ComplexNumber<double>(-0.001283,-0.0023884);
  ComplexNumber<double> Zo0 = Zcen0+offset0;

  ComplexNumber<double> Zcen = Zcen0;
  ComplexNumber<double> offset = offset0;
  ComplexNumber<double> aprx = approx(AF,offset0);
  ComplexNumber<double> Zo = Zo0;

  printf("Zcen: %f %f\n",Zcen.real,Zcen.imag);
  printf("aprx: %e %e\n",aprx.real,aprx.imag);
  printf("ofst: %e %e\n",offset.real,offset.imag);
  printf("of+ : %f %f\n",Zcen.real+offset.real,Zcen.imag+offset.imag);
  printf("Zo  : %f %f\n",Zo.real,Zo.imag);
  for (int i=0; i<8; i++){
    AF = MandelApproxPolyIter(AF,Zcen);
    aprx = approx(AF,offset0);
    offset = next_iteration(Zcen,offset,offset0);
    Zcen = next_iteration(Zcen,Zcen0);
    Zo = next_iteration(Zo,Zo0);
    printf("--- %d \n",i);
    printf("Zcen: %f %f\n",Zcen.real,Zcen.imag);
    printf("aprx: %e %e\n",aprx.real,aprx.imag);
    printf("ofst: %e %e\n",offset.real,offset.imag);
    printf("of+ : %f %f\n",Zcen.real+offset.real,Zcen.imag+offset.imag);
    printf("Zo  : %f %f\n",Zo.real,Zo.imag);
  }
}

void test_relative_norm(){
  ComplexNumber<FPHPN> fp1 = ComplexNumber<FPHPN>(1.29384,0.38492);
  ComplexNumber<FPHPN> fp2 = ComplexNumber<FPHPN>(-1.29384,0.38492);
  ComplexNumber<FPHPN> fp3 = ComplexNumber<FPHPN>(1.29384,-0.38492);
  ComplexNumber<FPHPN> fp4 = ComplexNumber<FPHPN>(-1.29384,-0.38492);
  ComplexNumber<double> offset1 = ComplexNumber<double>(0.001283,0.0023884);
  ComplexNumber<double> offset2 = ComplexNumber<double>(-0.001283,0.0023884);
  ComplexNumber<double> offset3 = ComplexNumber<double>(0.001283,-0.0023884);
  ComplexNumber<double> offset4 = ComplexNumber<double>(-0.001283,-0.0023884);

  printf("norm-fp:       %8.5f %8.5f %8.5f %8.5f\n",double(norm(fp1)),double(norm(fp2)),double(norm(fp3)),double(norm(fp4)));
  printf("norm-rel-fp1:  %8.5f %8.5f %8.5f %8.5f\n",norm(fp1,offset1),norm(fp1,offset2),norm(fp1,offset3),norm(fp1,offset4));
  printf("norm2-rel-fp1: %8.5f %8.5f %8.5f %8.5f\n",norm2(fp1,offset1),norm2(fp1,offset2),norm2(fp1,offset3),norm2(fp1,offset4));
  printf("norm-rel-fp2:  %8.5f %8.5f %8.5f %8.5f\n",norm(fp2,offset1),norm(fp2,offset2),norm(fp2,offset3),norm(fp2,offset4));
  printf("norm-rel-fp3:  %8.5f %8.5f %8.5f %8.5f\n",norm(fp3,offset1),norm(fp3,offset2),norm(fp3,offset3),norm(fp3,offset4));
  printf("norm-rel-fp4:  %8.5f %8.5f %8.5f %8.5f\n",norm(fp4,offset1),norm(fp4,offset2),norm(fp4,offset3),norm(fp4,offset4));
}

void test_relative(){
  ComplexNumber<double> Za1 = ComplexNumber<double>(1.29384,0.38492);
  ComplexNumber<double> offset1 = ComplexNumber<double>(-0.001283,-0.0023884);
  ComplexNumber<double> Zb1 = Za1 + offset1;
  ComplexNumber<double> Za2, Zb2, offset2;

  ComplexNumber<FPHPN> Za1FP = Za1;
  ComplexNumber<FPHPN> Zb1FP = Zb1;
  ComplexNumber<FPHPN> offset1FP = Zb1FP-Za1FP;
  ComplexNumber<FPHPN> Za2FP, Zb2FP, offset2FP;

  ApproxFunction<ComplexNumber<double>,4> AF;
  AF.parameters[0] = 1;

  print_ApproxFunction_Cd4(AF);
  printf("Za-FP: %s %s, %f\n",str(Za1FP.real),str(Za1FP.imag),double(norm(Za1FP)));
  printf("Zb-FP: %s %s, %f\n",str(Zb1FP.real),str(Zb1FP.imag),double(norm(Zb1FP)));
  printf("offFP: %s %s, %f\n",str(offset1FP.real),str(offset1FP.imag),double(norm(Za1FP,offset1FP)));
  printf("of+FP: %s %s, %f\n",str(Za1FP.real+offset1FP.real),str(Za1FP.imag+offset1FP.imag),double(norm(Za1FP)+norm2(Za1FP,offset1FP)));
  printf(" Za : %f %f, %f\n",Za1.real,Za1.imag,norm(Za1));
  printf(" Zb : %f %f, %f\n",Zb1.real,Zb1.imag,norm(Zb1));
  printf("off : %f %f, %f\n",offset1.real,offset1.imag,norm(Za1,offset1));
  printf("off+: %f %f, %f\n",Za1.real+offset1.real,Za1.imag+offset1.imag,norm(Za1)+norm(Za1,offset1));

  printf("---\n");

  AF = MandelApproxPolyIter(AF,Za1FP);
  ComplexNumber<double> Zo = approx(AF,offset1);
  Za2FP = next_iteration(Za1FP,Za1FP);
  Zb2FP = next_iteration(Zb1FP,Zb1FP);
  offset2FP = next_iteration(Za1FP,offset1FP,offset1FP);

  Za2 = next_iteration(Za1,Za1);
  Zb2 = next_iteration(Zb1,Zb1);
  offset2 = next_iteration(Za1,offset1,offset1);

  print_ApproxFunction_Cd4(AF);
  printf("Za-FP: %s %s, %f\n",str(Za2FP.real),str(Za2FP.imag),double(norm(Za2FP)));
  printf("Zb-FP: %s %s, %f\n",str(Zb2FP.real),str(Zb2FP.imag),double(norm(Zb2FP)));
  printf("offFP: %s %s, %f\n",str(offset2FP.real),str(offset2FP.imag),double(norm(Za2FP,offset2FP)));
  printf("of+FP: %s %s, %f\n",str(Za2FP.real+offset2FP.real),str(Za2FP.imag+offset2FP.imag),double(norm(Za2FP)+norm2(Za2FP,offset2FP)));
  printf(" Za : %f %f, %f\n",Za2.real,Za2.imag,norm(Za2));
  printf(" Zb : %f %f, %f\n",Zb2.real,Zb2.imag,norm(Zb2));
  printf("off : %f %f, %f\n",offset2.real,offset2.imag,norm(Za2,offset2));
  printf("off+: %f %f, %f\n",Za2.real+offset2.real,Za2.imag+offset2.imag,norm(Za2)+norm(Za2,offset2));
  //printf("comp: FP %f %f - AF %f %f\n",double(Z2_FP.real-Z1_FP.real),double(Z2_FP.imag-Z1_FP.imag),double(Zo.real),double(Zo.imag));
  //printf("norm: d %f - FP %f - AF %f - AF2 %f\n",norm(Z2)-norm(Z1),double(norm(Z2_FP)-norm(Z1_FP)),norm(Z2_FP,Zo),norm2(Z2_FP,Zo));
}

// --- MAIN --- //
int main(){
  TEST();
  return 0;
}