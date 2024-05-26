/*
  ==============================================================================

   This file is part of the Belle, Bonne, Sage library
   Copyright 2007-2010 by Andi

  ------------------------------------------------------------------------------

   Belle, Bonne, Sage can be redistributed and/or modified under the terms of
   the GNU Lesser General Public License, as published by the
   Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Belle, Bonne, Sage is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with Belle, Bonne, Sage; if not, visit www.gnu.org/licenses or write:
   
   Free Software Foundation, Inc.
   59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ==============================================================================
*/

#ifndef primMath
#define primMath

#include "primTypes.h"

namespace prim
{
  //Forward declaration for later math declarations.
  class String;
}

namespace prim {

/** A replacement for "math.h" which causes ambiguity problems because of
its overlap with the C standard library. Also, depending on how number is
defined, the class automatically adjusts to use either float or double, so
that both are not defined (which can lead to typecasting issues). Breaking
with convention found elsewhere in prim, math functions are generally given
in as a few letters as possible to facilitate keeping complex formulae on as
few lines as possible.*/

namespace math
{
  //Forward declaration for Vector
  template <class NumberSystem> struct Cartesian;
  typedef Cartesian<number> Vector;

  //---------//
  //Constants//
  //---------//

  extern const number HalfPi;
  extern const number Pi;
  extern const number ThreeHalvesPi;
  extern const number TwoPi;
  extern const number DegToRad;
  extern const number RadToDeg;

  //---------------------//
  //Exponential Functions//
  //---------------------//
  
  number Exp(number x);

  number Exp(number base, number x);

  number Log(number x);

  number Log(number base, number x);

  //-----------------------//
  //Trigonometric Functions//
  //-----------------------//

  ///Returns the trigonometric cosine of a number given in radians.
  number Cos(number Radians);

  ///Returns the trigonometric sine of a number given in radians.
  number Sin(number Radians);

  ///Returns the trigonometric tangent of a number given in radians.
  number Tan(number Radians);

  ///Returns the inverse sine in radians of a number.
  number ArcSin(number x);

  ///Returns the inverse cosine in radians of a number.
  number ArcCos(number x);

  ///Returns the inverse tangent in radians of a number.
  number ArcTan(number x);

  ///Converts radians into degrees.
  number Deg(number Radians);

  ///Converts degrees into radians.
  number Rad(number Degrees);

  //---------------------------//
  //Absolute Value and Distance//
  //---------------------------//

  ///Returns the sign of a number.
  number Sgn(number x);

  ///Returns the sign of an integer.
  integer Sgn(integer x);

  ///Returns the absolute value of a number.
  number Abs(number x);

  ///Returns the absolute value of an integer.
  integer Abs(integer x);

  ///Returns the square root of a number.
  number Sqrt(number x);

  /**\brief Returns the distance between a pair of x-y coordinates and the
  origin.*/
  number Dist(number x, number y);

  /**\brief Returns the distance between two pairs of x-y coordinates and
  the origin.*/
  number Dist(number x1, number y1, number x2, number y2);

  ///Returns the distance between two vectors.
  number Dist(Vector& v1, Vector& v2);

  //------//
  //Modulo//
  //------//

  ///Returns a cyclic modulo (i.e. -1 mod 3 = 2 and not 1)
  integer Mod(integer x, integer y);

  ///Returns an absolute value modulo (i.e. -1 mod 3 = 1 and not 2)
  integer ModAbs(integer x, integer y);

  //-----------------//
  //Quadratic Formula//
  //-----------------//

  /**\brief Returns the real zeroes to a quadratic equation given the
  coefficients. \details The algorithm chosen is numerically stable. The
  first root given is always the least of the two. Duplicate roots are
  given as the first. The return value contains the number of roots.*/
  count Roots(number a, number b, number c,
    number& Root1, number& Root2);


  //--------//
  //Swapping//
  //--------//

  ///Swaps two things of the same type.
  template <class T> static void Swap(T& First, T& Second)
  {
    T Temporary=First;
    First=Second;
    Second=Temporary;
  }

  ///Sorts two numbers in ascending order.
  template <class T> static void Ascending(T& First, T& Second)
  {
    if(First > Second)
      Swap(First,Second);
  }

  ///Sorts two numbers in descending order.
  template <class T> static void Descending(T& First, T& Second)
  {
    if(First < Second)
      Swap(First,Second);
  }

