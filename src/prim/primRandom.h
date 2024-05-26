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

#ifndef primRandom
#define primRandom

#include "primTypes.h"

namespace prim
{
  /**A random number generator using the multiply-with-carry algorithm. This
  algorithm produces consistently uniform data and has an extremely long period
  somewhere on the order of 2^64. This particular version of the number 
  generator has been evaluated with the "Dieharder" random number generator test 
  battery and performs extremely well (see results below). The generator
  defaults to seeding with the system clock, so you only need to seed it if you
  want to be able to reproduce a given random sequence.

  A summary page of the results of the Dieharder analysis are as follows:

  \verbatim
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: POSSIBLY WEAK at < 5% for RGB Bit Distribution Test
  Recommendation:  Repeat test to verify failure.
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Bit Distribution Test
  Assessment: PASSED at > 5% for RGB Generalized Minimum Distance Test
  Assessment: PASSED at > 5% for RGB Generalized Minimum Distance Test
  Assessment: PASSED at > 5% for RGB Generalized Minimum Distance Test
  Assessment: PASSED at > 5% for RGB Generalized Minimum Distance Test
  Assessment: PASSED at > 5% for RGB Permutations Test
  Assessment: POSSIBLY WEAK at < 5% for RGB Permutations Test
  Recommendation:  Repeat test to verify failure.
  Assessment: PASSED at > 5% for RGB Permutations Test
  Assessment: PASSED at > 5% for RGB Permutations Test
  Assessment: PASSED at > 5% for RGB Permutations Test
  Assessment: PASSED at > 5% for RGB Permutations Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: POOR at < 1% for RGB Lagged Sum Test
  Recommendation:  Repeat test to verify failure.
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: POSSIBLY WEAK at < 5% for RGB Lagged Sum Test
  Recommendation:  Repeat test to verify failure.
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: POSSIBLY WEAK at < 5% for RGB Lagged Sum Test
  Recommendation:  Repeat test to verify failure.
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: FAILED at < 0.01% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: POSSIBLY WEAK at < 5% for RGB Lagged Sum Test
  Recommendation:  Repeat test to verify failure.
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: POOR at < 1% for RGB Lagged Sum Test
  Recommendation:  Repeat test to verify failure.
  Assessment: FAILED at < 0.01% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: FAILED at < 0.01% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for RGB Lagged Sum Test
  Assessment: PASSED at > 5% for Diehard Birthdays Test
  Assessment: PASSED at > 5% for Diehard 32x32 Binary Rank Test
  Assessment: PASSED at > 5% for Diehard 6x8 Binary Rank Test
  Assessment: PASSED at > 5% for Diehard Bitstream Test
  Assessment: PASSED at > 5% for Diehard OPSO
  Assessment: PASSED at > 5% for Diehard OQSO Test
  Assessment: PASSED at > 5% for Diehard DNA Test
  Assessment: PASSED at > 5% for Diehard Count the 1s (stream) Test
  Assessment: PASSED at > 5% for Diehard Count the 1s Test (byte)
  Assessment: POSSIBLY WEAK at < 5% for Diehard Parking Lot Test
  Recommendation:  Repeat test to verify failure.
  Assessment: PASSED at > 5% for Diehard Minimum Distance (2d Circle) Test
  Assessment: PASSED at > 5% for Diehard 3d Sphere (Minimum Distance) Test
  Assessment: PASSED at > 5% for Diehard Squeeze Test
  Assessment: PASSED at > 5% for Diehard Runs Test
  Assessment: PASSED at > 5% for Diehard Runs Test
  Assessment: PASSED at > 5% for Diehard Craps Test
  Assessment: PASSED at > 5% for Diehard Craps Test
  Assessment: POSSIBLY WEAK at < 5% for Marsaglia and Tsang GCD Test
  Recommendation:  Repeat test to verify failure.
  Assessment: PASSED at > 5% for Marsaglia and Tsang GCD Test
  Assessment: PASSED at > 5% for STS Monobit Test
  Assessment: PASSED at > 5% for STS Runs Test
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: POSSIBLY WEAK at < 5% for STS Serial Test (Generalized)
  Recommendation:  Repeat test to verify failure.
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for STS Serial Test (Generalized)
  Assessment: PASSED at > 5% for Lagged Sum Test
\endverbatim
*/
  class Random
  {
    ///Stores the state of the random number generator.
    uint32 History[5];

    ///Accesses the system clock in order to seed the generator.
    uint32 GetSystemTicks(void);
  public:
    //Returns the next raw 32-bit random number.
    uint32 Next(void)
    {
      uint64 Sum = (uint64)2111111111UL * (uint64)History[3] + //
                   (uint64)1492 * (uint64)(History[2]) +       //
                   (uint64)1776 * (uint64)(History[1]) +       //
                   (uint64)5115 * (uint64)(History[0]) +       //
                   (uint64)History[4];                         //

      History[3] = History[2];
      History[2] = History[1];
      History[1] = History[0];
      History[4] = (uint32)(Sum >> (uint64)32);
      History[0] = (uint32)Sum;

      return History[0];
    }

