/* Mandelbrot
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * using deepixel
 *
 * Arguments:
 *  - samples:      '<sizeX>x<sizeY>' both int
 *  - center (2):   '<real> <imag>'   both hex_FPHPN
 *  - range (2):    '<real> <imag>'   both double
 *  - max Iteration:  '<iter>'        int
 *  - filename:     '<path>'          str
 * e.g:
 * ./mandel   512x512     0.         0         4         4       255   ./test
 */

#include <cstdio>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <ctime>

#include "../deepixel/deepixel.hpp"
#include "../common/complex.hpp"
#include "../common/asdf.hpp"

#define TYPE_ITER uint16_t
#define BASE FPHPN
#define COMPLEX ComplexNumber<BASE>

#define STRINGIFY(X) #X
#define STRING_MACRO(X) STRINGIFY(X)

using namespace std;

int main(int argc, char** argv){
  int width, height, maxIter;
  COMPLEX center, spacing, D, C0, C;
  TYPE_ITER I;
  char* fname;
  FILE* fptr;
  time_t rawtime = time(nullptr);
  tm* ts = gmtime(&rawtime);
  char timestamp[20];

  // --- Arguments --- //
  if (argc != 8) {
    fprintf(stderr,"Error: wrong number of arguments\n");
    exit(1);
  }
  sscanf(argv[1],"%dx%d",&width,&height);
  center.real = BASE(argv[2]);
  center.imag = BASE(argv[3]);
  spacing.real = BASE(strtod(argv[4], nullptr)/width);
  spacing.imag = BASE(strtod(argv[5], nullptr)/height);
  maxIter = strtol(argv[6], nullptr, 10);
  fname = argv[7];

  // --- Metadata --- //
  fptr = fopen(fname,"w");
  if (fptr == nullptr){
    fprintf(stderr,"Error opening file for writing metadata\n");
    exit(1);
  }
  asdf::header(fptr);
  fprintf(fptr, "instructions:\n");
  fprintf(fptr, "  type: 'Mandelbrot'\n");
  fprintf(fptr, "  keywords: ['deepixel']\n");
  strftime(timestamp,20,"%Y-%m-%dT%H:%M:%S",ts);
  fprintf(fptr, "  timestamp: '%19s'\n",timestamp);
  fprintf(fptr, "debug:\n");
  fprintf(fptr, "  original-arguments:\n"); // program + filename omitted
  fprintf(fptr, "    samples: '%s'\n",argv[1]);
  fprintf(fptr, "    center: ['%s','%s']\n",argv[2],argv[3]);
  fprintf(fptr, "    range: ['%s','%s']\n",argv[4],argv[5]);
  fprintf(fptr, "    max-Iter: '%s'\n",argv[6]);
  fprintf(fptr, "  configuration:\n");
  fprintf(fptr, "    TYPE_ITER: " STRING_MACRO(TYPE_ITER) "\n");
  fprintf(fptr, "    BASE_TYPE: " STRING_MACRO(BASE) "\n");
  fprintf(fptr, "    COMPLEX_TYPE: " STRING_MACRO(COMPLEX) "\n");
  fprintf(fptr, "sector:\n");
  fprintf(fptr, "  center:\n");
  fprintf(fptr, "    real: '%s'\n", center.real.ret_hex_all());
  fprintf(fptr, "    imag: '%s'\n", center.imag.ret_hex_all());
  fprintf(fptr, "  spacing:\n");
  fprintf(fptr, "    real: '%s'\n", spacing.real.ret_hex_all());
  fprintf(fptr, "    imag: '%s'\n", spacing.imag.ret_hex_all());
  fprintf(fptr, "data-raw: !core/ndarray-1.0.0\n");
  fprintf(fptr, "  source: 0\n");
  fprintf(fptr, "  datatype: uint16\n");
  fprintf(fptr, "  byteorder: little\n");
  fprintf(fptr, "  shape: [%d, %d]\n",width,height);
  fprintf(fptr, "...\n");

  // --- Calculation & Output --- //

  C0.real = center.real - spacing.real * BASE((double)width/2 - 0.5);
  C0.imag = center.imag + spacing.imag * BASE((double)height/2 - 0.5);
  asdf::block_header(fptr, width*height*sizeof(TYPE_ITER));
  C = C0;

  for (int h=0; h<height; h++){
    C.real = C0.real;
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
