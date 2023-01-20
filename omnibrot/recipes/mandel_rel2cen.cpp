/* Mandelbrot - calculated relative to precise reference at center
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * using:
 *  - deepixel/deepixel (FPHPN)
 *  - deepixel/relative (relative next_iteration)
 *  - common/asdf
 *
 * Arguments:
 *  - samples (2):   '<sizeX> <sizeY>'  both int
 *  - center (2):    '<real> <imag>'    both hex_FPHPN
 *  - range (2):     '<real> <imag>'    both double
 *  - max Iteration: '<iter>'           int
 *  - filename:      '<path/file.asdf>' str
 */

// === Includes === //

#include "../common/asdf.hpp"
#include "../deepixel/deepixel.hpp"
#include "../deepixel/relative.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>

// === Configuration === //

#define PROGRAM_NAME "Mandelbrot-Rel2Cen"
#define PROGRAM_KEYWORDS "FPHPN, Relative"
#define PROGRAM_VERSION "1.0.0"

#define TYPE_ITER uint32_t
#define TYPE_ITER_ASDF "uint32"
#define ASDF_BYTEORDER "little"
#define PROGRESS_INTERVAL 8
#define ITERATION_CAP_FRACTION 1
#define ITERATION_CAP_OFFSET -5
// === Utility === //

#define STRINGIFY(X) #X
#define STRING_MACRO(X) STRINGIFY(X)

// === Program === //

using namespace std;

