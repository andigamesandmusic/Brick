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

#ifndef primRectangle
#define primRectangle

#include "primVector.h"

namespace prim
{
  namespace math
  {
    /**Stores a rectangle as a pair of opposite vectors. A non-empty rectangle
    is defined to be one that has a non-zero area (and thus has four right 
    angles). An ordered rectangle is a non-empty rectangle with ordered
    coordinates such that a is the bottom-left corner, and b is the top-right
    corner.*/
    struct Rectangle
    {

      /**Location of the first of two opposite corners of a rectangle.*/
      Vector a;

      /**Location of the second of two opposite corners of a rectangle.*/
      Vector b;

      //------------//
      //Constructors//
      //------------//
      
      //Default constructor creates an invalid rectangle.
      Rectangle() {}

      ///Creates a collapsed rectangle at a single point.
      Rectangle(Vector p) : a(p), b(p) {}

      ///Creates a rectangle from a pair of minimum and maximum vectors.
      Rectangle(Vector p1, Vector p2) : a(p1), b(p2) {}

      ///Creates a rectangle from the coordinates of its four sides.
      Rectangle(number x1, number y1, number x2, number y2) :
        a(x1, y1), b(x2, y2) {}
      
      //---------//
      //Emptiness//
      //---------//

      /**Returns true if the rectangle is empty. A rectangle is empty if (and
      only if) its area is zero. If either the width or height is zero, the
      rectangle is empty. An empty rectangle is mathematically an invalid one
      since it contains undefined angles instead of right angles.*/
      inline bool IsEmpty(void) const
      {
        return a.x == b.x || a.y == b.y;
      }
      
      /**Clears a rectangle by reinitializing its coordinates to zero. The
      rectangle returns to an invalid and non-ordered state.*/
      inline void Clear(void)
      {
        a = b = Vector();
      }

      //-----//
      //Order//
      //-----//
      
      /**Ensures that the coordinates are in ascending order. IsOrdered() will
      still return false if either the width or height is zero.*/
      void Order(void)
      {
        Ascending(a.x, b.x);
        Ascending(a.y, b.y);
      }
      
      /**Returns whether or not the two points a and b are in ascending order.
      The rectangle must also be non-empty (have non-zero area) to be ordered.*/
      bool IsOrdered(void) const
      {
        return a.x < b.x && a.y < b.y;
      }

      /**Returns the positive difference of the x-coordinates. An invalid
      rectangle may have a non-zero width.*/
      inline number Width(void) const
      {
        return Abs(b.x - a.x);
      }

      /**Returns the positive difference of the y-coordinates. An invalid
      rectangle may have a non-zero height.*/
      inline number Height(void) const
      {
        return Abs(b.y - a.y);
      }
      
      /**Returns the positive or negative difference of the x-coordinates. An
      invalid rectangle may have a non-zero width.*/
      inline number ActualWidth(void) const
      {
        return b.x - a.x;
      }
      
      /**eturns the positive or negative difference of the y-coordinates. An
      invalid rectangle may have a non-zero height.*/
      inline number ActualHeight(void) const
      {
        return b.y - a.y;
      }

      /**Returns the postive area of the rectangle. A rectangle has non-zero
      area if (and only if) it is valid.*/
      inline number Area(void) const
      {
        return Abs(ActualWidth() * ActualHeight());
      }

      /**Returns the positive perimeter of the rectangle. An invalid rectangle
      may have a non-zero perimeter if one of its sides is non-zero.*/
      inline number Perimeter(void) const
      {
        return (Width() + Height()) * (number)2.0;
      }

      ///Returns the coordinate of the left side.
      inline number Left(void) const {return Min(a.x, b.x);}

      ///Returns the coordinate of the bottom side.
      inline number Bottom(void) const {return Min(a.y, b.y);}

      ///Returns the coordinate of the right side.
      inline number Right(void) const {return Max(a.x, b.x);}

      ///Returns the coordinate of the top side.
      inline number Top(void) const {return Max(a.y, b.y);}

      ///Returns the coordinate of the rectangle's bottom-left corner.
      inline Vector BottomLeft(void) const
      {
        return Vector(Left(), Bottom());
      }
      
