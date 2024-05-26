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

#ifndef primPath
#define primPath

#include "primList.h"
#include "primVector.h"
#include "primRectangle.h"
#include "primTransform.h"

#include "primStream.h"

namespace prim { namespace math
{
  ///A simple wrapper for a polygon class as an array of vertices.
  struct Polygon : public Array<Vector>
  {
  private:
    struct RadialSortingVector : public Vector
    {
      number theta;
      bool operator < (const RadialSortingVector& Other) const
      {
        return theta < Other.theta;
      }

      bool operator > (const RadialSortingVector& Other) const
      {
        return theta > Other.theta;
      }

      bool operator == (const RadialSortingVector& Other) const
      {
        return theta == Other.theta;
      }
    };

  public:
    mutable Rectangle BoundingBox;

    ///Clears the polygon including its bounding box.
    void Clear(void)
    {
      Array<Vector>::Clear();
      BoundingBox.Clear();
    }

    ///Updates the bounding box of the polygon.
    void UpdateBoundingBox(void) const
    {
      Rectangle r(GetConstItem(0));
      for(count i = 1; i < n(); i++)
        r = r + GetConstItem(i);
      BoundingBox = r;
    }

    ///Determines whether or not this polygon intersects the outline of another.
    bool IntersectsOutline(const Polygon& Other, Vector OtherDisplacement =
      Vector(0, 0)) const
    {
      Rectangle OtherBoundingBoxDisplaced(
        Other.BoundingBox.a + OtherDisplacement,
        Other.BoundingBox.b + OtherDisplacement);

      Rectangle IntersectBox = Rectangle::Intersection(
        BoundingBox, OtherBoundingBoxDisplaced);

      if(BoundingBox.Contains(IntersectBox) ||
        OtherBoundingBoxDisplaced.Contains(IntersectBox))
        return true;
      else if(IntersectBox.IsEmpty())
        return false;

      Vector p1, p2, q1, q2;

      p1 = GetItemValue(n() - 1);
      for(count i = n() - 2; i >= 0; i--)
      {
        p2 = GetItemValue(i);
        q1 = Other.GetItemValue(Other.n() - 1) + OtherDisplacement;
        for(count j = Other.n() - 2; j >= 0; j--)
        {
          q2 = Other.GetItemValue(j) + OtherDisplacement;
          Line P(p1, p2), Q(q1, q2);
          if(P.Intersects(Q))
            return true;
          q1 = q2;
        }
        p1 = p2;
      }

      return false;
    }

    /**Determines the order of three points. The result can be as clockwise (1),
    counterclockwise (-1), or collinear (0).*/
    static inline count DeterminePointOrder(Vector p0, Vector p1, Vector p2)
    {
      number p1x_p0x = p1.x - p0.x;
      number p2x_p0x = p2.x - p0.x;
      number p1y_p0y = p1.y - p0.y;
      number p2y_p0y = p2.y - p0.y;

      number Left  = p2y_p0y * p1x_p0x;
      number Right = p1y_p0y * p2x_p0x;
      
      if(Left < Right)
        return 1;
      else if(Left == Right)
        return 0;
      else
        return -1;
    }

