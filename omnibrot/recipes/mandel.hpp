/* mandel.hpp
 * header file for Mandelbrot - mandel.cpp
 */

#ifndef MANDEL_HPP
#define MANDEL_HPP

#include "../common/complex.hpp"
#include "../deepixel/deepixel.hpp"

// --- standard mandelbrot --- //
// generic reference implementation (with double or FPHPN)
// using `escapetime`
// not optimized
template <typename INT, class BaseType>
void mandel(INT* array, ComplexNumber<BaseType> center, ComplexNumber<BaseType> range, ComplexNumber<int> shape, INT maxIter){
//void mandel(long* array, ComplexNumber<FPHPN> center, ComplexNumber<FPHPN> range, ComplexNumber<int> shape, long maxIter){
  ComplexNumber<BaseType> D, C;

  for (int i = 0; i < shape.imag; i++){
    C.imag = center.imag + range.imag * BaseType((i + 0.5) / shape.imag - 0.5);
    for (int r = 0; r < shape.real; r++){
      C.real = center.real + range.real * BaseType((r + 0.5) / shape.real - 0.5);
      array[i * shape.real + r] = escapetime(C, D, maxIter);
    }
  }
}

//template <typename INT, class CenterBaseType, class RelativeBaseType>
//void mandel(INT*, ComplexNumber<CenterBaseType>, ComplexNumber<RelativeBaseType>, ComplexNumber<int>, INT);
void mandel(long*, ComplexNumber<FPHPN>, ComplexNumber<double>, ComplexNumber<int>, long);


#endif // MANDEL_HPP