  ///Returns the greater of two values.
  template <class T> static T Max(const T& First, const T& Second)
  {
    return (First > Second ? First : Second);
  }

  ///Returns the lesser of two values.
  template <class T> static T Min(const T& First, const T& Second)
  {
    return (First < Second ? First : Second);
  }
  
  ///Increases the value of the first to that of the second if it is larger.
  template <class T> static void Increase(T& First, const T& Second)
  {
    if(First < Second)
      First = Second;
  }
  
  ///Decreases the value of the first to that of the second if it is smaller.
  template <class T> static void Decrease(T& First, const T& Second)
  {
    if(First > Second)
      First = Second;
  }
  
  ///Determines if a value is in between two others (inclusively).
  template <class T> static bool IsInBetween(const T& Value, const T& Range1,
    const T& Range2)
  {
    return (Range2 >= Range1 && Value >= Range1 && Value <= Range2) ||
      (Range1 > Range2 && Value >= Range2 && Value <= Range1);
  }


  //-------------//
  //Randomization//
  //-------------//

  ///Computes a random integer between floor and ceiling values.
  integer Rand(integer Floor, integer Ceiling);

  ///Computes a random number between floor and ceiling values.
  number RandNumber(number Floor, number Ceiling);

  ///Computes a random integer between 0 and the ceiling value.
  integer Rand(integer Ceiling);

  ///Computes a random integer between 0 and the ceiling value.
  number RandNumber(number Ceiling);

  //-----------------------------//
  //String to Numeric Conversions//
  //-----------------------------//

  /**Converts a floating-point number to a decimal string. The method
  safeguards against the unpredictable use of scientific notation by
  setting the bounds from +-1E-10 at the minimum and +-1E10 at the
  maximum. Also, you may specify the number of significant figures to
  display, and all other digits will be zeroed or removed as
  appropriate.*/
  String NumberToString(number Number, count SignificantFigures=5);

  ///Converts a string to a number. If an error occurs the result is 0.
  number StringToNumber(const String& NumericString);

  ///Converts a string to an integer. If an error occurs the result is 0.
  integer StringToInteger(const String& NumericString);
  
  //-----------------//
  //Number Comparison//
  //-----------------//
  bool IsApproximatelyEqual(number Number1, number Number2,
    count DigitsOfSensitivity = 5);
}}

namespace prim { namespace math {

/**Templated meta-math functions that evaluate at compile-time. The purpose of
these functions is retrieve constant values from functions that are usually
considered noncomputable at compile time, such as logarithms or powers of
two.*/

namespace meta
{
  /**Computes a base-2 logarithm from an integer. For example,
  LogBaseTwo<32>::Value returns 5.*/
  template <uint64 u>
  struct LogBaseTwo
  {
    static const uint64 Value = (u % (uint64)2 == 0) ? 
      LogBaseTwo<u / (uint64)2>::Value + (uint64)1 : 0;
  };
  template <>
  struct LogBaseTwo<0>
  {
    static const uint64 Value = 0;
  };

  /**Computes a power of two given an exponent. For example,
  PowerOfTwo<5>::Value returns 32.*/
  template <uint64 u>
  struct PowerOfTwo
  {
    static const uint64 Value = PowerOfTwo<(uint64)u - 
      (uint64)1>::Value * (uint64)2;
  };
  template <>
  struct PowerOfTwo<0>
  {
    static const uint64 Value = 1;
  };

  /**Computes the maximum unsigned integer value of a bitfield of all ones.
  The meta-function is safe to use for all values up to the number of bits
  of uint64.*/
  template <count b>
  struct MaxValuePerBits
  {
    static const uint64 Value = PowerOfTwo<b - 1>::Value + 
      MaxValuePerBits<b - 1>::Value;
  };
  template <>
  struct MaxValuePerBits<0>
  {
    static const uint64 Value = 0;
  };

  /**Computes the maximum value of an unsigned integer type. For example,
  MaxValueOfUnsignedType<uint8>::Value returns 255.*/
  template <class T>
  struct MaxValueOfType
  {
    static const uint64 Value = MaxValuePerBits<(count)sizeof(T) *
      (count)8>::Value;
  };

  /**Computes the value of the highest bit of the type. For example,
  HighestBitOfUnsignedType<uint8>::Value returns 128.*/
  template <class T>
  struct HighestBitOfType
  {
    static const uint64 Value = PowerOfTwo<(uint64)sizeof(T) * (uint64)8 -
      (uint64)1>::Value;
  };
}}}
#endif
