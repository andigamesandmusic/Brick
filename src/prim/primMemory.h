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

#ifndef primMemory
#define primMemory

#include "primTypes.h"

/*Normally, we would include memory.h, however doing so introduces the entire
C standard library into the global namespace, which is what we're trying to
avoid. Thankfully, we can just include new.h to take care of the placement new
operation that we need to be templated.*/
#include <new>

namespace prim
{
  ///Memory is a wrapper for clearing/copying memory via templated methods.
  class Memory
  {
  public:
    /**\brief Wrapper for memset (prevents memory.h from being included
    externally) */
    static void Set(
      void* Destination, uint8 ValueToSet, count BytesToSet);

    /**\brief Wrapper for memcpy (prevents memory.h from being included
    externally) */
    static void Copy(
      void* Destination, void* Source, count BytesToCopy);

    /**Clears out a single object with a particular value. Warning: do not use
    use this to initialize a virtual object as doing this destroys the virtual
    pointer table and causes the application to crash when the object is
    deleted.*/
    template <class Type>
    static void ClearObject(Type& Object, uint8 Value=0)
    {
      Set((void*)&Object,Value,sizeof(Type));
    }

    ///Clears out an array of objects with a particular value.
    template <class Type>
    static void ClearArray(Type* Object, count Items, uint8 Value=0)
    {
      Set((void*)Object,Value,Items*sizeof(Type));
    }

    ///Copies an object to another memory region.
    template <class Type>
    static void CopyObject(Type& Destination, const Type& Source)
    {
      Copy((void*)&Destination,(void*)&Source,sizeof(Type));
    }

    ///Copies an array of objects to another memory region.
    template <class Type>
    static void CopyArray(Type* Destination, const Type* Source, count Items)
    {
      Copy((void*)Destination,(void*)Source,Items*sizeof(Type));
    }

    ///Calls a placement new on an object.
    template <class Type>
    static Type* PlacementNew(Type* Object)
    {
      return new (Object) Type;
    }
  };
}

#endif
