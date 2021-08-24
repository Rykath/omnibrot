/* complex.hpp
 * common/complex - header & source file
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * Complex Numbers - template base class
 *
 * NOTICE
 *  This library provides templates, therefore it can't be precompiled
 *  -> no complex.cpp, all definitions are located in complex.hpp as well
 */

#ifndef OMNIBROT_COMPLEX_HPP
#define OMNIBROT_COMPLEX_HPP

template <class BaseType>
class ComplexNumber {
public:
  // --- Components --- //
  BaseType real;
  BaseType imag;

  // --- Constructors --- //
  ComplexNumber(){
    real = BaseType(0);
    imag = BaseType(0);
  }

  ComplexNumber(BaseType R){
    real = R;
    imag = BaseType(0);
  }

  template <class OtherBaseType>
  ComplexNumber(ComplexNumber<OtherBaseType> C){
    real = BaseType(C.real);
    imag = BaseType(C.imag);
  }

  template <class OtherType>
  ComplexNumber(OtherType R){
    real = BaseType(R);
    imag = BaseType(0);
  }

  ComplexNumber(BaseType R ,BaseType I){
    real = R;
    imag = I;
  }

  template <class OtherType>
  ComplexNumber(OtherType R, OtherType I){
    real = BaseType(R);
    imag = BaseType(I);
  }
};

// --- Operators --- //
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
BaseType norm (ComplexNumber<BaseType> X) {
  return X.real * X.real + X.imag * X.imag;
}

template <class BaseType>
ComplexNumber<BaseType> pow(ComplexNumber<BaseType> C, unsigned int E){
  ComplexNumber<BaseType> X = C;
  for (unsigned int i=1; i<E; i++){
    X = X * C;
  }
  return X;
}

// --- Special --- //
template <class BaseType>
ComplexNumber<BaseType> next_iteration_ref(ComplexNumber<BaseType> Z, ComplexNumber<BaseType> C) {
  return Z*Z + C;
}

template <class BaseType>
bool calc_esc_ref(ComplexNumber<BaseType> X){
  return norm(X) >= 4;
}

template <class BaseType>
int escapetime(ComplexNumber<BaseType> C, ComplexNumber<BaseType> D, int maxI){
  ComplexNumber<BaseType> Z = D;
  if (calc_esc(Z)){
    return 0;
  }
  for (int iter=1; iter<maxI; iter++){
    Z = next_iteration(Z,C);
    if (calc_esc(Z)){
      return iter;
    }
  }
  return maxI;
}

// --- Defaults --- //
template <class BaseType>
ComplexNumber<BaseType> next_iteration(ComplexNumber<BaseType> Z, ComplexNumber<BaseType> C){
  return next_iteration_ref(Z,C);
}

template <class BaseType>
bool calc_esc(ComplexNumber<BaseType> X){
  return calc_esc_ref(X);
}

#endif //OMNIBROT_COMPLEX_HPP
