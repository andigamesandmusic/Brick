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

#ifndef primHeap
#define primHeap

#include "primMath.h"
#include "primMemory.h"
#include "primProfiler.h"

namespace prim
{
  /**\brief Heap is a templated heap allocation and garbage collector with
  special emphasis on fast creation of individual elements. \details It solves
  two common annoyances of C++ programs: fragmentation of the heap over time
  and the inconvenience of having to do a manual 'delete' on items allocated
  through the 'new' operator. As you repeatedly call 'new', C++ must try
  harder and harder to find a free area in its hash table. Also, 'new' calls
  are very expensive computationally if you, for example, allocate a number of
  things all at once that are not contiguous.

  Heap allocates memory only in type unit powers of two. This means that as
  more and more units are allocated, Heap calls the 'new' operator less 
  frequently since the allocation curve anticipates growth.

  Working the opposite way, when units are deallocated, Heap saves a linked 
  deletion tree that traverses through the old locations of each deallocated 
  unit. When the largest power-of-two heap has a full deletion tree, it is 
  automatically collapsed. Since Heap stores the linking pointers in the 
  abandoned memory, the unit width must be greater than or equal to the size of 
  the pointer. Otherwise, Heap forces the allocated units to be at least as 
  large as the size of a pointer. Allocating a heap of chars would thus be
  inefficient. Heap works best with larger types, like links in a linked list. 
  Also, when the deletion tree is active, subsequent calls to New() simply pop 
  an item off that stack instead of increasing the heap's size.

  Allocating and deallocating memory works exactly like it does in plain C++
  memory operations. Call New() to get a pointer to the next new item. Note
  that by design, you can not ask for an array of units. If you need the items
  to be contiguous then use an Array.

  You call Delete(T*& ObjectPointer) with a reference to the object
  pointer. Before it deallocates the memory, it will call the destructor of
  the object if there is one, and then it will zero out your pointer.

  You can also instruct Heap to delete all of the subheaps at once. Doing this
  however does not call the destructors of each object, so make sure you are
  calling it for the right reason (i.e. the object's destructor is irrelevant
  because all the memory is being deleted). To delete in this way call
  DeleteAllWithoutDestructor(). This is also the default behavior of the Heap
  destructor, so the memory just needs to be freed then you can let Heap take 
  care of the garbage collection as it goes out of scope.

  To determine the number of elements in the heap call Size(). Note that by
  design you can not iterate through the elements like an array, which is also
  why you can not delete all of the subheaps and simultaneously call each 
  object's destructor. Heap has no direct knowledge of a unit's existence except
  indirectly through the deletion tree. Iterating would essentially be a
  O(n-squared) operation if it existed under this algorithm.

  Heap comes at a cost of 376 bytes on a 32-bit platform plus the amount of
  data allocated rounded up to the nearest power of two. According to 
  statistical tests, the data structure takes up on average 1.4 times the
  amount it would take up as a single array. However, when used without
  destructors, tests indicate that it is up to 50 times faster than using
  regular new/delete operations and with destructors called, it is
  approximately 2 times faster.

  Heap has a few disadvantages over other memory pool management algorithms.
  Firstly, Heap easily inflates, but deflates only when all units in the largest
  subheap have been deleted. Thus, if you were to randomly delete elements, then
  the size of the heap would not deflate until practically units had been 
  deleted.

  Note: if you need statistical information about the memory, you can derive a
  class from Heap and call the protected statistics methods.*/

  template <class T>
  class Heap
  {
  private:


    //CONSTANTS//

    /**\brief UnitAlignment stores the number of bytes in a pointer, so as
    to be independent of 32-bit/64-bit platforms. \details It is computed at
    compile- time. We must determine the size of pointers in bytes, so that
    we can know what the alignment is and so that each unit will be
    guaranteed to take up at least enough space to contain a pointer, so
    that the deletion tree will work.*/
    static const count UnitAlignment = sizeof(void*);

    /**\brief BinaryHeapMaximum contains the physical limit of heaps per the
    build environment (i.e\details . 32-bit or 64-bit). On a 32-bit system
    this will compute to 30, meaning approximately a billion units at 4
    bytes each, which maxes out at 4 GB, as you expect. On a 64-bit system,
    this will compute to 61. Of course the operating system will probably
    deny these requests before this limit is reached. This number is used
    for the static array of heap pointers.*/
    static const count BinaryHeapMaximum = UnitAlignment * (count)8 -
      (count)math::meta::LogBaseTwo<UnitAlignment>::Value;

    /*Whenever memory is allocated or deallocated, each char takes on
    this value.*/
    static const uint8 MemSetDefaultValue = 0xFF;

    /**\brief Stores the size of T taking into account alignment (for
    example if smaller than the width of a pointer, then the size will be
    the width of a pointer.*/
    static const count SizeOfUnit = (count)sizeof(T) < UnitAlignment ?
      UnitAlignment : (count)sizeof(T);


    //INTERNAL DATA//

    /**\brief Number of virtual address spaces made available either as data
    nodes or deletion nodes*/
    count UnitsAllocated;

