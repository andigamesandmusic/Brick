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

#ifndef primDimensions
#define primDimensions

#include "primTypes.h"
#include "primVector.h"

namespace prim
{
  namespace math
  {
    ///Type definition for storing abstractly the compile-time unit type.
    typedef prim::count Unit;

    ///Unit and conversion definitions structure
    struct Units
    {
      //---------------//
      //Units of Length//
      //---------------//
      
      ///The basic unit of length from which other types are converted through.
      static const Unit Point = 0;
      
      ///Defined as 72 points.
      static const Unit Inch = 1;
      
      ///Defined as 1/25.4 inches.
      static const Unit Millimeter = 2;
      
      ///The same size as a point since it has indefinite physical size.
      static const Unit Pixel = 3;

      /**\brief A method to get the conversion ratio for a measurement in
      some 'basic' unit type. \details The Measurement class uses these to
      convert between any two arbitrary units of measurement.*/
      static number ConversionRatio(Unit otherUnits)
      {
        switch(otherUnits)
        {
        case Point: //Basic unit of length
          return 1.0f;
        case Inch:
          return 72.0f;
        case Millimeter:
          return (number)(72.0 / 25.4);
        case Pixel:
          return 1.0f;
        default:
          return 1.0f;
        }
      }
    };

    /**A class for storing vectors with compile-time unit information. The
    measurement class is templated to enforce typing differentiation of
    unlike units, and conversions are automatically done when measurements
    of unlike units are assigned or constructed.*/
    template <Unit T>
    struct Measurement : public math::Vector
    {
      ///Default constructor initializes coordinates with zeroes.
      Measurement() {}

      ///Assigns coordinates in a particular unit of measurement.
      Measurement(number x, number y)
      {
        Measurement::x = x;
        Measurement::y = y;
      }

      /**\brief Creates a coordinate in a particular unit of measurement
      on the unit circle with a specified angle from the x-axis.*/
      Measurement(number Angle)
      {
        Polar(Angle);
      }

      /**Explictly converts from one measurement to another. Usually this
      is not necessary.*/
      template <Unit F>
      void ConvertFrom(const Measurement<F>& m)
      {
        //Calculate the conversion ratio.
        number Multiplier = Units::ConversionRatio(F) /
          Units::ConversionRatio(T);
        //Do the conversion.
        x = m.x * Multiplier;
        y = m.y * Multiplier;
      }

      /**\brief Copy constructor which copies another measurement using
      the local unit of measurement.*/
      template <Unit F>
      Measurement(const Measurement<F>& m)
      {
        ConvertFrom(m);
      }

      /**\brief Assignment operator which copies another measurement using
      the local unit of measurement.*/
      template <Unit F>
      Measurement<T> operator = (const Measurement<F>& m)
      {
        ConvertFrom(m);
        return *this;
      }
    };

    //Common measurements
    typedef Measurement<Units::Inch> Inches;
    typedef Measurement<Units::Millimeter> Millimeters;
    typedef Measurement<Units::Point> Points;
    typedef Measurement<Units::Pixel> Pixels;
  }
}
#endif
