/*
  ==============================================================================

   This file is part of the Belle, Bonne, Sage library
   Copyright 2007-2010 by William Andrew Burnson

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

#ifndef primVector
#define primVector

#include "primTypes.h"
#include "primMath.h"

namespace prim
{
  namespace math
  {
    /**Cartesian is a high-level coordinate-pair template. It is used by
    math::Vector, which is a Cartesian that uses prim::number as its number
    system. If you write your own number system class, then you can use
    Cartesian with it, so long as you overload the correct math functions in
    the math namespace. If you look through the implementation then you'll
    see which ones require overloading. The most convenient feature of
    Cartesian is its ability to fluidly work with polar coordinates and also
    the ability to treat pairs of x-y coordinates as single entities, to cut
    down on parameter lists.*/
    template <class T>
    struct Cartesian
    {
      ///Stores the value for the x-coordinate.
      T x;

      ///Stores the value for the y-coordinate.
      T y;


      //---------------------------//
      //CARTESIAN-POLAR CONVERSIONS//
      //---------------------------//

      /**\brief Stores the cartesian equivalent of the given angle (in
      radians) and magnitude. \details Note that specifying a negative
      magnitude will cause the angle to increase by Pi. This method loses
      less precision than by calling Mag and Ang separately.*/
      inline void Polar(T Angle,
        T Magnitude=(T)1)
      {
        x = Cos(Angle) * Magnitude;
        y = Sin(Angle) * Magnitude;
      }

      /**Returns the magnitude (distance from the origin) of the x-y pair.
      */
      inline T Mag(void) const
      {
        return math::Dist(x,y);
      }

      /**\brief Changes the magnitude (distance from the origin) of the
      x-y pair while keeping the angle the same.*/
      inline void Mag(T Magnitude)
      {
        T Angle = Ang();
        Polar(Angle, Magnitude);
      }

      ///Returns the angle of the x-y pair relative to the origin.
      inline T Ang(void) const
      {
        if(x == 0.0 && y == 0.0)
          return 0;
        else if(Abs(y) < Abs(x))
        {
          if(x > 0)
            return ArcTan(y / x);
          else
            return ArcTan(y / x) + Pi;
        }
        else
        {
          if(y > 0)
            return HalfPi - ArcTan(x / y);
          else
            return ThreeHalvesPi - ArcTan(x / y);
        }
      }

      /**\brief Changes the angle of the x-y pair relative to the origin
      while keeping the magnitude (distance from the origin) the same.*/
      inline void Ang(T Angle)
      {
        T Magnitude = Mag();
        Polar(Angle, Magnitude);
      }

      ///Returns the magnitude distance to another cartesian coordinate.
      inline T Mag(Cartesian OtherCartesian) const
      {
        return math::Dist(x, y, OtherCartesian.x, OtherCartesian.y);
      }

      ///Returns the angle between this x-y pair and another.
      inline T Ang(Cartesian OtherCartesian) const
      {
        Cartesian<T> Delta(OtherCartesian.x - x,
          OtherCartesian.y - y);

        return Delta.Ang();
      }

      //---------//
      //OPERATORS//
      //---------//

      /**\brief Multiplies both x and y coordinates by some multiplier (in
      other words, it multiplies the magnitude) and returns the result.*/
      inline Cartesian operator* (T Multiplier) const
      {
        return Cartesian(x * Multiplier, y * Multiplier);
      }

      /**\brief Divides both x and y coordinates by some divisor (in other
      words, it divides the magnitude) and returns the result.*/
      inline Cartesian operator/ (T Divisor) const
      {
        return Cartesian(x / Divisor, y / Divisor);
      }

      ///Adds another x-y pair to this one and returns the result.
      inline Cartesian operator+ (Cartesian otherCartesian) const
      {
        return Cartesian(x + otherCartesian.x, y + otherCartesian.y);
      }

      ///Substracts another x-y pair to this one and returns the result.
      inline Cartesian operator- (Cartesian otherCartesian) const
      {
        return Cartesian(x - otherCartesian.x, y - otherCartesian.y);
      }

      /**\brief Multiplies both x and y coordinates by some multiplier (in
      other words, it multiplies the magnitude) and stores the result.*/
      inline Cartesian operator*= (T Multiplier)
      {
        x *= Multiplier;
        y *= Multiplier;
        return *this;
      }

      /**\brief Divides both x and y coordinates by some divisor (in other
      words, it divides the magnitude) and stores the result.*/
      inline Cartesian operator/= (T Divisor)
      {
        x /= Divisor;
        y /= Divisor;
        return *this;
      }

      ///Adds another x-y pair to this one and stores the result.
      inline Cartesian operator+= (Cartesian otherCartesian)
      {
        x += otherCartesian.x;
        y += otherCartesian.y;
        return *this;
      }

      ///Substracts another x-y pair to this one and stores the result.
      inline Cartesian operator-= (Cartesian otherCartesian)
      {
        x -= otherCartesian.x;
        y -= otherCartesian.y;
        return *this;
      }

      ///Assignment operator copies another x-y pair into this one.
      inline Cartesian operator = (Cartesian otherCartesian)
      {
        x = otherCartesian.x;
        y = otherCartesian.y;
        return *this;
      }

      ///Equivalence comparison operator.
      inline bool operator == (Cartesian otherCartesian) const
      {
        return x == otherCartesian.x && y == otherCartesian.y;
      }

      ///Unequivalence comparison operator.
      inline bool operator != (Cartesian otherCartesian) const
      {
        return x != otherCartesian.x || y != otherCartesian.y;
      }

      //------------//
      //CONSTRUCTORS//
      //------------//

      ///Creates a Cartesian x-y pair with the given coordinates.
      Cartesian(T x, T y)
      {
        Cartesian<T>::x = x;
        Cartesian<T>::y = y;
      }

      /**\brief Creates a Cartesian x-y pair from an angle, with the
      magnitude normalized to one.*/
      Cartesian(T Angle)
      {
        Polar(Angle);
      }

      /**\brief Creates a Cartesian x-y pair from an angle, with the
      magnitude normalized to one.*/
      Cartesian(T Angle, T Magnitude, bool UseDegrees)
      {
        if(UseDegrees)
          Polar(Angle * DegToRad, Magnitude);
        else
          Polar(Angle, Magnitude);
      }

      ///Copy constructor copies another x-y pair into this one.
      Cartesian(const Cartesian& otherCartesian)
      {
        x = otherCartesian.x;
        y = otherCartesian.y;
      }

      ///Creates a Cartesian x-y pair at the origin.
      Cartesian() : x(0), y(0) {}
    };

    /**\brief The type-definition for prim::Vector is a Cartesian of
    representation type prim::number.*/
    typedef Cartesian<number> Vector;
  }
}
#endif
