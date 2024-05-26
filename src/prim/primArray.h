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

#ifndef primArray
#define primArray

#include "primMath.h"
#include "primMemory.h"
#include "primTypes.h"

namespace prim
{
  /**\brief A smart array with exponential (base-two) element creation.
  
  Since reallocations only occur at the base-two increments (1 unit, 2 units,
  4 units, 8 units, etc.) very little time is spent overall doing memory
  copies.*/
  template <class T>
  class Array
  {
    void* Data;
    count LogicalSize;

    static count CalculatePhysicalSizeFromLogical(count Logical)
    {
      //Return 0 if bad argument.
      if(Logical < 0)
        return 0;

      //If 1 or 2, then Physical = Logical
      if(Logical <= 2)
        return Logical;

      /*If 3 or higher calculate the physical size by finding the least power of
      two greater than the logical size.*/
      count Physical = 2;
      do
      {
        Physical *= 2;
        if(Logical <= Physical)
          return Physical;
      }
      while(Physical > 0 &&
        (uintptr)Physical < math::meta::HighestBitOfType<void*>::Value);

      //Overflow returns zero.
      return 0;
    }

  public:
    ///Returns the size of the array.
    inline count n(void) const
    {
      return LogicalSize;
    }

    ///Sets the size of the array.
    T* n(count NewLogicalSize)
    {
      //In the case of a no size array, just delete all the data.
      if(NewLogicalSize <= 0)
      {
        //Call all the destructors.
        for(count i = 0; i < LogicalSize; i++)
          ((T*)Data)[i].~T();
        delete [] (byte*)Data;
        Data = 0;
        LogicalSize = 0;
        return 0;
      }

      /*Since this class uses an exponential expansion model, the size will only
      increase at powers of two. This means in exchange for using up a little
      more memory, there are much fewer actual allocations.*/
      count OldPhysicalSize = CalculatePhysicalSizeFromLogical(LogicalSize);
      count NewPhysicalSize = CalculatePhysicalSizeFromLogical(NewLogicalSize);

      /*If no extra data needs to be allocated, then just change the size of the
      array.*/
      if(OldPhysicalSize == NewPhysicalSize)
      {
        //Zero out any previously uninitialized elements.
        if(NewLogicalSize > LogicalSize)
        {
          Memory::Set((void*)&((T*)Data)[LogicalSize], 0, 
          sizeof(T) * (NewLogicalSize - LogicalSize));

          //Call the constructors of the new objects.
          for(count i = LogicalSize; i < NewLogicalSize; i++)
            Memory::PlacementNew(&((T*)Data)[i]);
        }
        else if(NewLogicalSize < LogicalSize)
        {
          //Call the destructors for the objects that are being removed.
          for(count i = NewLogicalSize; i < LogicalSize; i++)
            ((T*)Data)[i].~T();
        }
        
        //Set the new size of the array.
        LogicalSize = NewLogicalSize;
        return (T*)Data;
      }

      //Allocate a new contiguous block of memory.
      void* NewData = (void*)new byte[NewPhysicalSize * sizeof(T)];

      //If memory could not be allocated, return zero.
      if(!NewData)
        return 0;

      //Test to see if the array is getting bigger or smaller.
      if(NewLogicalSize > LogicalSize)
      {
        //Copy original data and zero out the new portion.
        Memory::CopyArray((byte*)NewData, (byte*)Data, 
          LogicalSize * sizeof(T));
        Memory::Set((void*)&((T*)NewData)[LogicalSize], 0, 
          sizeof(T) * (NewLogicalSize - LogicalSize));
        for(count i = LogicalSize; i < NewLogicalSize; i++)
          Memory::PlacementNew(&((T*)NewData)[i]);
      }
      else //Assume that NewLogicalSize is less than LogicalSize.
      {
        //Call the destructors of the objects being removed.
        for(count i = NewLogicalSize; i < LogicalSize; i++)
          ((T*)Data)[i].~T();

        //Copy data into a smaller array.
        Memory::CopyArray((byte*)NewData, (byte*)Data, 
          NewLogicalSize * sizeof(T));
      }

      //Delete old array.
      delete [] (byte*)Data;

      //Set the new size of the array.
      LogicalSize = NewLogicalSize;

      //Return pointer to the new data array.
      return (T*)(Data = NewData);
    }

    ///Clears the array.
    inline void Clear(void)
    {
      n(0);
    }

    ///Adds an element to the array.
    void Add(const T& NewElement)
    {
      n(LogicalSize + 1);
      ((T*)Data)[LogicalSize - 1] = NewElement;
    }

    ///Returns the last element of the array.
    inline T& last(void)
    {
      return ((T*)Data)[LogicalSize - 1];
    }

    ///Adds one element to the array and a reference to that element.
    T& AddOne(void)
    {
      n(LogicalSize + 1);
      return last();
    }

    /**Returns an element by index. The method does not check bounds before
    attempting to access the data. The element itself has read-write access.*/
    inline T& operator[](count Index) const
    {
      return ((T*)Data)[Index];
    }

    /**Returns an element by index. The method does not check bounds before
    attempting to access the data. The element itself has read-write access.*/
    inline T& GetItem(count Index) const
    {
      return ((T*)Data)[Index];
    }

    /**Returns an element read-only by index. The method does not check bounds
    before attempt to access the data.*/
    inline const T& GetConstItem(count Index) const
    {
      return (const T&)(((T*)Data)[Index]);
    }
    
    /**Returns the value of an element given its index. Changing the value does
    not change the array since the value returned is a copy.*/
    inline T GetItemValue(count Index) const
    {
      return ((T*)Data)[Index];
    }

    ///Constructs the array with no elements.
    Array() : Data(0), LogicalSize(0) {}

    ///Constructs the array with some number of elements.
    Array(count Size) : Data(0), LogicalSize(0) {n(Size);}

    /**Copys another array into this one. A copy operation is performed on each
    element to preserve construction and destruction of complex objects.*/
    void CopyFrom(const Array<T> &Other)
    {
      if(&Other == this)
        return;
      n(0);
      LogicalSize = Other.LogicalSize;
      Data = (void*)new byte[CalculatePhysicalSizeFromLogical(LogicalSize) * 
        sizeof(T)];
      for(count i = 0; i < LogicalSize; i++)
      {
        Memory::PlacementNew(&(((T*)Data)[i]));
        ((T*)Data)[i] = ((T*)Other.Data)[i];
      }
    }

    /**Copy constructor (use sparingly). Normally you would want to use 
    references to a Array rather than copy the whole thing.*/
    Array(const Array<T> &Other) : Data(0), LogicalSize(0) {CopyFrom(Other);}
    
    /**Copy constructor from array data. Does not call items' constructors.*/
    Array(const T* OtherData, count Length) : Data(0), LogicalSize(0)
    {
      n(Length);
      Memory::CopyArray((T*)Data, OtherData, Length);
    }

    ///Copys an array to another of the same type.
    Array<T>& operator=(const Array<T>& Other)
    {
      CopyFrom(Other);
      return *this;
    }

    ///Destroys the array.
    ~Array() {n(0);}
  };
}
#endif
