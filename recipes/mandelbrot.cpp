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
#include <cstring>

#include "../common/complex.hpp"
#include "../common/asdf.hpp"
#include "../deepixel/deepixel.hpp"
#include "../deepixel/relative.hpp"

#define CALC_FUN calc_Relative2CenterDoubleApprox
#define TYPE_ITER uint16_t
#define PROGRESS_INTERVAL 8
#define PEEK_ITERATION 64

#define STRINGIFY(X) #X
#define STRING_MACRO(X) STRINGIFY(X)

using namespace std;

// --- Utility --- //

char* str(double d){
  char* c = (char*) malloc(20*sizeof(char));
  sprintf(c,"%12e",d);
  return c;
}

// --- Calculation --- //

void calc_FPHPN(FILE* fptr, ComplexNumber<FPHPN> center, ComplexNumber<double> range, int width, int height, int maxIter){
  ComplexNumber<FPHPN> spacing, C, C0, D;
  TYPE_ITER I;

  spacing.real = range.real/width;
  spacing.imag = range.imag/height;
  C0.real = center.real - spacing.real * FPHPN((double)width/2 - 0.5); // corner vertex
  C0.imag = center.imag + spacing.imag * FPHPN((double)height/2 - 0.5);

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
}

void calc_double(FILE* fptr, ComplexNumber<FPHPN> center, ComplexNumber<double> range, int width, int height, int maxIter){
  ComplexNumber<double> D, C;
  TYPE_ITER I;

  for (int h=0; h<height; h++){
    C.imag = double(center.imag) + range.imag * ((double(h)+0.5) / height - 0.5);
    if (h % (height / PROGRESS_INTERVAL) == 0){
      printf("Row: %d%% %d/%d\n",h*100/height,h,height);
    }
    for (int w=0; w<width; w++){
      C.real = double(center.real) + range.real * ((double(w)+0.5) / width - 0.5);
      I = escapetime(C,D,maxIter);
      fwrite(&I, sizeof(TYPE_ITER), 1,fptr);
    }
  }
}

void calc_Relative2Center(FILE* fptr, ComplexNumber<FPHPN> center, ComplexNumber<double> range, int width, int height, int maxIter){
  ComplexNumber<FPHPN> Zcen [maxIter];
  double esc[maxIter];
  TYPE_ITER max;
  ComplexNumber<double> dZ, dC;

  Zcen[0] = center;
  for (int i=0; i<maxIter-1; i++){
    esc[i] = 4 - double(norm(Zcen[i]));
    Zcen[i+1] = next_iteration(Zcen[i],center);
  }

  for (int h=0; h<height; h++){
    if (h % (height / PROGRESS_INTERVAL) == 0){
      printf("Row: %d%% %d/%d\n",h*100/height,h,height);
    }
    dC.imag = double(range.imag) * ((double(h)+0.5) / height - 0.5);
    for (int w=0; w<width; w++){
      dC.real = double(range.real) * ((double(w)+0.5) / width - 0.5);
      dZ = dC;
      max = maxIter;
      for (TYPE_ITER i=0; i<TYPE_ITER(maxIter); i++){
        if (norm(Zcen[i],dZ) > esc[i]){
          fwrite(&i,sizeof(TYPE_ITER),1,fptr);
          max = 0;
          break;
        }
        dZ = next_iteration(Zcen[i],dZ,dC);
      }
      if (max){
        fwrite(&max,sizeof(TYPE_ITER),1,fptr);
      }
    }
  }
}

void calc_Relative2CenterFPHPN(FILE* fptr, ComplexNumber<FPHPN> center, ComplexNumber<double> range, int width, int height, int maxIter){
  ComplexNumber<FPHPN> Zcen [maxIter];
  FPHPN esc[maxIter];
  TYPE_ITER max;
  ComplexNumber<FPHPN> dZ, dC;

  Zcen[0] = center;
  for (int i=0; i<maxIter-1; i++){
    if (i % (maxIter / PROGRESS_INTERVAL) == 0){
      printf("Iter: %d%% %d/%d\n",i*100/maxIter,i,maxIter);
    }
    esc[i] = FPHPN(4) - norm(Zcen[i]);
    Zcen[i+1] = next_iteration(Zcen[i],center);
  }

  for (int h=0; h<height; h++){
    if (h % (height / PROGRESS_INTERVAL) == 0){
      printf("Row: %d%% %d/%d\n",h*100/height,h,height);
    }
    dC.imag = double(range.imag) * ((double(h)+0.5) / height - 0.5);
    for (int w=0; w<width; w++){
      dC.real = double(range.real) * ((double(w)+0.5) / width - 0.5);
      dZ = dC;
      max = maxIter;
      for (TYPE_ITER i=0; i<TYPE_ITER(maxIter); i++){
        if (norm(Zcen[i],dZ) > esc[i]){
          fwrite(&i,sizeof(TYPE_ITER),1,fptr);
          max = 0;
          break;
        }
        dZ = next_iteration(Zcen[i],dZ,dC);
      }
      if (max){
        fwrite(&max,sizeof(TYPE_ITER),1,fptr);
      }
    }
  }
}