    ///Returns a random signed integer.
    integer NextInteger(void)
    {
      uint64 RandomInteger = ((uint64)Next() << (uint64)32) + (uint64)Next();
      return (integer)RandomInteger;
    }

    /**Returns a nonnegative random integer. This can be effective if you
    subsequently trim the range using a modulo of a power of two (i.e. strip
    upper bits). Do not use modulos that are not powers of two or else you will
    end up with some integers being over or underpreferred especially at high 
    modulos. To get a range of integers use NextIntegerInRange() which uses 
    high-precision floating-point numbers to smoothly calculate a random number
    in a given range.*/
    integer NextNonnegativeInteger(void)
    {
      integer RandomSignedInteger = NextInteger();
      if(RandomSignedInteger < 0)
        RandomSignedInteger = (integer)-1 - RandomSignedInteger;
      return RandomSignedInteger;
    }

    /**Returns a random 64-bit float in the interval [1.0, 2.0). The number is
    raw in that it has not yet been touched by any floating-point arithmetic. It
    is calculated by fixing the floating-point exponent and dumping random bits
    into the mantissa.*/
    float64 NextRawFloat64(void)
    {
      float64 Float = 0;
      uint64& BinaryFloat = *(uint64*)(float64*)&Float;
      BinaryFloat = ((uint64)1023 << (uint64)52);
      uint64 RandomMantissa = ((uint64)Next() << (uint64)32) + (uint64)Next();
      RandomMantissa = RandomMantissa >> (uint64)12;
      BinaryFloat += RandomMantissa;
      return Float;
    }

    /**Returns a random number in the interval [0.0, 1.0). This method is not as
    theoretically precise as NextRawFloat64() since it must use floating-point
    subtraction to transpose the interval range down by 1.0.*/
    number NextNumber(void)
    {
      return (number)(NextRawFloat64() - (float64)1.0);
    }

    /**Returns a random integer in a particular range. The order of the 
    arguments does not make a difference. Note that the upper integer is 
    excluded. In other words the interval is [Low, High).*/
    integer NextIntegerInRange(integer RangeBound1, integer RangeBound2 = 0)
    {
      //If bounds are equal, return the bound.
      if(RangeBound1 == RangeBound2)
        return RangeBound1;

      //Determine floating point range.
      float64 Low, High;
      integer LowInteger, HighInteger;
      if(RangeBound1 < RangeBound2)
      {
        Low = (float64)RangeBound1;
        LowInteger = RangeBound1;
        High = (float64)RangeBound2;
        HighInteger = RangeBound2;
      }
      else
      {
        Low = (float64)RangeBound2;
        LowInteger = RangeBound2;
        High = (float64)RangeBound1;
        HighInteger = RangeBound1;
      }

      //Generate a random number in the interval [1.0, 2.0).
      float64 Float = NextRawFloat64();

      //Set the range.
      Float = Float * High - Float * Low - High + (float64)2.0 * Low;

      //Convert to integer.
      integer Integer = (integer)Float;

      //Clamp the range in case of any floating-point arithmetic errors.
      if(Integer < LowInteger)
        Integer = LowInteger;
      else if(Integer >= HighInteger)
        Integer = HighInteger - 1;
      
      return Integer;
    }

    /**Returns a random number in a particular range. The order of the arguments
    does not make a difference. Note that the upper number is excluded. In other
    words the interval is [Low, High).*/
    number NextNumberInRange(number RangeBound1, number RangeBound2 = 0)
    {
      //Determine floating point range.
      float64 Low, High;
      if(RangeBound1 < RangeBound2)
      {
        Low = (float64)RangeBound1;
        High = (float64)RangeBound2;
      }
      else
      {
        Low = (float64)RangeBound2;
        High = (float64)RangeBound1;
      }

      //Generate a random number in the interval [1.0, 2.0).
      float64 Float = NextRawFloat64();

      //Set the range.
      Float = Float * High - Float * Low - High + (float64)2.0 * Low;

      //Clamp the range in case of floating-point arithmetic errors.
      if(Float < Low)
        Float = Low;
      if(Float > High)
        Float = High;
      return (number)Float;
    }

    ///Pick a random sequence using a 32-bit seed.
    void PickSequence(uint32 Seed)
    {
      //Use some arithmetic jibberish to generate the initial seeds.
      for(count i = 0; i < 5; i++)
      {
        Seed *= 29943829;
        Seed -= 1;
        History[i] = Seed;
      }
      
      //Step through the sequence a bit before actually using the values.
      for(count i = 0; i < 100; i++)
        Next();
    }

    ///Picks a random sequence automatically using the system clock.
    void PickRandomSequence(void)
    {
      PickSequence(GetSystemTicks());
    }

    ///Initialize the random number generator with the system clock.
    Random()
    {
      PickRandomSequence();
    }
    
    ///Initialize the random number generator with a 32-bit seed.
    Random(uint32 Seed)
    {
      PickSequence(Seed);
    }
  };
}

#endif