    ///Number of heaps
    count HeapsAllocated;

    ///Number of units at which a new heap must be created
    count HeapThreshold;

    ///Index of current unit within largest heap
    count HeapUnit;

    ///Physical spaces which contain pre-allocated memory
    void* Heaps[BinaryHeapMaximum];

    ///Deletion trees for each heap
    void* DeletionTreeRoot[BinaryHeapMaximum];

    ///Deletion counts per heap for enabling automatic collapses
    count DeletionCountPerHeap[BinaryHeapMaximum];


    //HELPERS//

    ///Resets all data structures, but does not know to free memory.
    void Initialize(void)
    {
      _profile(primProfiles);
      UnitsAllocated=0;
      HeapUnit=0;
      HeapsAllocated=0;
      HeapThreshold=1;
      for(count i=0;i<BinaryHeapMaximum;i++)
      {
        _profile(primProfiles);
        Heaps[i]=0;
        DeletionTreeRoot[i]=0;
        DeletionCountPerHeap[i]=0;
      }
      _endprofile(primProfiles);
    }

    /**Gets the address range of a particular heap. Note, in practice this is
    the most-called function in the prim library!*/
    inline void GetHeapRange(count HeapIndex,
      void*& LowAddress, void*& HighAddress) const
    {
      _profile(primProfiles);
      //We assume... HeapIndex >= 0 and HeapIndex < HeapsAllocated
      LowAddress = Heaps[HeapIndex];
      HighAddress = (void*)((byte*)LowAddress +
        ((uintptr)SizeOfUnit << (uintptr)HeapIndex));
      _endprofile(primProfiles);
    }

    ///Finds the heap that contains a particular unit.
    void FindIndexLocationFromPointer(void* HeapPointer,
      count& HeapIndex, count& UnitIndex) const
    {
      _profile(primProfiles);
      //Iterate through the heaps and find which one's range includes
      //the pointer in question.
      for(count i=0;i<HeapsAllocated;i++)
      {
        void* HeapRangeLow=0, *HeapRangeHigh=0;
        GetHeapRange(i, HeapRangeLow, HeapRangeHigh);
        if(HeapPointer >= HeapRangeLow && HeapPointer < HeapRangeHigh)
        {
          HeapIndex=i;
          UnitIndex=(*(uintptr*)&HeapPointer-*(uintptr*)&HeapRangeLow)
            / SizeOfUnit;
          _endprofile(primProfiles);
          return;
        }
      }
      /*Can not find the heap. This occurs when the user asks something
      not in any heap to be deleted, which in general should not happen.
      This is cautionary code.*/
      HeapIndex=-1;
      UnitIndex=0;
      _endprofile(primProfiles);
    }

  protected:


    //STATISTICS//

    ///Gets the total number of units that allocated with New().
    count TotalUnitsAssigned(void) const
    {
      return UnitsAllocated;
    }

    ///Gets the total number of unassigned but allocated spaces available.
    count TotalUnitsUnassigned(void) const
    {
      return HeapThreshold-UnitsAllocated-1;
    }

    ///Gets the number of heaps.
    count TotalHeaps(void) const
    {
      return HeapsAllocated;
    }

    ///Returns the number of units at which a new heap will be allocated.
    count CurrentUnitThreshold(void) const
    {
      return HeapThreshold;
    }

    ///Returns the largest heap, from which new allocations are being made.
    count CurrentHeapUnit(void) const
    {
      return HeapUnit;
    }

    ///Returns the number of items in the deletion trees.
    count DeletionTreeSize(void) const
    {
      count DeletionSum=0;
      for(count i=0;i<HeapsAllocated;i++)
        DeletionSum+=DeletionCountPerHeap[i];
      return DeletionSum;
    }

    /**\brief Calculates the amount of excess space as a ratio of the total
    allocated space to the minimum that would be required to fit the data.*/
    number MemorySpaceInefficiency(void) const
    {
      if(CurrentUnitThreshold()>1)
        return (number)(CurrentUnitThreshold()-1) / (number)Size();
      else
        return (number)1.0;
    }

  public:

    ///Constructs Heap with no elements allocated.
    Heap()
    {
      Initialize();
    }

    /**\brief Destroys Heap and automatically frees its memory, but does not
    call the destructors of the still-allocated units.*/
    ~Heap()
    {
      DeleteAllWithoutDestructor();
    }