    /**Creates a new polygon consisting of the convex hull of this polygon. The
    algorithm employed is the "three-penny" convex hull finder.*/
    void CreateConvexHull(Polygon& NewPolygon) const
    {
      //Make sure there are at least three points to work with.
      if(n() < 3)
      {
        if(n())
          NewPolygon = *this;
        else
          NewPolygon.Clear();
        return;
      }

      //Copy polygon array into temporary list.
      List<RadialSortingVector> L;
      {
        //First find left-most point.
        count this_n = n();
        count LeftMostIndex = 0;
        number LeftMostX = GetItemValue(0).x;
        for(count i = 1; i < this_n; i++)
        {
          number CurrentX = GetItemValue(i).x;
          if(CurrentX < LeftMostX)
          {
            LeftMostIndex = i;
            LeftMostX = CurrentX;
          }
        }
        
        /*Now add the points with the left-most point first. This guarantees
        that the first three points in the list will be convex.*/
        for(count i = LeftMostIndex; i < this_n; i++)
          static_cast<Vector&>(L.Add()) = GetItemValue(i);
        for(count i = 1; i <= LeftMostIndex; i++)
          static_cast<Vector&>(L.Add()) = GetItemValue(i);

        ///Determine the beginning point order.
        Vector L_0 = L[0];
        while(DeterminePointOrder(L_0, L[1], L[2]) == 0)
          L.Remove(1);

        //Determine the angles of the points so that they can be sorted.
        RadialSortingVector v = L[0];
        count L_n_1 = L.n() - 1;
        for(count i = 1; i < L_n_1; i++)
        {
          number theta = v.Ang(L[i]);
          if(theta > Pi)
            theta -= TwoPi;
          L[i].theta = theta;
        }

        //Force the first point to be first in the list after the sort.
        L[0].theta = -Pi;

        //Force the last point to be last in the list after the sort.
        L[L_n_1].theta = Pi;

        //Sort the vector list radially.
        L.QuickSort();
      }

      //Using the "three-penny" convex hull algorithm.
      for(count i = 0; i < L.n() - 2; i++)
      {
        Vector p0 = L[i], p1 = L[i + 1], p2 = L[i + 2];
        if(DeterminePointOrder(p0, p1, p2) != -1)
        {
          L.Remove(i + 1);
          i -= 2;
        }
      }

      //Copy results into new polygon.
      {
        Rectangle& B = NewPolygon.BoundingBox;
        NewPolygon.Clear();

        Vector L_first = L.first();
        NewPolygon.n(L.n());
        NewPolygon[0] = L_first;
        B = Rectangle(L_first);

        count L_n = L.n();
        for(count i = 1; i < L_n; i++)
        {
          Vector v = L[i];
          NewPolygon[i] = v;
          B = B + v;
        }
      }
    }
  };

  ///Used for simplifying a curve-based path into a bounding polygon.
  struct PolygonPath
  {
    Array<Polygon> Components;

    ///Gets the bounding box around the path.
    math::Rectangle GetBoundingBox(void) const
    {
      math::Rectangle r;

      for(count i = Components.n() - 1; i >= 0; i--)
        r = r + (math::Rectangle)Components.GetConstItem(i).BoundingBox;

      return r;
    }

    ///Determines if this polygon path intersects the outline of another.
    bool IntersectsOutline(const PolygonPath& Other,
      Vector OtherDisplacement = Vector(0, 0)) const
    {
      for(count i = Components.n() - 1; i >= 0; i--)
      {
        const Polygon& p1 = Components.GetConstItem(i);
        for(count j = Other.Components.n() - 1; j >= 0; j--)
        {
          const Polygon& p2 = Other.Components.GetConstItem(j);

          if(p1.IntersectsOutline(p2, OtherDisplacement))
            return true;
        }
      }
      return false;
    }

    ///Create a set of convex hulls for each of the component polygons.
    void CreateConvexHull(PolygonPath& NewPolygonPath) const
    {
      NewPolygonPath.Components.n(0);
      count Components_n = Components.n();
      for(count i = 0; i < Components_n; i++)
        Components.GetConstItem(i).CreateConvexHull(
          NewPolygonPath.Components.AddOne());
    }

    ///Count the total number of line segments in the polygon path.
    count GetLineSegmentCount(void)
    {
      count Result = 0;
      count Components_n = Components.n();
      for(count i = Components_n - 1; i >= 0; i--)
        Result += Components[i].n();
      Result -= Components_n;
      return Result;
    }
  };
}}

namespace prim
{
  /**\brief A Path is a list of path components that collectively use the
  non-zero winding rule to define a closed region which can subsequently be
  filled and/or stroked.*/
  class Path
  {
  public:
    /**Represents a straight-line or a cubic Bezier curve.*/
    struct Curve
    {
      ///The end of the curve.
      math::Vector End;

      ///The first control point, to which the curve will at first travel.
      math::Vector StartControl;

      /**\brief The second control point, from which the curve will
      finally travel.*/
      math::Vector EndControl;

      /**If true, only the End vector has effect and the result is a line.
      Otherwise the control points are used for a cubic Bezier curve.*/
      bool IsCurve;
      Curve() : IsCurve(false) {}
    };

    /**Array of connected lines and curves intended for stroking or filling.*/
    class Component
    {
      ///Friends
      friend class Path;

      ///Stores the component's bounding box.
      math::Rectangle BoundingBox;

    public: //Just for now.

