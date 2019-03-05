/* deepixel.c
 * deePixel lib - source file
 * by Robert Babin
 *
 * calculate the mandelbrot series escape for one pair of coordinates
 */

#include <stdio.h> // printf
#include <stdbool.h> // boolean variables
#include <stdlib.h> // calloc

// ----- CONFIGURATION ----- //

// length of the High Precision Number in subunits
#define NUM_SIZE 4
// number of additional subunits during calculation
#define CALC_SIZE_ADD 2

// testing subunits
// std: PART_* defined
//      CHECK_* not defined
#define PART_IMAG_I
#define PART_IMAG_II
#define PART_REAL_I
#define PART_REAL_II
#define PART_REAL_III
#define CHECK_SEGFAULT

// ----- CONSTANTS ----- //

#define BIN32 4294967295 // 2^32-1
#define BIN33 4294967296 // 2^32

// ----- STRUCTS <LOCAL> ----- //

// High Precision Number - HPN
struct HighPrecNum {
  bool neg;
  unsigned int* digits;
};

// Complex Number, consisting of 2 High Precision Numbers
struct ComplexNum {
  struct HighPrecNum real;
  struct HighPrecNum imag;
};

// ----- LOCAL FUNCTIONS ----- //

void hpn_new(struct HighPrecNum* hpn){
  hpn->neg = false;
  hpn->digits = (unsigned int*) calloc(NUM_SIZE, sizeof(unsigned int));
}

char hpn_larger(struct HighPrecNum** s, struct HighPrecNum** l, struct HighPrecNum num1, char c1, struct HighPrecNum num2, char c2){
  // determine larger High Precision Number
  // based on absolute values (.digits), ignoring sign (.neg)
  for (int i = 0; i < NUM_SIZE; i++){
    if (num1.digits[i] > num2.digits[i]){
      *s = &num2;
      *l = &num1;
      return c1;
    }
    else if (num1.digits[i] < num2.digits[i]){
      *s = &num1;
      *l = &num2;
      return c2;
    }
  }
  // default: s = num1, l = num2
  *s = &num1;
  *l = &num2;
  return '=';
}

void hpn_from_double(struct HighPrecNum* hpnptr, double number){
  if (number == 0){
    hpnptr->neg = false;
    for (int i = 0; i < NUM_SIZE; i++){
      hpnptr->digits[i] = 0;
    }
    return;
  }
  else if (number > 0){
    hpnptr->neg = false;
  }
  else {
    hpnptr->neg = true;
    number *= -1;
  }
  for (int i = 0; i < NUM_SIZE; i++){
    hpnptr->digits[i] = (unsigned int) number;
    number -= hpnptr->digits[i];
    number *= BIN33;
  }
}

// ----- VISIBLE FUNCTIONS ----- //

int check_sizes(){
  if (sizeof(int) != 4 || sizeof(long) != 8){
    fprintf(stderr, "wrong integer sizes\nint: %ld/4\nlong: %ld/8\n",sizeof(int),sizeof(long));
    return(-1);
  }
  return(0);
}

struct ComplexNum next_z_imag(struct ComplexNum posZo, struct ComplexNum posZ, struct ComplexNum posC){
  // posZo = old Z
  // posZ = preallocated slot for new Z - members of posZ.imag are modified
  // posC = C
  unsigned long buffer;
  unsigned long carry_s; // small carry - 64 bit long, usually filled 32 bit, same position as buffer
  unsigned long carry_l; // large carry - 64 bit long, usually filled 32 bit, shifted 32 bits left from buffer
  struct HighPrecNum* ptr_s = NULL; // pointer to smaller number
  struct HighPrecNum* ptr_l = NULL; // pointer to larger number
  char larger;

  // Z.i = 2 * Zo.r * Zo.i + C.i
  
