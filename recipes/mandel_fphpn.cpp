/* Mandelbrot - with Fixed Point High Precision Numbers
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * using:
 *  - deepixel/deepixel (FPHPN)
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

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>

// === Configuration === //

#define PROGRAM_NAME "Mandelbrot-FPHPN"
#define PROGRAM_KEYWORDS "FPHPN"
#define PROGRAM_VERSION "1.0.0"

#define TYPE_ITER uint16_t
#define TYPE_ITER_ASDF "uint16"
#define ASDF_BYTEORDER "little"
#define PROGRESS_INTERVAL 8

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

  ComplexNumber<FPHPN> spacing, C, C0, D;
  TYPE_ITER I;

  spacing.real = range.real/width;
  spacing.imag = range.imag/height;
  C0.real = center.real - spacing.real * FPHPN((double)width/2 - 0.5); // corner vertex
  C0.imag = center.imag + spacing.imag * FPHPN((double)height/2 - 0.5);

  // --- Metadata --- //
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
  fprintf(fptr, "    spacing:\n");
  fprintf(fptr, "      real: '%s'\n", str(spacing.real));
  fprintf(fptr, "      imag: '%s'\n", str(spacing.imag));
  fprintf(fptr, "    corner:\n");
  fprintf(fptr, "      real: '%s'\n", str(C0.real));
  fprintf(fptr, "      imag: '%s'\n", str(C0.imag));
  fprintf(fptr, "configuration:\n");
  fprintf(fptr, "  TYPE_ITER: " STRING_MACRO(TYPE_ITER) "\n");
  fprintf(fptr, "  FPHPN_INT_BITS: " STRING_MACRO(INT_BITS) "\n");
  fprintf(fptr, "  FPHPN_NUM_SIZE: " STRING_MACRO(NUM_SIZE) "\n");
  fprintf(fptr, "  FPHPN_CALC_SIZE: " STRING_MACRO(CALC_SIZE) "\n");
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

  // === CALCULATION === //
  C = C0;
  for (int h=0; h<height; h++){
    C.real = C0.real;
    if (h % (height / PROGRESS_INTERVAL) == 0){
      printf("Row: %d%% %d/%d\n",h*100/height,h,height);
    }
    for (int w=0; w<width; w++){
      I = escapetime(C,D,maxIter);
      fwrite(&I, sizeof(TYPE_ITER), 1,fptr);
      C.real = C.real + spacing.real;
    }
    C.imag = C.imag - spacing.imag;
  }
  fclose(fptr);
  return 0;
}