int main(int argc, char** argv){
  // --- Arguments --- //
  int width, height;
  TYPE_ITER maxIter;
  ComplexNumber<FPHPN> center;
  ComplexNumber<double> range;
  char* fname;

  // --- Arguments --- //
  if (argc != 9) {
    fprintf(stderr,"Error: wrong number of arguments\n");
    exit(1);
  }
  width = strtol(argv[1], nullptr, 10);
  height = strtol(argv[2], nullptr, 10);
  center.real = FPHPN(argv[3]);
  center.imag = FPHPN(argv[4]);
  range.real = strtod(argv[5], nullptr);
  range.imag = strtod(argv[6], nullptr);
  maxIter = strtol(argv[7], nullptr, 10);
  fname = argv[8];

  // --- Declarations --- //
  FILE* fptr;
  time_t rawtime = time(nullptr);
  tm* ts = gmtime(&rawtime);
  char timestamp[20];

  strftime(timestamp,20,"%Y-%m-%dT%H:%M:%S",ts);

  // === CALCULATION PART I === //
  ComplexNumber<FPHPN>* Zcen = (ComplexNumber<FPHPN>*) malloc(maxIter * sizeof(ComplexNumber<FPHPN>));
  double* esc = (double*) malloc(maxIter * sizeof(double));
  TYPE_ITER Icen = maxIter;
  TYPE_ITER Icap = maxIter;

  Zcen[0] = center;
  esc[0] = 4 - double(norm(Zcen[0]));
  printf("Reference\n");
  for (TYPE_ITER i=0; i<maxIter-1; i++){
    Zcen[i+1] = next_iteration(Zcen[i],center);
    esc[i+1] = 4 - double(norm(Zcen[i+1]));
    if (esc[i+1] < 0) {
      Icen = i + 1;
      // cap iteration at fraction of the reference escape iteration
      Icap = Icen * ITERATION_CAP_FRACTION + ITERATION_CAP_OFFSET;
      printf("Reference escapes at %d/%d, capping maximum Iteration.\n", Icen, maxIter);
      break;
    }
  }
  

  // === METADATA === //
  fptr = fopen(fname,"w");
  if (fptr == nullptr){
    fprintf(stderr,"Error opening file for writing metadata\n");
    exit(1);
  }
  asdf::header(fptr);
  fprintf(fptr, "program:\n");
  fprintf(fptr, "  name: %s\n",PROGRAM_NAME);
  fprintf(fptr, "  keywords: [%s]\n",PROGRAM_KEYWORDS);
  fprintf(fptr, "  version: %s\n",PROGRAM_VERSION);
  fprintf(fptr, "  timestamp: '%19s'\n",timestamp);
  fprintf(fptr, "debug:\n");
  fprintf(fptr, "  original-arguments:\n"); // program + filename omitted
  fprintf(fptr, "    samples: ['%s','%s']\n",argv[1],argv[2]);
  fprintf(fptr, "    center: ['%s','%s']\n",argv[3],argv[4]);
  fprintf(fptr, "    range: ['%s','%s']\n",argv[5],argv[6]);
  fprintf(fptr, "    max-Iter: '%s'\n",argv[7]);
  fprintf(fptr, "  calculation:\n");
  fprintf(fptr, "    escape-iteration-center: %d\n", Icen);
  fprintf(fptr, "    switch-relative-exact-iteration: %d\n", Icap);
  fprintf(fptr, "configuration:\n");
  fprintf(fptr, "  TYPE_ITER: " STRING_MACRO(TYPE_ITER) "\n");
  fprintf(fptr, "  FPHPN_INT_BITS: " STRING_MACRO(INT_BITS) "\n");
  fprintf(fptr, "  FPHPN_NUM_SIZE: " STRING_MACRO(NUM_SIZE) "\n");
  fprintf(fptr, "  FPHPN_CALC_SIZE: " STRING_MACRO(CALC_SIZE) "\n");
  fprintf(fptr, "  ITERATION_CAP_FRACTION: " STRING_MACRO(ITERATION_CAP_FRACTION) "\n");
  fprintf(fptr, "  ITERATION_CAP_OFFSET: " STRING_MACRO(ITERATION_CAP_OFFSET) "\n");
  fprintf(fptr, "sector:\n");
  fprintf(fptr, "  type: Mandelbrot\n");
  fprintf(fptr, "  center:\n");
  fprintf(fptr, "    real: '%s'\n", str(center.real));
  fprintf(fptr, "    imag: '%s'\n", str(center.imag));
  fprintf(fptr, "  range:\n");
  fprintf(fptr, "    real: '%s'\n", str(range.real));
  fprintf(fptr, "    imag: '%s'\n", str(range.imag));
  fprintf(fptr, "  samples: {'real': %d, 'imag': %d}\n",width,height);
  fprintf(fptr, "  orientation: ['-real','+imag']\n");
  fprintf(fptr, "  maximum-iteration: %d\n",maxIter);
  fprintf(fptr, "data-raw: !core/ndarray-1.0.0\n");
  fprintf(fptr, "  source: 0\n");
  fprintf(fptr, "  datatype: %s\n",TYPE_ITER_ASDF);
  fprintf(fptr, "  byteorder: %s\n",ASDF_BYTEORDER);
  fprintf(fptr, "  shape: [%d, %d]\n",width,height);
  fprintf(fptr, "...\n");

  asdf::block_header(fptr, width*height*sizeof(TYPE_ITER));

  // === CALCULATION PART II === //
  // Calculate relative to reference path (with FPHPN) at sector center
  // omits 0th iteration, sets D=C -> output one less than reference implementation (generic complex/escapetime)
  // if the reference escapes, relative calculation is not possible beyond that point, so the iterations are capped
  // however we can continue with normal FPHPN calculation from that point
  ComplexNumber<double> dZ, dC;
  ComplexNumber<FPHPN> Ze;
  ComplexNumber<FPHPN> Ce;
  bool escape;

  for (int h=0; h<height; h++){
    if (h % (height / PROGRESS_INTERVAL) == 0){
      printf("Row: %d%% %d/%d\n",h*100/height,h,height);
    }
    dC.imag = -1 * double(range.imag) * ((double(h)+0.5) / height - 0.5);
    for (int w=0; w<width; w++){
      dC.real = double(range.real) * ((double(w)+0.5) / width - 0.5);
      dZ = dC;
      escape = false;
      // relative calculation 
      for (TYPE_ITER i = 0; i < Icap; i++){
        // escapes
        if (norm(Zcen[i], dZ) > esc[i]){
          fwrite(&i, sizeof(TYPE_ITER), 1, fptr);
          escape = true;
          break;
        }
        dZ = next_iteration(Zcen[i], dZ, dC);
      }
      // handle escape & special cases
      if (escape) {
        continue;
      }
      if (Icap == maxIter){
        fwrite(&Icap, sizeof(TYPE_ITER), 1, fptr);
        continue;
      }
      // continue with exact calculation
      Ce = center + ComplexNumber<FPHPN>(dC);
      Ze = Zcen[Icap] + ComplexNumber<FPHPN>(dZ);
      TYPE_ITER i = Icap + escapetime(Ce, Ze, maxIter - Icap);
      fwrite(&i, sizeof(TYPE_ITER), 1, fptr);
    }
  }
  fclose(fptr);
  return 0;
}

