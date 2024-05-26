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

#ifndef primEndian
#define primEndian

#include "primTypes.h"

namespace prim
{
  /*A class for check endianness at run-time and making the appropriate
  conversions.*/
  class Endian
  {
  public:

    ///Returns true if the system is little-endian (low-to-high).
    static inline bool IsLittleEndian(void)
    {
      static bool haveCheckedEndianness = false;
      static bool cachedLittleEndianness = false;

      if(!haveCheckedEndianness)
      {
        //Create a two-byte number and initialize it to one. If the
        //first byte contains the one then, it's little, otherwise it's
        //big-endian.
        uint16 a = 1;
        if(*((uint8*)&a) == 1)
          cachedLittleEndianness = true;
        else
          cachedLittleEndianness = false;
        haveCheckedEndianness = true;
      }
      return cachedLittleEndianness;
    }

    ///Reverses the byte order of any complex data type.
    template <class T>
    static void ReverseOrder(T& value)
    {
      T new_value=0;
      uint8* old_ptr = (uint8*)&value;
      uint8* new_ptr = (uint8*)&new_value;

      for(count i = 0; i < (count)sizeof(T); i++)
        new_ptr[i] = old_ptr[sizeof(T) - i - 1];

      value = new_value;
    }

    ///Ensures that a value is stored in little-endian.
    template <class T>
    static void ConvertObjectToLittleEndian(T& units)
    {
      if(!IsLittleEndian())
        ReverseOrder(units);
    }

    ///Ensures that a value is stored in big-endian.
    template <class T>
    static void ConvertObjectToBigEndian(T& units)
    {
      if(IsLittleEndian())
        ReverseOrder(units);
    }

    ///Ensures that an array of things is stored in little-endian.
    template <class T>
    static void ConvertArrayToLittleEndian(T* units, count unit_count)
    {
      if(!IsLittleEndian())
        for(count i = 0; i < unit_count; i++)
          ReverseOrder(units[i]);
    }

    ///Ensures that an array of things is stored in big-endian.
    template <class T>
    static void ConvertArrayToBigEndian(T* units, count unit_count)
    {
      if(IsLittleEndian())
        for(count i = 0; i < unit_count; i++)
          ReverseOrder(units[i]);
    }
  };
}
#endif
