/* mandel.hpp
 * header file for Mandelbrot - mandel.cpp
 */

#ifndef MANDEL_HPP
#define MANDEL_HPP

#include "../common/complex.hpp"
#include "../deepixel/deepixel.hpp"

//template <typename INT, class BaseType>
//void mandel(INT*, ComplexNumber<BaseType>, ComplexNumber<BaseType>, ComplexNumber<int>, INT);
void mandel(long*, ComplexNumber<FPHPN>, ComplexNumber<FPHPN>, ComplexNumber<int>, long);

//template <typename INT, class CenterBaseType, class RelativeBaseType>
//void mandel(INT*, ComplexNumber<CenterBaseType>, ComplexNumber<RelativeBaseType>, ComplexNumber<int>, INT);
void mandel(long*, ComplexNumber<FPHPN>, ComplexNumber<double>, ComplexNumber<int>, long);


#endif // MANDEL_HPP
