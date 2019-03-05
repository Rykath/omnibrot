#include <stdio.h>
#include <math.h>

// signed int 32 implementation - 1 bit sign, 31 bits data: 5 bits pre comma, 26 bits precision: factor=2^13, max 31.999

long mult_32(long a, long b){
  return (a/pow(2,13))*(b/pow(2,13)) + (a/pow(2,13)*(b-(b/pow(2,13))*pow(2,13)) + b/pow(2,13)*(a-(a/pow(2,13))*pow(2,13)))/pow(2,13);
}

void cxmult_32(long ar, long ai, long br, long bi, long* crp, long* cip){
  // C = A * B
  // Cr = Ar*Br - Ai*Bi
  // Ci = Ar*Bi + Ai*Br
  *crp = mult_32(ar,br) - mult_32(ai,bi);
  *cip = mult_32(ar,bi) + mult_32(ai,br);
}

// signed int 64 implementation - 1 bit sign, 3 bit pre comma, 60 bits precision: factor=2^30

long long mult_64(long long a, long long b){
  return (a/pow(2,30))*(b/pow(2,30)) + (a/pow(2,30)*(b-(b/pow(2,30))*pow(2,30)) + b/pow(2,30)*(a-(a/pow(2,30))*pow(2,30)))/pow(2,30);
}

void cxmult_64(long long ar, long long ai, long long br, long long bi, long long* crp, long long* cip){
  // C = A * B
  // Cr = Ar*Br - Ai*Bi
  // Ci = Ar*Bi + Ai*Br
  *crp = mult_64(ar,br) - mult_64(ai,bi);
  *cip = mult_64(ar,bi) + mult_64(ai,br);
}

// test

int main(int argc, char** argv){
  long a = -127782600;
  long b = 31770870;
  printf("%ld %f %f %f \n",mult_32(a,b),mult_32(a,b)/pow(2,26),a/pow(2,26),b/pow(2,26));

  long long u = 2772261101412312397;
  long long v =  756400994849272519;
  printf("%lld %f %f %f \n",mult_64(u,v),mult_64(u,v)/pow(2,60),u/pow(2,60),v/pow(2,60));

  printf("sizeof(char) => %d\n", sizeof(char));
  printf("sizeof(short) => %d\n", sizeof(short));
  printf("sizeof(short int) => %d\n", sizeof(short int));
  printf("sizeof(int) => %d\n", sizeof(int));
  printf("sizeof(long) => %d\n", sizeof(long));
  printf("sizeof(long int) => %d\n", sizeof(long int));
  printf("sizeof(long long) => %d\n", sizeof(long long));
  printf("sizeof(long long int) => %d\n", sizeof(long long int));

  return 0;
}
