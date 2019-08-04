/* complex.hpp
 * common/complex - header file
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * Complex Numbers - base class
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
  ComplexNumber();
  ComplexNumber(BaseType,BaseType);
};

// --- Operators --- //
template <class BaseType> ComplexNumber<BaseType> operator+ (ComplexNumber<BaseType>, ComplexNumber<BaseType>);
template <class BaseType> ComplexNumber<BaseType> operator- (ComplexNumber<BaseType>, ComplexNumber<BaseType>);
template <class BaseType> ComplexNumber<BaseType> operator* (ComplexNumber<BaseType>, ComplexNumber<BaseType>);

template <class BaseType> BaseType norm (ComplexNumber<BaseType>);

// --- Special --- //
template <class BaseType>
ComplexNumber<BaseType> next_iteration_ref(ComplexNumber<BaseType>, ComplexNumber<BaseType>);

#endif //OMNIBROT_COMPLEX_HPP
