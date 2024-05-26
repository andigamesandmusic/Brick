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

#ifndef primLink
#define primLink

namespace prim
{
  ///Implements a simple forward link.
  template <class T>
  class SingleLink
  {
  private:
    ///Object containing data of the list's type
    T Data;

  public:
    ///Pointer to the next Link, or null if there is none.
    SingleLink<T>* Next;

    ///Default constructor
    SingleLink<T>() : Next(0) {}

    ///Copy constructor
    SingleLink<T>(T& DataToCopy) : Next(0) {Data = DataToCopy;}

    ///Assignment operator
    T operator= (const T& Other)
    {
      Data = Other;
      return Data;
    }

    ///Automatic casting into T
    operator T&()
    {
      return Data;
    }
  };

  ///Implements a simple double link.
  template <class T>
  class DoubleLink
  {
  public:
    ///Object containing data of the list's type
    T Data;

    ///Pointer to the next DoubleLink, or null if there is none.
    DoubleLink<T>* Next;

    ///Pointer to the previous DoubleLink, or null if there is none.
    DoubleLink<T>* Prev;

    ///Default constructor
    DoubleLink<T>() : Next(0), Prev(0) {}

    ///Copy constructor
    DoubleLink<T>(T& DataToCopy) : Next(0), Prev(0) {Data = DataToCopy;}

    ///Assignment operator
    T operator= (const T& Other)
    {
      Data = Other;
      return Data;
    }

    ///Automatic casting into T
    operator T&()
    {
      return Data;
    }
  };
}

#endif
