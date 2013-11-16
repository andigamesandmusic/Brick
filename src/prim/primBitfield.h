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

#ifndef bbsBitfield_h
#define bbsBitfield_h

#include "primTypes.h"

namespace prim
{
  ///A 32-bit bitfield untyped bitfield.
  class Bitfield
  {
    ///Stores the bitfields bits.
    uint32 Bits;

  protected:
    inline void Write(uint32 BitIndex, uint32 BitsToTake, uint32 Value)
    {
      uint32 Masks[33] = {0, //0
        1,3,7,15,31,63,127,255, //1-8
        511,1023,2047,4095,8191,16383,32767,65535, //9-16
        131071,262143,524287,1048575,2097151,4194303,8388607,16777215, //17-24
        33554431,67108863,134217727,268435455,536870911,1073741823,2147483647,
          0xFFFFFFFF}; //25-31

      //Operator Key
      // |   Inclusive OR
      // ^   Exclusive OR (T + T = F)
      // &   AND
      // <<  Shift bits UP by some number
      // >>  Shift bits DOWN by some number

      //WRITE Algorithm
      //Efficiency: 2 OR, 2 AND, 1 BITSHIFT = 5 bitwise operations
      Bits = 
      ( //1-out anything in the mask area and leave the rest untouched, 
        //i.e. rrrrrrr11rrr
        Bits 

        |

        (
          Masks[BitsToTake] << BitIndex  //COMPILER OPTIMIZED CONSTANT
        )
      )

      &  

      ( //i.e. 111111vv111, where v is an arbitrary data value
        (
          ( //Prevent overflow errors by masking bits higher than the range
            Value 
            & 
            Masks[BitsToTake] //COMPILER OPTIMIZED CONSTANT
          )
          <<
          BitIndex
        )

        | 

        ( //The following creates an 'inverse' of the transposed mask, 
          //i.e. 11111100111
          Masks[sizeof(prim::uint32) << 3] 
          ^
          (
            Masks[BitsToTake] << BitIndex
          )
        ) //COMPILER OPTIMIZED CONSTANT
      );
    }

    inline uint32 Read(uint32 BitIndex = 0, uint32 BitsToTake = 32)
    {
      uint32 Masks[33] = {0, //0
        1,3,7,15,31,63,127,255, //1-8
        511,1023,2047,4095,8191,16383,32767,65535, //9-16
        131071,262143,524287,1048575,2097151,4194303,8388607,16777215, //17-24
        33554431,67108863,134217727,268435455,536870911,1073741823,2147483647,
          0xFFFFFFFF}; //25-31

      //Operator Key
      // |   Inclusive OR
      // ^   Exclusive OR (T + T = F)
      // &   AND
      // <<  Shift bits UP by some number
      // >>  Shift bits DOWN by some number

      //READ Algorithm
      //Efficiency: 1 BITSHIFT, 1 AND = 2 bitwise operations
      return (Bits >> BitIndex) & Masks[BitsToTake];
    }

    inline bool ReadBool(uint32 BitIndex)
    {
      return (Read(BitIndex, 1) == 1);
    }

    inline void WriteBool(uint32 BitIndex, bool BoolValue)
    {
      if(BoolValue)
        Write(BitIndex, 1, 1);
      else
        Write(BitIndex, 1, 0);
    }

    inline integer ReadSignedInteger(uint32 BitIndex, uint32 TotalBits)
    {
      if(ReadBool(BitIndex + TotalBits - 1))
        return -((integer)Read(BitIndex, TotalBits - 1));
      else
        return Read(BitIndex, TotalBits - 1);
    }

    inline void WriteSignedInteger(uint32 BitIndex, uint32 TotalBits,
      integer Value)
    {
      if(Value < 0)
      {
        WriteBool(BitIndex + TotalBits - 1, true);
        Write(BitIndex, TotalBits - 1, (uint32)(-Value));
      }
      else
      {
        WriteBool(BitIndex + TotalBits - 1, false);
        Write(BitIndex, TotalBits - 1, (uint32)Value);
      }
    }

  public:
    Bitfield(){Bits = 0;}
    ~Bitfield(){Bits = 0;}

    bool operator == (Bitfield Other)
    {
      return (Bits == Other.Bits);
    }

    bool operator != (Bitfield  Other)
    {
      return (Bits != Other.Bits);
    }
  };

  //----------------------//
  //Bitfield Type Template//
  //----------------------//

  typedef int32 BitfieldType;

  /**A typed bitfield.
  
  Uses the upper 8 bits to store a BitfieldType, and uses the lower 24 bits to
  store data.*/
  template <BitfieldType T>
  class TypedBitfield : public Bitfield
  {
  protected:
    static const BitfieldType Type = T;
    
  public:
    TypedBitfield(void)
    {
      Write(24, 8, T); //Set upper 24 bits to the new type.
      WriteBool(23, true); //Assume that the bitfield is valid to begin with.
    }

    BitfieldType ReadType(void)
    {
      return Read(24, 8);
    }

    bool Valid(void)
    {
      return (Read(23, 1) == 1);
    }

    void Valid(bool IsValid)
    {
      if(IsValid)
        WriteBool(23, true);
      else
        Write(0, 24, 0); //This erases all of the information except the type.
    }
  };
}

#endif