  #ifdef PART_IMAG_I
  // --- I --- //
  // Z.i = 2 * Zo.r * Zo. i
  posZ.imag.neg = (posZo.imag.neg != posZo.real.neg);
  carry_l = 0;
  carry_s = 0;
  for (int p = NUM_SIZE+CALC_SIZE_ADD-1; p >= 0; p--){ // looping through result from behind
    for (int i = 0; i <= p; i++){ // loop through all possible inputs
      if (i >= NUM_SIZE || p-i >= NUM_SIZE){
        continue;
      }
      buffer = posZo.real.digits[i];
      buffer *= posZo.imag.digits[p-i];
      carry_l += (buffer >> 32) << 1;
      carry_s += (buffer & BIN32) << 1;
      //printf("posZ.imag-buffer: %d %d | %10u x %10u > %20lu == %20lu == %20lu\n",i,p-i,posZo.real.digits[i],posZo.imag.digits[p-i],buffer,carry_l,carry_s);
    }
    #ifdef PART_IMAG_II
    // --- II.a --- //
    // if Z.i and C.i have same sign
    // Z.i += C.i
    if (posZ.imag.neg == posC.imag.neg && p < NUM_SIZE){
      carry_s += posC.imag.digits[p];
    }
    #endif
    // --- I cont. --- //
    carry_l += carry_s >> 32; // add overflow of carry_s to carry_l
    if (p < NUM_SIZE){
      posZ.imag.digits[p] = carry_s; // only last 32 bits are copied anyway
      //printf("posZ.imag: %d | %10u == %20lu\n",p,posZ.imag.digits[p],carry_s);
    }
    carry_s = carry_l;
    carry_l = 0;
  }
  #endif
  #ifdef PART_IMAG_II
  // --- II.b --- //
  // if Z.i and C.i have opposite sign
  // Z.i += C.i
  if (posZ.imag.neg != posC.imag.neg){
    larger = hpn_larger(&ptr_s, &ptr_l, posZ.imag, 'Z', posC.imag, 'C');
    // if Z.i == C.i
    if (larger == '='){
      for (int p = 0; p < NUM_SIZE; p++){
        posZ.imag.digits[p] = 0;
      }
      posZ.imag.neg = false;
    }
    else {
      carry_l = 0;
      carry_s = 0;
      for (int p = NUM_SIZE-1; p >= 0; p--){ // looping from behind
        carry_s += ptr_s->digits[p];
	carry_l = carry_s >> 32;
	carry_s = carry_s & BIN32;
	if (ptr_l->digits[p] < carry_s){
	  carry_l += 1;
	}
	posZ.imag.digits[p] = ptr_l->digits[p] - carry_s;
	carry_s = carry_l;
      }
    }
    if (larger == 'C'){
      posZ.imag.neg = !posZ.imag.neg;
    }
  }
  #endif
  return posZ;
}

struct ComplexNum next_z_real(struct ComplexNum posZo, struct ComplexNum posZ, struct ComplexNum posC){
  // posZo = old Z
  // posZ = preallocated slot for new Z - members of posZ.real are modified
  // posC = C
  unsigned long buffer;
  unsigned long carry_s; // small carry
  unsigned long carry_l; // large carry
  struct HighPrecNum* ptr_s = NULL; // pointer to smaller number
  struct HighPrecNum* ptr_l = NULL; // pointer to larger number
  char larger;

  // Z.r = Zo.r^2 - Zo.i^2 + C.r

