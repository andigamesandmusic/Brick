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

#ifndef primAuto
#define primAuto

namespace prim
{
  /**\brief A simple auto-pointer.
  
  This does nothing except delete the object when it goes out of scope.
  Auto-pointers are bound for their lifetime so you can not change what they
  point to after they are set to something. The default constructor will
  automatically create the object (assuming that object also has a valid default
  constructor). You can also assign the object after the constructor by
  initially specifying null for the constructor.*/
  template<class T>
  class Auto
  {
    mutable T* PointerToObject;
  public:
    Auto()
    {
      PointerToObject = new T;
    }

    Auto(T* newObject)
    {
      PointerToObject = newObject;
    }

    void operator = (T* newObject) const
    {
      if(!PointerToObject)
        PointerToObject = newObject;
    }

    T& operator * (void) const
    {
      return *PointerToObject;
    }

    T* operator -> (void) const
    {
      return PointerToObject;
    }

    operator T*(void) const
    {
      return PointerToObject;
    }

    ~Auto()
    {
      delete PointerToObject;
    }
  };
}
#endif