      ///A list of Curves that compose the path Component.
      Array<Curve> Curves;

      ///Determines if this component intersects the outline of another.
      bool IntersectsOutline(const Component& Other,
        math::Vector OtherDisplacement = math::Vector(0, 0)) const
      {
        using namespace prim::math;
        
        _profile(primProfiles);

        //Skip line segment test if bounding boxes do not overlap.
        Rectangle OtherBoundingBoxDisplaced(
          Other.BoundingBox.a + OtherDisplacement,
          Other.BoundingBox.b + OtherDisplacement);

        Rectangle IntersectBox = Rectangle::Intersection(
          BoundingBox, OtherBoundingBoxDisplaced);

        if(BoundingBox.Contains(IntersectBox) &&
          OtherBoundingBoxDisplaced.Contains(IntersectBox))
          return true;
        else if(IntersectBox.IsEmpty())
          return false;

        /*Prepare a small vector list to keep track of the multiple points
        within the Curve object.*/
        Vector p[4], q[4];
        count psegmentcount = 0, qsegmentcount = 0;
        const Array<Curve>& OtherCurves = Other.Curves;
        count thisn = Curves.n(), othern = OtherCurves.n();
        
        //Begin outer loop
        //----------------------------------------------------------------------
        p[0] = Curves.GetConstItem(0).End;
        for(count i = 1; i < thisn; i++)
        {
          const Curve& pcurve = Curves.GetConstItem(i);
          bool pcurveIsCurve = pcurve.IsCurve;
          if(pcurveIsCurve)
          {
            p[1] = pcurve.StartControl;
            p[2] = pcurve.EndControl;
            p[3] = pcurve.End;
            psegmentcount = 3;
          }
          else
          {
            p[1] = pcurve.End;
            psegmentcount = 1;
          }
          
          for(count pi = 0; pi < psegmentcount; pi++)
          {
            Vector p1 = p[pi], p2 = p[pi + 1];

            //Inner loop is functionally identical to outer loop
            //--------------------------------------------------
            q[0] = OtherCurves.GetConstItem(0).End + OtherDisplacement;
            for(count j = 1; j < othern; j++)
            {
              const Curve& qcurve = OtherCurves.GetConstItem(j);
              bool qcurveIsCurve = qcurve.IsCurve;
              if(qcurveIsCurve)
              {
                q[1] = qcurve.StartControl + OtherDisplacement;
                q[2] = qcurve.EndControl + OtherDisplacement;
                q[3] = qcurve.End + OtherDisplacement;
                qsegmentcount = 3;
              }
              else
              {
                q[1] = qcurve.End + OtherDisplacement;
                qsegmentcount = 1;
              }
              
              for(count qj = 0; qj < qsegmentcount; qj++)
              {
                //Line segment intersection loop
                //------------------------------
                Line P(p1, p2), Q(q[qj], q[qj + 1]);
                if(P.Intersects(Q))
                {
                  _endprofile(primProfiles);
                  return true;
                }
                //------------------------------
              }

              if(qcurveIsCurve)
                q[0] = q[3];
              else
                q[0] = q[1];
            }
            //--------------------------------------------------
          }
          if(pcurveIsCurve)
            p[0] = p[3];
          else
            p[0] = p[1];
        }
        //----------------------------------------------------------------------

        _endprofile(primProfiles);
        return false;
      }

      /**Updates the bounding box. This method only needs to be used if the
      curve or line point data is directly edited, or if path components are
      manually added or removed from the path.*/
      math::Rectangle UpdateBoundingBox(void)
      {
        //Clear the old bounding box.
        BoundingBox.Clear();

        //If the component is empty, so is the bounding box.
        if(!Curves.n())
          return BoundingBox;
        
        //Add at least one point so that the union works correctly.
        BoundingBox = math::Rectangle(Curves[0].End);
        
        //Union the bounding box each control point in the path component.
        for(count i = Curves.n() - 1; i >= 0; i--)
        {
          Curve& c = Curves[i];
          if(c.IsCurve)
          {
            BoundingBox = BoundingBox + c.StartControl;
            BoundingBox = BoundingBox + c.EndControl;
            BoundingBox = BoundingBox + c.End;
          }
          else
            BoundingBox = BoundingBox + c.End;
        }

        return BoundingBox;
      }