void calc_Relative2CenterDouble(FILE* fptr, ComplexNumber<FPHPN> center, ComplexNumber<double> range, int width, int height, int maxIter){
  ComplexNumber<double> Zcen [maxIter];
  double esc[maxIter];
  TYPE_ITER max;
  ComplexNumber<double> dZ, dC, cen;
  cen = center;

  Zcen[0] = cen;
  for (int i=0; i<maxIter-1; i++){
    if (i % (maxIter / PROGRESS_INTERVAL) == 0){
      printf("Iter: %d%% %d/%d\n",i*100/maxIter,i,maxIter);
    }
    esc[i] = 4 - double(norm(Zcen[i]));
    Zcen[i+1] = next_iteration(Zcen[i],cen);
  }

  for (int h=0; h<height; h++){
    if (h % (height / PROGRESS_INTERVAL) == 0){
      printf("Row: %d%% %d/%d\n",h*100/height,h,height);
    }
    dC.imag = double(range.imag) * ((double(h)+0.5) / height - 0.5);
    for (int w=0; w<width; w++){
      dC.real = double(range.real) * ((double(w)+0.5) / width - 0.5);
      dZ = dC;
      max = maxIter;
      for (TYPE_ITER i=0; i<TYPE_ITER(maxIter); i++){
        if (norm(Zcen[i],dZ) > esc[i]){
          fwrite(&i,sizeof(TYPE_ITER),1,fptr);
          max = 0;
          break;
        }
        dZ = next_iteration(Zcen[i],dZ,dC);
      }
      if (max){
        fwrite(&max,sizeof(TYPE_ITER),1,fptr);
      }
    }
  }
}

void calc_Relative2CenterDoubleApprox(FILE* fptr, ComplexNumber<FPHPN> center, ComplexNumber<double> range, int width, int height, int maxIter){
  ComplexNumber<double> Zcen [maxIter];
  ApproxFunction<ComplexNumber<double>,32> AF [maxIter];
  AF[0].parameters[0] = 1;
  double esc[maxIter];
  TYPE_ITER max;
  ComplexNumber<double> dZ, dC, cen;
  cen = center;

  Zcen[0] = cen;
  for (int i=0; i<maxIter-1; i++){
    if (i % (maxIter / PROGRESS_INTERVAL) == 0){
      printf("Iter: %d%% %d/%d\n",i*100/maxIter,i,maxIter);
    }
    esc[i] = 4 - double(norm(Zcen[i]));
    Zcen[i+1] = next_iteration(Zcen[i],cen);
    AF[i+1] = MandelApproxPolyIter(AF[i],Zcen[i]);
  }

  for (int h=0; h<height; h++){
    if (h % (height / PROGRESS_INTERVAL) == 0){
      printf("Row: %d%% %d/%d\n",h*100/height,h,height);
    }
    dC.imag = double(range.imag) * ((double(h)+0.5) / height - 0.5);
    for (int w=0; w<width; w++){
      dC.real = double(range.real) * ((double(w)+0.5) / width - 0.5);
      dZ = dC;
      max = maxIter;
      for (TYPE_ITER i=0; i<TYPE_ITER(maxIter); i++){
        if (norm(Zcen[i],dZ) > esc[i]){
          fwrite(&i,sizeof(TYPE_ITER),1,fptr);
          max = 0;
          break;
        }
        //dZ = next_iteration(Zcen[i],dZ,dC);
        dZ = approx(AF[i],dC);
      }
      if (max){
        fwrite(&max,sizeof(TYPE_ITER),1,fptr);
      }
    }
  }
}