    /**Requests a pointer to a new unit. There is no method available for
    requesting multiple units at once as Heap is designed for allocating
    non-contiguous units of memory.*/
    T* New(void)
    {
      _profile(primProfiles);
      //Test to see if there are any open spots in the deletion trees.
      for(count i=HeapsAllocated-1;i>=0;i--)
      {
        if(DeletionCountPerHeap[i]>0)
        {
          //Found a previously deleted element, so just use that one.
          DeletionCountPerHeap[i]--;
          T* RecycledUnit=(T*)DeletionTreeRoot[i];

          //Grab the reference to the next deletion tree node.
          DeletionTreeRoot[i]=*(void**)RecycledUnit;

          //Initialize the unit.
          Memory::Set(RecycledUnit, MemSetDefaultValue, SizeOfUnit);

          //Call a placement new.
          _endprofile(primProfiles);
          return Memory::PlacementNew(RecycledUnit);
        }
      }


      if(UnitsAllocated+1==HeapThreshold)
      {
        //Have to make a new heap...
        uintptr HeapBytes=SizeOfUnit*HeapThreshold;

        Heaps[HeapsAllocated]=(void*)new int8[HeapBytes];

        //We need to make sure we actually received the memory from the
        //operating system...
        if(Heaps[HeapsAllocated]==0)
        {
          //Out of memory... return null pointers, and keep everything
          //as is.
          _endprofile(primProfiles);
          return (T*)0;
        }

        UnitsAllocated++;
        HeapUnit=0;
        HeapsAllocated++;
        HeapThreshold*=2;

        //Initialize all the memory.
        Memory::Set(
          Heaps[HeapsAllocated-1],MemSetDefaultValue,HeapBytes);
      }
      else
      {
        UnitsAllocated++;
        HeapUnit++;
      }

      //Calculate the address of the new unit.
      uintptr address=((*(uintptr*)&Heaps[HeapsAllocated-1])
        +SizeOfUnit*HeapUnit);

      //Placement new will call object T's constructor.
      _endprofile(primProfiles);
      return new (*(void**)&address) T;
    }

    /**Requests to delete an object allocated with New(). If the pointer is
    valid then the address will be added to the deletion tree for later
    reallocation later, or if all the units in the largest heap have been
    deleted then that heap will collapse automatically. This method calls
    the object's destructor before destroying the data, just like the
    'delete' method in C++.*/
    void Delete(T*& ObjectPointer)
    {
      _profile(primProfiles);

      count HeapIndex=0, UnitIndex=0;

      if(ObjectPointer==0)
      {
        _endprofile(primProfiles);
        return; //Pointer is null... There's nothing to delete.
      }

      FindIndexLocationFromPointer((void*) ObjectPointer,
        HeapIndex, UnitIndex);
      if(HeapIndex<0)
      {
        //The heap could not be found, so null the pointer anyway and
        //get out of here.
        ObjectPointer=0;
        _endprofile(primProfiles);
        return;
      }

      /*This will call the object's destructor. Weird, but it works since the
      destructor is public.*/
      ObjectPointer->~T();

      //Erase the memory to the default value.
      Memory::Set((void*)ObjectPointer, MemSetDefaultValue, SizeOfUnit);

      //Embed a deletion tree node in the memory of the old data unit.
      *(void**)ObjectPointer=DeletionTreeRoot[HeapIndex];
      DeletionTreeRoot[HeapIndex]=(void*)ObjectPointer;
      DeletionCountPerHeap[HeapIndex]++;
      ObjectPointer=(T*)0;
      for(count i=HeapsAllocated-1;i>=0;i--)
      {
        //Go through each heap starting from the largest and determine
        //if they can be collapsed.
        if(DeletionCountPerHeap[i]==((count)1 << i) ||
          (i==HeapsAllocated-1 &&
          DeletionCountPerHeap[i]==HeapUnit+1))
        {
          DeletionCountPerHeap[i]=0;
          DeletionTreeRoot[i]=0;
          delete [] (int8*)(Heaps[i]);
          Heaps[i]=0;

          /*Don't ask... (basically, UnitsAllocated isn't the total
          number. It's only the rightmost unit in the heap set, so
          once the heap deflates, it needs to basically become the
          last element of the heap before...)*/

          //= 2 ^ i
          HeapThreshold = ((count)1 << i);

          //(2 ^ i) - 1
          UnitsAllocated = ((count)1 << i) - (count)1;

          //= (2 ^ (i-1)) - 1
          HeapUnit = ((count)1 << (i-1)) - (count)1;

          HeapsAllocated--;
        }
        else
        {
          _endprofile(primProfiles);
          return;
        }
      }
      _endprofile(primProfiles);
    }

    /**Deletes all units, effectively clearing out all heaps and all memory.
    No destructors are called, so make sure that you use this method in the
    appropriate context, for example, deleting a list of things. This method
    is very fast, as it only needs to call the C++ 'delete' operator the
    base-two log of the number of units. This method is called
    automatically, if Heap goes out of scope or is destroyed.*/
    void DeleteAllWithoutDestructor(void)
    {
      _profile(primProfiles);
      //Hopefully HeapsAllocated isn't corrupted, but just in case...
      if(HeapsAllocated>0 && HeapsAllocated<BinaryHeapMaximum)
      {
        for(count i=0;i<HeapsAllocated;i++)
        {
          delete [] (int8*) Heaps[i];
        }
      }

      Initialize();
      _endprofile(primProfiles);
    }

    ///Gets the number of units allocated by New().
    count Size(void) const
    {
      return UnitsAllocated-DeletionTreeSize();
    }
  };
}
#endif