      ///Returns the coordinate of the rectangle's bottom-right corner.
      inline Vector BottomRight(void) const
      {
        return Vector(Right(), Bottom());
      }
      
      ///Returns the coordinate of the rectangle's top-left corner.
      inline Vector TopLeft(void) const
      {
        return Vector(Left(), Top());
      }
      
      ///Returns the coordinate of the rectangle's top-right corner.
      inline Vector TopRight(void) const
      {
        return Vector(Right(), Top());
      }
      
      ///Returns the coordinate of the rectangle's center.
      inline Vector Center(void) const
      {
        return Vector((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
      }

      ///Sets the coordinate of the left side.
      inline void Left(number Left)
      {
        a.x = Left;
      }

      ///Sets the coordinate of the bottom side.
      inline void Bottom(number Bottom)
      {
        a.y = Bottom;
      }

      ///Sets the coordinate of the right side.
      inline void Right(number Right)
      {
        b.x = Right;
      }

      ///Sets the coordinate of the top side.
      inline void Top(number Top)
      {
        b.y = Top;
      }

      /**Sets the coordinate of the rectangle's bottom-left corner.*/
      inline void BottomLeft(Vector p)
      {
        a = p;
      }
      
      /**Sets the coordinate of the rectangle's bottom-right corner.*/
      inline void BottomRight(Vector p)
      {
        b.x = p.x;
        a.y = p.y;
      }
      
      /**Sets the coordinate of the rectangle's top-left corner.*/
      inline void TopLeft(Vector p)
      {
        a.x = p.x;
        b.y = p.y;
      }
      
      /**Sets the coordinate of the rectangle's top-right corner.*/
      inline void TopRight(Vector p)
      {
        b = p;
      }

      /**Returns true if a particular point is contained by the rectangle. If
      the rectangle is collapsed, the point can still be contained even though
      the rectangle is empty.*/
      inline bool Contains(Vector p) const
      {
        return p.x >= Left() && p.y >= Bottom() &&
                 p.x <= Right() && p.y <= Top();
      }

      /**Returns whether or not a particular point is contained by the
      rectangle. If the rectangle is collapsed, the point can still be contained
      even though the rectangle is empty.*/
      inline bool Contains(const Rectangle& r) const
      {
        return Contains(r.a) && Contains(r.b);
      }

      /**Returns whether or not a particular point is inside the rectangle. If
      the rectangle is empty, it returns false.*/
      inline bool IsPointInside(Vector p) const
      {
        return !IsEmpty() && Contains(p);
      }

      /**Subtracts an amount to 'a', and adds an equivalent amount to 'b'. For
      positive values, the rectangle is guaranteed to always gets bigger,
      regardless of the ordering of the corners; for negative values with
      magnitude less than both the width and height of the rectangle, the 
      rectangle gets smaller, and for more negative values the result is 
      undefined.*/
      void Dilate(number Amount)
      {
        number SgnOfdx =  ActualWidth() < 0 ? -1.0f : 1.0f;
        number SgnOfdy = ActualHeight() < 0 ? -1.0f : 1.0f;
        number SgnOfdxTimesAmount = SgnOfdx * Amount;
        number SgnOfdyTimesAmount = SgnOfdy * Amount;
        
        a.x -= SgnOfdxTimesAmount;
        b.x += SgnOfdxTimesAmount;
        a.y -= SgnOfdyTimesAmount;
        b.y += SgnOfdyTimesAmount;
      }
      
      /**Computes the intersection of two rectangles. If either rectangle is
      empty then an empty rectangle is returned.*/
      static Rectangle Intersection(Rectangle r1, Rectangle r2)
      {
        if(r1.IsEmpty() || r2.IsEmpty())
          return Rectangle(); //Return an empty rectangle.
        
        r1.Order();
        r2.Order();

        number tblx = r1.a.x;
        number oblx = r2.a.x;
        number tbly = r1.a.y;
        number obly = r2.a.y;
        number ttrx = r1.b.x;
        number otrx = r2.b.x;
        number ttry = r1.b.y;
        number otry = r2.b.y;

        number nblx = 0;
        number nbly = 0;
        number ntrx = 0;
        number ntry = 0;

        //X-dimension
        if(tblx <= oblx && oblx <= ttrx)
        {
          nblx = oblx;
          if(otrx <= ttrx)
          {
            //CASE: OTHER is completely contained by THIS.
            ntrx = otrx;
          }
          else
          {
            //CASE: Only OTHER's minimum is contained by THIS.
            ntrx = ttrx;
          }
        }
        else if(tblx <= otrx && otrx <= ttrx)
        {
          //CASE: Only OTHER's maximum is contained by THIS.
          nblx = tblx;
        }
        else if(oblx <= tblx && tblx <= otrx && oblx <= ttrx && ttrx <= otrx)
        {
          //CASE: THIS is completely contained by OTHER.
          nblx = tblx;
          ntrx = ttrx;
        }
        else
        {
          //CASE: No intersection.
          nblx = 0;
          ntrx = 0;
        }

        //Y-dimension
        if(tbly <= obly && obly <= ttry)
        {
          nbly = obly;
          if(otry <= ttry)
          {
            //CASE: OTHER is completely contained by THIS.
            ntry = otry;
          }
          else
          {
            //CASE: Only OTHER's minimum is contained by THIS.
            ntry = ttry;
          }
        }
        else if(tbly <= otry && otry <= ttry)
        {
          //CASE: Only OTHER's maximum is contained by THIS.
          nbly = tbly;
        }
        else if(obly <= tbly && tbly <= otry && obly <= ttry && ttry <= otry)
        {
          //CASE: THIS is completely contained by OTHER.
          nbly = tbly;
          ntry = ttry;
        }
        else
        {
          //CASE: No intersection.
          nbly = 0;
          ntry = 0;
        }

        return Rectangle(Vector(nblx, nbly), Vector(ntrx, ntry));
      }
      
      /*Finds the union of two rectangles, or bounding rectangle. If one is
      empty, the other is returned. If both are empty, the result is an
      empty rectangle.*/
      static Rectangle Union(Rectangle r1, Rectangle r2)
      {
        bool r1Empty = r1.IsEmpty();
        bool r2Empty = r2.IsEmpty();
        
        r1.Order();
        r2.Order();
        
        if(r1Empty && r2Empty)
          return Rectangle(); //Return an empty rectangle.
        else if(r1Empty)
          return r2;
        else if(r2Empty)
          return r1;
        
        number tblx = r1.a.x;
        number oblx = r2.a.x;
        number tbly = r1.a.y;
        number obly = r2.a.y;
        number ttrx = r1.b.x;
        number otrx = r2.b.x;
        number ttry = r1.b.y;
        number otry = r2.b.y;
        
        Vector a(Min(tblx, oblx), Min(tbly, obly));
        Vector b(Max(ttrx, otrx), Max(ttry, otry));
        
        return Rectangle(a, b);
      }

      /*Finds the bounding rectangle of a rectangle and a point. This method
      does not check to ensure that the rectangle is valid beforehand, so it 
      is possible to do a union of a collapsed rectangle of zero area and
      another vector to get the bounding rectangle of the two points.*/
      static Rectangle Union(Rectangle r1, Vector p1)
      {        
        number tblx = r1.a.x;
        number tbly = r1.a.y;
        number ttrx = r1.b.x;
        number ttry = r1.b.y;
        number ox = p1.x;
        number oy = p1.y;
        
        Vector a(Min(tblx, ox), Min(tbly, oy));
        Vector b(Max(ttrx, ox), Max(ttry, oy));
        
        return Rectangle(a, b);
      }

      /**Finds the intersection of two rectangles. The intersection is a 
      commutative operation (the order of arguments does not matter).*/
      Rectangle operator - (const Rectangle& Other) const
      {
        return Intersection(*this, Other);
      }

      /**Finds the union of two rectangles. The union is a commutative
      operation (the order of arguments does not matter).*/
      Rectangle operator + (const Rectangle& Other) const
      {
        return Union(*this, Other);
      }

      /**Calculates the union of a rectangle and a point.*/
      Rectangle operator + (const Vector& Other) const
      {
        return Union(*this, Other);
      }

      ///Determines if this rectangle has the same vertices as another.
      bool operator == (const Rectangle& Other) const
      {
        Rectangle r1 = *this, r2 = Other;
        r1.Order();
        r2.Order();
        return r1.a == r2.a && r1.b == r2.b;
      }
    };
  }
}

#endif
