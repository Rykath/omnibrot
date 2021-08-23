/* relative.hpp
 * deepixel/relative - header file
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * Relative Calculation
 *
 * NOTICE
 *  This library provides templates, therefore it can't be precompiled
 */

#ifndef OMNIBROT_RELATIVE_HPP
#define OMNIBROT_RELATIVE_HPP

#include <cmath>
#include <cstring>

#include "../common/complex.hpp"

template <class RefBaseType, class DifBaseType>
ComplexNumber<DifBaseType> next_iteration(ComplexNumber<RefBaseType> Z, ComplexNumber<DifBaseType> dZ, ComplexNumber<DifBaseType> dC){
  return ComplexNumber<DifBaseType>(2)*ComplexNumber<DifBaseType>(Z)*dZ + dZ*dZ + dC;
}

template <class RefBaseType, class DifBaseType>
DifBaseType norm(ComplexNumber<RefBaseType> X, ComplexNumber<DifBaseType> dX){ // difference to norm(X)
  return norm(dX) + DifBaseType(2)*DifBaseType(X.real)*dX.real + DifBaseType(2)*DifBaseType(X.imag)*dX.imag;
}

template <class RefBaseType, class DifBaseType>
DifBaseType norm2(ComplexNumber<RefBaseType> X, ComplexNumber<DifBaseType> dX){ // difference to norm(X)
  // same as norm, but with intermediate conversion to RefBaseType, maybe more accurate but slower?
  return DifBaseType(RefBaseType(norm(dX)) + RefBaseType(2)*X.real*RefBaseType(dX.real) + RefBaseType(2)*X.imag*RefBaseType(dX.imag));
}

// --- Approximation Array --- //
template <class BaseType, int size> // BaseType is usually ComplexNumber<ActualBaseType>
class ApproxFunction {
public:
  BaseType parameters[size];

  ApproxFunction(){
    memset(parameters,0,size*sizeof(BaseType));
  }
};

template <class BaseType, int size>
ApproxFunction<BaseType,size> operator+ (ApproxFunction<BaseType,size> A, ApproxFunction<BaseType,size> B){
  ApproxFunction<BaseType,size> C;
  for (int i=0; i<size; i++){
    C.parameters[i] = A.parameters[i] + B.parameters[i];
  }
  return C;
}

template <class BaseType, int size>
ApproxFunction<BaseType,size> operator* (ApproxFunction<BaseType,size> A, ApproxFunction<BaseType,size> B){
  ApproxFunction<BaseType,size> C;
  for (int sum=1; sum<size+1; sum++){
    C.parameters[sum-1] = 0;
    for (int p=1; p<sum; p++){
      if (p >= size+1 || sum-p >= size+1){
        continue;
      }
      C.parameters[sum-1] = C.parameters[sum-1] + A.parameters[p-1]*B.parameters[sum-p-1];
    }
  }
  return C;
}

template <class BaseType, int size, class OtherType>
ApproxFunction<BaseType,size> operator* (ApproxFunction<BaseType,size> A, OtherType B){
  ApproxFunction<BaseType,size> C;
  for (int i=0; i<size; i++){
    C.parameters[i] = OtherType(A.parameters[i]) * B;
  }
  return C;
}

template <class BaseType, int size, class OffsetType>
OffsetType approx(ApproxFunction<BaseType,size> AF, OffsetType D){
  OffsetType res;
  for (int i=0; i<size; i++){
    res = res + OffsetType(AF.parameters[i]) * pow(D,i+1);
  }
  return res;
}

template <class BaseType, int size, class RefType>
ApproxFunction<BaseType,size> MandelApproxPolyIter (ApproxFunction<BaseType,size> A, RefType Z){
  ApproxFunction<BaseType,size> X = A*RefType(2.0)*Z + A*A;
  X.parameters[0] = X.parameters[0] + BaseType(1.0);
  return X;
}

#endif //OMNIBROT_RELATIVE_HPP