    public:
      /**Adds a line to the last point in this Component. Automatically updates
      the bounding box.*/
      void AddCurve(math::Vector NextPoint)
      {
        Curves.AddOne().End = NextPoint;
        BoundingBox = BoundingBox + NextPoint;
      }

      /**Adds a cubic Bezier curve to the last point in this Component.
      Automatically updates the bounding box.*/
      void AddCurve(math::Vector StartControl, math::Vector EndControl,
        math::Vector End)
      {
        Curve& c = Curves.AddOne();
        c.StartControl = StartControl;
        c.EndControl = EndControl;
        c.End = End;
        c.IsCurve = true;
        BoundingBox = BoundingBox + StartControl;
        BoundingBox = BoundingBox + EndControl;
        BoundingBox = BoundingBox + End;
      }
      

      /**Adds a cubic Bezier curve directly to this component. It discards the
      starting point however, so be careful to make sure that the curve starts
      from where you intended. Automatically updates the bounding box.*/
      void AddCurve(math::Bezier& b)
      {
        using namespace math;
        Vector p0;
        Vector p1;
        Vector p2;
        Vector p3;

        b.GetControlPoints(p0, p1, p2, p3);

        AddCurve(p1, p2, p3);
      }
      
      
      /**Adds a quadratic Bezier curve to the last point in this Component. The
      quadratic is first up-converted to a cubic (with no loss of precision).
      Automatically updates the bounding box.*/
      void AddCurve(math::Vector Control, math::Vector End)
      {
        math::Bezier b;
        b.SetControlPoints(Curves.last().End, Control, End);
        AddCurve(b);
      }
    };

  public: //Just for now.

    ///Array of path components.
    Array<Component> Components;

    /**An array of transform matrix contexts. A path containing no contexts is
    the same as a path containing an identity matrix. Contexts can be used to
    draw multiple transformations of a path simultaneously without needing to
    copy the path data multiple times. Renderers can use this information to
    speed up the drawing of canvases that have many of the same shape.*/
    Array<math::AffineMatrix> Contexts;
    
  public:
    ///Empties the path of all its components.
    void Clear(void)
    {
      Components.Clear();
    }

    ///Adds a component to the path and sets its starting point.
    void AddComponent(math::Vector Start)
    {
      Components.AddOne().AddCurve(Start);
      Components.last().BoundingBox = math::Rectangle(Start);
    }

    /**Adds a component to the path without setting its starting point. Use the
    AddCurve method of the path or the component to set the starting point.*/
    void AddComponent(void)
    {
      Components.AddOne();
    }

    /**Adds a line to the last component of the path. This method also returns
    the length of the line.*/
    number AddLine(math::Vector NextPoint)
    {
      using namespace prim::math;

      Path::Component& Component = Components.last();

      Component.AddCurve(NextPoint);

      count Lines = Component.Curves.n();
      if(Lines <= 1)
        return 0;
      else
      {
        Vector Start = Component.Curves[Lines - 2].End;
        return Dist(Start.x, Start.y, NextPoint.x, NextPoint.y);
      }
    }

    /**Adds a line to the last component of the path. Make sure to add a
    component first.*/
    void AddCurve(math::Vector NextPoint)
    {
      Components.last().AddCurve(NextPoint);
    }
    
    /**Adds a curve to the last component of the path. Make sure to add a
    component first.*/
    void AddCurve(math::Vector Control, math::Vector End)
    {
      Components.last().AddCurve(Control, End);
    }

    /**Adds a cubic curve to the last component of the path. Make sure to add a
    component first.*/
    void AddCurve(math::Vector StartControl, math::Vector EndControl,
      math::Vector End)
    {
      Components.last().AddCurve(StartControl, EndControl, End);
    }

    /**Adds a cubic Bezier curve directly to this path. It discards the starting
    point however, so be careful to make sure that the curve starts from where
    you intended.*/
    void AddCurve(math::Bezier& b)
    {
      using namespace prim::math;

      Vector p0, p1, p2,p3;
      b.GetControlPoints(p0, p1, p2, p3);
      AddCurve(p1, p2, p3);
    }

