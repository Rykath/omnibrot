/* complex.cpp
 * common/complex - source file
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * Complex Numbers - base class
 */

#include "complex.hpp"

// === Constructors === //
template <class BaseType>
ComplexNumber<BaseType>::ComplexNumber() {
  real = BaseType();
  imag = BaseType();
}

template <class BaseType>
ComplexNumber<BaseType>::ComplexNumber(BaseType R, BaseType I) {
  real = R;
  imag = I;
}

// === Operators === //
template <class BaseType>
ComplexNumber<BaseType> operator+ (ComplexNumber<BaseType> A, ComplexNumber<BaseType> B){
  return ComplexNumber<BaseType>(A.real + B.real, A.imag + B.imag);
}

template <class BaseType>
ComplexNumber<BaseType> operator- (ComplexNumber<BaseType> A, ComplexNumber<BaseType> B){
  return ComplexNumber<BaseType>(A.real - B.real, A.imag - B.imag);
}

template <class BaseType>
ComplexNumber<BaseType> operator* (ComplexNumber<BaseType> A, ComplexNumber<BaseType> B){
  return ComplexNumber<BaseType>(A.real * B.real - A.imag * B.imag, A.imag * B.real + A.real * B.imag);
}

template <class BaseType>
BaseType norm (ComplexNumber<BaseType> X){
  return X.real * X.real + X.imag * X.imag;
}

// === Special === //
template <class BaseType>
ComplexNumber<BaseType> next_iteration_ref(ComplexNumber<BaseType> Z, ComplexNumber<BaseType> C) {
  return Z*Z + C;
}