void calc_Relative2CenterApprox(FILE* fptr, ComplexNumber<FPHPN> center, ComplexNumber<double> range, int width, int height, int maxIter){
  ComplexNumber<FPHPN> Zcen;
  ApproxFunction<ComplexNumber<double>,32> AF;
  double esc;
  ComplexNumber<double> dC;
  TYPE_ITER* sector = (TYPE_ITER*) malloc(width*height*sizeof(TYPE_ITER));
  for (int i=0; i<width*height; i++){
    sector[i] = maxIter;
  }

  AF.parameters[0] = 1;
  Zcen = center;

  for (TYPE_ITER i=0; i<TYPE_ITER(maxIter); i++){
    if (i % (maxIter / PROGRESS_INTERVAL) == 0){
      printf("Iter: %d%% %d/%d\n",i*100/maxIter,i,maxIter);
    }
    esc = 4 - double(norm(Zcen));
    for (int h=0; h<height; h++) {
      dC.imag = double(range.imag) * ((double(h) + 0.5) / height - 0.5);
      for (int w = 0; w < width; w++) {
        dC.real = double(range.real) * ((double(w) + 0.5) / width - 0.5);
        if (sector[h * width + w] == maxIter && norm(Zcen, approx(AF, dC)) > esc) {
          sector[h * width + w] = i;
        }
      }
    }
    AF = MandelApproxPolyIter(AF,Zcen);
    Zcen = next_iteration(Zcen,center);
  }
  // --- Store --- //
  for (int h=0; h<height; h++){
    for (int w=0; w<width; w++){
      fwrite(sector+h*width+w,sizeof(TYPE_ITER),1,fptr);
    }
  }
}

void calc_Relative2CenterApproxDouble(FILE* fptr, ComplexNumber<FPHPN> center, ComplexNumber<double> range, int width, int height, int maxIter){
  ComplexNumber<double> Zcen;
  ApproxFunction<ComplexNumber<double>,8> AF;
  double esc;
  ComplexNumber<double> dC, cen;
  cen = center;
  TYPE_ITER* sector = (TYPE_ITER*) malloc(width*height*sizeof(TYPE_ITER));
  for (int i=0; i<width*height; i++){
    sector[i] = maxIter;
  }

  AF.parameters[0] = 1;
  Zcen = cen;

  for (TYPE_ITER i=0; i<TYPE_ITER(maxIter); i++){
    if (i % (maxIter / PROGRESS_INTERVAL) == 0){
      printf("Iter: %d%% %d/%d\n",i*100/maxIter,i,maxIter);
    }
    esc = 4 - double(norm(Zcen));
    for (int h=0; h<height; h++) {
      dC.imag = double(range.imag) * ((double(h) + 0.5) / height - 0.5);
      for (int w = 0; w < width; w++) {
        dC.real = double(range.real) * ((double(w) + 0.5) / width - 0.5);
        if (sector[h * width + w] == maxIter && norm(Zcen, approx(AF, dC)) > esc) {
          sector[h * width + w] = i;
        }
      }
    }
    AF = MandelApproxPolyIter(AF,Zcen);
    Zcen = next_iteration(Zcen,cen);
  }
  // --- Store --- //
  for (int h=0; h<height; h++){
    for (int w=0; w<width; w++){
      fwrite(sector+h*width+w,sizeof(TYPE_ITER),1,fptr);
    }
  }
}

void calc_RelativeApproxCenter(FILE* fptr, ComplexNumber<FPHPN> center, ComplexNumber<double> range, int width, int height, int maxIter){
  ComplexNumber<FPHPN> Zcen, ZcenSave;
  TYPE_ITER IcenSave;
  ApproxFunction<ComplexNumber<double>,16> AF, AFSave;
  double normThresh;

  bool Save;
  ComplexNumber<double> offset;
  TYPE_ITER* sector = (TYPE_ITER*) malloc(width*height*sizeof(TYPE_ITER));
  for (int i=0; i<width*height; i++){
    sector[i] = maxIter;
  }

  // --- build poly & center --- //
  IcenSave = 0;
  ZcenSave = center;
  memset(AFSave.parameters, 0, sizeof(AFSave.parameters));
  AFSave.parameters[0] = 1;
  Zcen = ZcenSave;
  AF = AFSave;
  while (IcenSave < maxIter-PEEK_ITERATION) {
    printf("Peek: %d%% %d/%d\n",IcenSave*100/maxIter,IcenSave,maxIter);
    for (int i = IcenSave; i < IcenSave + PEEK_ITERATION; i++) {
      AF = MandelApproxPolyIter(AF, Zcen);
      Zcen = next_iteration(Zcen, center);
    }
    // --- check escape --- //
    Save = false;
    if (calc_esc(Zcen)) {
      Save = true;
    } else { // check whole sector
      normThresh = 4-double(norm(Zcen));
      printf("debug-thresh: %f\n",normThresh);
      for (int h = 0; h < height; h++) {
        offset.imag = double(range.imag) * ((double(h)+0.5) / height - 0.5);
        for (int w = 0; w < width; w++) {
          offset.real = double(range.real) * ((double(w)+0.5) / width - 0.5);
          if (norm(Zcen,approx(AF, offset)) > normThresh) {
            printf("debug-esc-save: iter %d, pos %d %d, Z %s %s & %f %f, norm %f\n",IcenSave+PEEK_ITERATION,h,w,str(Zcen.real),str(Zcen.imag),offset.real,offset.imag,norm(approx(AF,offset)));
            Save = true;
            break;
          }
        }
        if (Save) { break; }
      }
    }
    if (Save) {break;}
    IcenSave += PEEK_ITERATION;
    ZcenSave = Zcen;
    AFSave = AF;
  }
  printf("debug AF: %d %s %s\n",IcenSave,str(AFSave.parameters[0].real),str(AFSave.parameters[0].imag));
  // --- Revert and Calculate --- //
  AF = AFSave;
  Zcen = ZcenSave;
  for (int i=IcenSave; i<maxIter; i++){
    normThresh = 4-double(norm(Zcen));
    if (i % ((maxIter-IcenSave) / PROGRESS_INTERVAL) == 0){
      printf("Iter: %d%% %d/%d\n",(i-IcenSave)*100/(maxIter-IcenSave),i-IcenSave,maxIter-IcenSave);
    }
    for (int h=0; h<height; h++){
      offset.imag = double(range.imag) * ((double(h)+0.5)/height - 0.5);
      for (int w=0; w<width; w++){
        offset.real = double(range.real) * ((double(w)+0.5)/width - 0.5);
        if (sector[h*width+w] == maxIter && norm(Zcen,approx(AF,offset)) > normThresh){
          sector[h*width+w] = i;
        }
      }
    }
    AF = MandelApproxPolyIter(AF,Zcen);
    Zcen = next_iteration(Zcen,center);
  }
  // --- Store --- //
  for (int h=0; h<height; h++){
    for (int w=0; w<width; w++){
      fwrite(sector+h*width+w,sizeof(TYPE_ITER),1,fptr);
    }
  }
}