    ///Adds a circular arc with one or more bezier curves to the last component.
    number AddArc(number Radius, number StartAngleDegrees,
      number EndAngleDegrees, math::Vector Origin = math::Vector(0, 0))
    {
      using namespace prim::math;

      Path::Component& Component = Components.last();

      Vector Scale(Radius * 2.0f, Radius * 2.0f);

      number StartAngleRadians = StartAngleDegrees * DegToRad;
      number EndAngleRadians = EndAngleDegrees * DegToRad;

      while(StartAngleRadians < 0)
      {
        StartAngleRadians += TwoPi;
        EndAngleRadians += TwoPi;
      }
      while(StartAngleRadians >= TwoPi)
      {
        StartAngleRadians -= TwoPi;
        EndAngleRadians -= TwoPi;
      }

      if(StartAngleRadians < EndAngleRadians)
      {
        /*Normalize so that angle is essentially beginning at 0, but all curves
        are rotated by StartAngleDegrees.*/
        EndAngleRadians -= StartAngleRadians;
        number Rotation = StartAngleRadians;
        count Q = 1;
        for(number r = 0; r < EndAngleRadians - (number)0.00001; r += HalfPi)
        {
          Bezier b = Ellipse::toSpline(Origin, Scale, Rotation, Q, true);
          if(EndAngleRadians - r < HalfPi)
            b.Trim(0, (EndAngleRadians - r) / HalfPi);
          Component.AddCurve(b);
          Q++;
        }
      }
      else
      {
        /*Normalize so that angle is essentially beginning at 0, but all curves
        are rotated by StartAngleDegrees.*/
        Swap(StartAngleRadians, EndAngleRadians);
        StartAngleRadians -= EndAngleRadians;
        number Rotation = EndAngleRadians;
        count Q = 4;
        for(number r = 0; r > StartAngleRadians + (number)0.00001; r -= HalfPi)
        {
          Bezier b = Ellipse::toSpline(Origin, Scale, Rotation, Q, false);
          if(r - StartAngleRadians < HalfPi)
            b.Trim(0, (r - StartAngleRadians) / HalfPi);
          Component.AddCurve(b);
          Q--;
        }
      }

      return Abs(EndAngleDegrees - StartAngleDegrees) * DegToRad * Radius;
    }

    ///Adds a polygon as a set of line segments.
    void AddPolygon(const math::Polygon& p)
    {
      if(!p.n())
        return;

      AddComponent(p.GetConstItem(0));
      for(count i = 1; i < p.n(); i++)
        AddCurve(p.GetConstItem(i));
    }

    ///Adds a polygon path as a set of line segments.
    void AddPolygonPath(const math::PolygonPath& pp)
    {
      for(count i = 0; i < pp.Components.n(); i++)
        AddPolygon(pp.Components.GetConstItem(i));
    }

    /**Gets the raw bounding box around the path data. Bounding boxes are cached
    at the component level. This method creates a union of those sub-boundaries
    and returns the result.*/
    math::Rectangle GetRawBoundingBox(void) const
    {
      math::Rectangle r;

      for(count i = Components.n() - 1; i >= 0; i--)
        r = r + Components.GetConstItem(i).BoundingBox;

      return r;
    }
    
    /**Returns the bounding box of a given context. If -1 is passed for the 
    context index, then the bounding box of all the contexts together is
    calculated. If there are no contexts, then the raw bounding box of the path
    data is returned. Note that if context contains non-trivial rotation (not a
    multiple of 90 degrees), then the bounding box must be calculated from
    scratch which is a little slower. If you must take the bounding box of a
    rotated context, then be sure to save the calculation so that it does not
    need to be recalculated.*/
    math::Rectangle GetBoundingBox(count ContextIndex = -1) const
    {
      if(ContextIndex == -1)
      {
        if(!Contexts.n())
          return GetRawBoundingBox();
        else
        {
          math::Rectangle r;
          r = GetBoundingBox(0);
          for(count i = 1; i < Contexts.n(); i++)
            r = r + GetBoundingBox(i);
          return r;
        }
      }
      else if(ContextIndex >= 0 && ContextIndex < Contexts.n())
      {
        math::Rectangle r = GetRawBoundingBox();
        math::AffineMatrix& c = Contexts[ContextIndex];
        if(!c.HasNonTrivialRotation())
        {
          //Just work on the cached bounding box.
          math::Vector p1 = c << r.a;
          math::Vector p2 = c << r.b;
          math::Rectangle BoundingBox(p1, p2);
          BoundingBox.Order();
          return BoundingBox;
        }
        else
        {
          math::Rectangle r;
          
          //Get the first point so that the rectangle is valid.
          if(Components.n() && Components[0].Curves.n())
            r = math::Rectangle(c << Components[0].Curves[0].End);
          
          //Add all the points now to the bounding box.
          for(count i = Components.n() - 1; i >= 0; i--)
          {
            const Component& comp = Components[i];
            for(count j = comp.Curves.n() - 1; j >= 0; j--)
            {
              const Curve& curv = comp.Curves[j];
              if(curv.IsCurve)
              {
                r = r + (c << curv.StartControl);
                r = r + (c << curv.EndControl);
                r = r + (c << curv.End);
              }
              else
                r = r + (c << curv.End);
            }
          }
          return r;
        }
      }
      else
        return math::Rectangle();
    }

