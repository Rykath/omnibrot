/* Mandelbrot
 *
 * different implementations using different types
 *   differences in speed, memory usage, accuracy and significant digits
 * functions to be called externally (e.g. by Cython)
 */

#include "mandel.hpp"
#include "../common/complex.hpp"
#include "../deepixel/deepixel.hpp"
#include "../deepixel/relative.hpp"

#include <cstdlib>

using namespace std;

// --- relative to center path --- //
// the escape path of the center sample is calculated with high accuracy
// all other samples are calculated using this path with lower accuracy
// switches over to a standard mandelbrot once the center escapes
// ToDo: switch calculation already sooner?
// ToDo: add template
//   template <typename INT, class CenterBaseType, class RelativeBaseType>
//   void mandel(INT array[], ComplexNumber<CenterBaseType> center, ComplexNumber<RelativeBaseType> range, ComplexNumber<int> shape, INT Imax){
void mandel(long array[], ComplexNumber<FPHPN> center, ComplexNumber<double> range, ComplexNumber<int> shape, long Imax){
  ComplexNumber<FPHPN>* Zcen = (ComplexNumber<FPHPN>*) malloc(Imax * sizeof(ComplexNumber<FPHPN>));
  double* radius = (double*) malloc(Imax * sizeof(double));
  long Icen = Imax;
  ComplexNumber<double> dZ, dC;

  // escape path of center sample
  // and escape radius for relative samples
  Zcen[0] = ComplexNumber<FPHPN>(0); // D
  radius[0] = FPHPN(4);
  for (long n = 1; n < Imax; n++){
    Zcen[n] = next_iteration(Zcen[n - 1], center);
    radius[n] = double(4) - double(norm(Zcen[n]));
    if (radius[n] <= double(0)){
      Icen = n;
      break;
    }
  }

  // loop over all samples
  for (int i = 0; i < shape.imag; i++){
    dC.imag = range.imag * double((i + 0.5) / shape.imag - 0.5);
    for (int r = 0; r < shape.real; r++){
      dC.real = range.real * double((r + 0.5) / shape.real - 0.5);
      dZ = 0; // dD
      //array[i * shape.real + r] = 256; // why?

      // relative calculation
      long n = 0;
      for (; n < Icen; n++){
        if (norm(Zcen[n], dZ) > radius[n]){
          array[i * shape.real + r] = n;
          break;
        }
        dZ = next_iteration(Zcen[n], dZ, dC);
      }

      // continue with standard mandelbrot (with D = Z + dZ)
      if (n >= Icen) {
        array[i * shape.real + r] = Icen + escapetime(center + ComplexNumber<FPHPN>(dC), Zcen[Icen] + ComplexNumber<FPHPN>(dZ), Imax - Icen);
      }
    }
  }
}