// --- MAIN --- //

int main(int argc, char** argv){
  // --- Arguments --- //
  int width, height, maxIter;
  ComplexNumber<FPHPN> center;
  ComplexNumber<double> range;
  char* fname;

  // --- Arguments --- //
  if (argc != 8) {
    fprintf(stderr,"Error: wrong number of arguments\n");
    exit(1);
  }
  sscanf(argv[1],"%dx%d",&width,&height);
  center.real = FPHPN(argv[2]);
  center.imag = FPHPN(argv[3]);
  range.real = strtod(argv[4], nullptr);
  range.imag = strtod(argv[5], nullptr);
  maxIter = strtol(argv[6], nullptr, 10);
  fname = argv[7];

  // --- Declarations --- //
  FILE* fptr;
  time_t rawtime = time(nullptr);
  tm* ts = gmtime(&rawtime);
  char timestamp[20];

  strftime(timestamp,20,"%Y-%m-%dT%H:%M:%S",ts);

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
  fprintf(fptr, "  timestamp: '%19s'\n",timestamp);
  fprintf(fptr, "debug:\n");
  fprintf(fptr, "  original-arguments:\n"); // program + filename omitted
  fprintf(fptr, "    samples: '%s'\n",argv[1]);
  fprintf(fptr, "    center: ['%s','%s']\n",argv[2],argv[3]);
  fprintf(fptr, "    range: ['%s','%s']\n",argv[4],argv[5]);
  fprintf(fptr, "    max-Iter: '%s'\n",argv[6]);
  fprintf(fptr, "  configuration:\n");
  fprintf(fptr, "    TYPE_ITER: " STRING_MACRO(TYPE_ITER) "\n");
  fprintf(fptr, "    NUM_SIZE: " STRING_MACRO(NUM_SIZE) "\n");
  fprintf(fptr, "    CALC_SIZE: " STRING_MACRO(CALC_SIZE) "\n");
  fprintf(fptr, "sector:\n");
  fprintf(fptr, "  center:\n");
  fprintf(fptr, "    real: '%s'\n", str(center.real));
  fprintf(fptr, "    imag: '%s'\n", str(center.imag));
  fprintf(fptr, "  range:\n");
  fprintf(fptr, "    real: '%s'\n", str(range.real));
  fprintf(fptr, "    imag: '%s'\n", str(range.imag));
  fprintf(fptr, "data-raw: !core/ndarray-1.0.0\n");
  fprintf(fptr, "  source: 0\n");
  fprintf(fptr, "  datatype: uint16\n");
  fprintf(fptr, "  byteorder: little\n");
  fprintf(fptr, "  shape: [%d, %d]\n",width,height);
  fprintf(fptr, "...\n");

  // --- Calculation & Output --- //
  asdf::block_header(fptr, width*height*sizeof(TYPE_ITER));
  CALC_FUN(fptr, center, range, width, height, maxIter);
  fclose(fptr);
  return 0;
}