  // --- I --- //
  // Z.r = Zo.larger^2
  larger = hpn_larger(&ptr_s, &ptr_l, posZo.real, 'r', posZo.imag, 'i');
  printf("%c %8X %8X | %8X %8X\n",larger,ptr_l->digits[1],ptr_s->digits[1],posZo.real.digits[1],posZo.imag.digits[1]);
  if (larger == '='){
    // --- II.a --- //
    // Z.r = C.r
    for (int p = 0; p < NUM_SIZE; p++){
      posZ.real.digits[p] = posC.real.digits[p];
    }
    posZ.real.neg = posC.real.neg;
    // --- I cont. --- //
  }
  else {
    if (larger == 'r'){
      posZ.real.neg = false;
    }
    else {
      posZ.real.neg = true;
    }
    carry_s = 0;
    carry_l = 0;
    //printf("%u.%u.%u.%u\n",(ptr_l->digits)[0],(ptr_l->digits)[1],(ptr_l->digits)[2],(ptr_l->digits)[3]);
    #ifdef PART_REAL_I
    for (int p = NUM_SIZE+CALC_SIZE_ADD-1; p >= 0; p--){
      // larger^2
      for (int i = 0; i <= p/2; i++){
        if (i >= NUM_SIZE || p-i >= NUM_SIZE){
	  continue;
	}
	buffer = ptr_l->digits[i];
	buffer *= ptr_l->digits[p-i];
        if (2*i == p){
	  carry_l += buffer >> 32;
          carry_s += buffer & BIN32;
	}
	else { // *2
	  carry_l += (buffer >> 32) << 1;
	  carry_s += (buffer & BIN32) << 1;
	}
        //printf("p:%d i:%d d(%d):%u d(%d):%u buf:%lu cl:%lu cs:%lu\n",p,i,i,(ptr_l->digits)[i],p-i,(ptr_l->digits)[p-i],buffer,carry_l,carry_s);
        printf("posZ.real-buffer: %d %d | %8X x %8X > %16lX == %16lX == %16lX\n",i,p-i,ptr_l->digits[i],ptr_l->digits[p-i],buffer,carry_l,carry_s);
      }
      // --- II.b --- //
      #ifdef PART_REAL_II
      // C.r has same sign as larger
      if (posC.real.neg == ptr_l->neg && p < NUM_SIZE){
        carry_s += posC.real.digits[p];
      }
      #endif
      // --- I cont. --- //
      carry_l += carry_s >> 32;
      //printf("r.carry_s: %lu\n",carry_s);
      if (p < NUM_SIZE){
        posZ.real.digits[p] = carry_s;
        printf("posZ.real: %d | %8X == %16lX\n",p,posZ.real.digits[p],carry_s);
      }
      carry_s = carry_l;
      carry_l = 0;
    }
    #endif
    //printf("r.larger^2(+c.r): %d %u..%10u \n",posZ.real.neg,posZ.real.digits[0],posZ.real.digits[1]);
    // --- III --- //
    // Z.r -= Zo.smaller^2
    carry_s = 0;
    carry_l = 0;
    #ifdef PART_REAL_III
    printf("posZ.real-smaller: %d %8X.%8X.%8X.%8X\n",ptr_s->neg,ptr_s->digits[0],ptr_s->digits[1],ptr_s->digits[2],ptr_s->digits[3]);
    for (int p = NUM_SIZE+CALC_SIZE_ADD-1; p >= 0; p--){
      #ifdef CHECK_SEGFAULT
      printf("check p %d\n",p);
      #endif
      for (int i = 0; i <= p/2; i++){
        #ifdef CHECK_SEGFAULT
	printf("check i %d\n",i);
	#endif
        if (i >= NUM_SIZE || p-i >= NUM_SIZE){
	  continue;
	}
	buffer = ptr_s->digits[i];
	buffer *= ptr_s->digits[p-i];
	if (2*i == p){
	  carry_l += buffer >> 32;
	  carry_s += buffer & BIN32;
	}
	else { // *2
	  carry_l += (buffer >> 32) << 1;
	  carry_s += (buffer & BIN32) << 1;
	}
	printf("posZ.real-buffer: %d %d | %8X x %8X > %16lX == %16lX == %16lX\n",i,p-i,ptr_s->digits[i],ptr_s->digits[p-i],buffer,carry_l,carry_s);
      }
      #ifdef CHECK_SEGFAULT
      printf("check II.c\n");
      #endif
      // --- II.c --- //
      #ifdef PART_REAL_II
      // C.r has same sign as smaller
      if (posC.real.neg == ptr_s->neg && p < NUM_SIZE){
        carry_s += posC.real.digits[p];
      }
      #endif
      // --- III cont. --- //
      carry_l += carry_s >> 32;
      carry_s = carry_s & BIN32;
      if (p < NUM_SIZE){
        if (posZ.real.digits[p] < carry_s){
          carry_l += 1;
        }
        posZ.real.digits[p] -= carry_s;
        printf("posZ.real: %d | %8X == %16lX\n",p,posZ.real.digits[p],carry_s);
      }
      carry_s = carry_l;
      carry_l = 0;
    }
    #endif
    if (carry_s != 0){ // Zo.smaller^2 + C.r > Zo.larger^2
      posZ.real.neg = !posZ.real.neg; // switch sign
      buffer = 1;
      for (int p = NUM_SIZE-1; p >= 0; p--){
        buffer += ~posZ.real.digits[p];
        posZ.real.digits[p] = buffer; // clipping to 32 bits
	buffer = buffer >> 32;
      }
    }
  }
//  printf("next real: %d %u..%10u \n",posZ.real.neg,posZ.real.digits[0],posZ.real.digits[1]);
  return posZ;
}