    ///Returns the polygon path in line segments from the control points.
    void GetPolygonPathOutline(math::PolygonPath& pp,
      math::Vector Translation = math::Vector(0, 0)) const
    {
      _profile(primProfiles);

      using namespace prim::math;

      pp.Components.n(0);
      count Components_n = Components.n();
      for(count i = 0; i < Components_n; i++)
      {
        Polygon& p = pp.Components.AddOne();
        Component& c = Components[i];
        Array<Curve>& c_Curves = c.Curves;
        count c_Curves_n = c_Curves.n();
        for(count j = 0; j < c_Curves_n; j++)
        {
          Curve& c_Curves_j = c_Curves[j];

          if(c_Curves_j.IsCurve)
          {
            p.AddOne() = c_Curves_j.StartControl + Translation;
            p.AddOne() = c_Curves_j.EndControl + Translation;
          }
          p.AddOne() = c_Curves_j.End + Translation;
        }
        
        if(p[0] != p.last())
          p.AddOne() = p[0];

        c.UpdateBoundingBox();
        p.BoundingBox.a = c.BoundingBox.a + Translation;
        p.BoundingBox.b = c.BoundingBox.b + Translation;
      }
      _endprofile(primProfiles);
    }

    ///Determines if this path intersects the outline of another.
    bool IntersectsOutline(const Path& Other,
      math::Vector OtherDisplacement = math::Vector(0, 0)) const
    {
      _profile(primProfiles);
      count Components_n = Components.n();
      count Other_Components_n = Other.Components.n();
      for(count i = 0; i < Components_n; i++)
      {
        const Component& c1 = Components.GetConstItem(i);
        for(count j = 0; j < Other_Components_n; j++)
        {
          const Component& c2 = Other.Components.GetConstItem(j);

          if(c1.IntersectsOutline(c2, OtherDisplacement))
          {
            _endprofile(primProfiles);
            return true;
          }
        }
      }
      _endprofile(primProfiles);
      return false;
    }

    //Operators
    const Path& operator += (math::Vector v)
    {
      for(count i = Components.n() - 1; i >=0; i--)
      {
        Component& Components_i = Components[i];
        Array<Curve>& Curves = Components_i.Curves;
        for(count j = Curves.n() - 1; j >= 0; j--)
        {
          Curve& Curves_j = Curves[j];
          Curves_j.End += v;
          if(Curves_j.IsCurve)
          {
            Curves_j.StartControl += v;
            Curves_j.EndControl += v;
          }
        }
        Components_i.BoundingBox.a += v;
        Components_i.BoundingBox.b += v;
      }
      return *this;
    }

    const Path& operator -= (math::Vector v)
    {
      v *= -1.0f;
      *this += v;
      return *this;
    }

    const Path& operator *= (number k)
    {
      for(count i = Components.n() - 1; i >=0; i--)
      {
        Component& Components_i = Components[i];
        Array<Curve>& Curves = Components_i.Curves;
        for(count j = Curves.n() - 1; j >= 0; j--)
        {
          Curve& Curves_j = Curves[j];
          Curves_j.End *= k;
          if(Curves_j.IsCurve)
          {
            Curves_j.StartControl *= k;
            Curves_j.EndControl *= k;
          }
        }
        Components_i.BoundingBox.a *= k;
        Components_i.BoundingBox.b *= k;
      }
      return *this;
    }
  };
}

#endif