int main(int argc, char *argv[]){
  // just one pixel
  // arguments: program max_iteration cr ci
  if (argc != 4 || check_sizes()){
    return -1;
  }
  int iteration_max = atoi(argv[1]);
  int iteration;
  struct ComplexNum posC;
  hpn_new(&(posC.real));
  hpn_new(&(posC.imag));
  hpn_from_double(&(posC.real), atof(argv[2]));
  hpn_from_double(&(posC.imag), atof(argv[3]));
  struct ComplexNum posZ1;
  hpn_new(&(posZ1.real));
  hpn_new(&(posZ1.imag));
  hpn_from_double(&(posZ1.real), atof(argv[2]));
  hpn_from_double(&(posZ1.imag), atof(argv[3]));
  struct ComplexNum posZ2;
  hpn_new(&(posZ2.real));
  hpn_new(&(posZ2.imag));
  struct ComplexNum* ptrZ = &posZ1;
  /*
  for (iteration = 0; iteration < iteration_max; iteration++){
    printf("%2d: %d %u..%10u..%10u | %d %u..%10u..%10u\n",iteration,ptrZ->real.neg,ptrZ->real.digits[0],ptrZ->real.digits[1],ptrZ->real.digits[2],ptrZ->imag.neg,ptrZ->imag.digits[0],ptrZ->imag.digits[1],ptrZ->imag.digits[2]);
    if (ptrZ->real.digits[0]*ptrZ->real.digits[0]+ptrZ->imag.digits[0]*ptrZ->imag.digits[0] > 4){
      //printf("%d\n",iteration);
      return iteration;
    }
    if (iteration % 2 == 0){
      posZ2 = next_z_real(posZ1,posZ2,posC);
      posZ2 = next_z_imag(posZ1,posZ2,posC);
      ptrZ = &posZ2;
    }
    else{
      posZ1 = next_z_real(posZ2,posZ1,posC);
      posZ1 = next_z_imag(posZ2,posZ1,posC);
      ptrZ = &posZ1;
    }
  }
  //printf("max\n");
  return iteration_max;
  */
  printf("iteration: negative real-value | negative imaginary-value\n");
  iteration = 0;
  printf("%2d: %d %X..%8X..%8X | %d %X..%8X..%8X\n",iteration,ptrZ->real.neg,ptrZ->real.digits[0],ptrZ->real.digits[1],ptrZ->real.digits[2],ptrZ->imag.neg,ptrZ->imag.digits[0],ptrZ->imag.digits[1],ptrZ->imag.digits[2]);
  iteration += 1;
  posZ2 = next_z_imag(posZ1,posZ2,posC);
  posZ2 = next_z_real(posZ1,posZ2,posC);
  ptrZ = &posZ2;
  printf("%2d: %d %X..%8X..%8X | %d %X..%8X..%8X\n",iteration,ptrZ->real.neg,ptrZ->real.digits[0],ptrZ->real.digits[1],ptrZ->real.digits[2],ptrZ->imag.neg,ptrZ->imag.digits[0],ptrZ->imag.digits[1],ptrZ->imag.digits[2]);
}
