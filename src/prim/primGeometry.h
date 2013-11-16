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

#ifndef primGeometry
#define primGeometry

#include "primMath.h"
#include "primProfiler.h"
#include "primRectangle.h"
#include "primTypes.h"
#include "primVector.h"

namespace prim
{
  namespace math
  {
    ///A line segment represented by two coordinates.
    struct Line
    {
      ///The first coordinate.
      Vector a;
      
      ///The second coordinate.
      Vector b;

      ///Default constructor.
      Line() {}

      ///Initializing constructor with four individual coordinate values.
      Line(number x1, number y1, number x2, number y2) :
        a(x1, y1), b(x2, y2) {}

      ///Initializing constructor with two vectors.
      Line(Vector p1, Vector p2) : a(p1), b(p2) {}

      ///Returns the length of the segment ab.
      number Length(void) const
      {
        return Dist(a.x, a.y, b.x, b.y);
      }

      ///Returns the angle in radians from a to b.
      number Angle(void) const
      {
        return (b - a).Ang();
      }

      ///Makes a line segment parallel to this one with the same length.
      Line MakeParallelLine(number Displacement) const
      {
        number LineAngle = (b - a).Ang();
        number DisplacementAngle = LineAngle + HalfPi;
        Vector DisplacementVector(DisplacementAngle, Displacement, false);
        return Line(a + DisplacementVector, b + DisplacementVector);
      }

      /*An asymptoteless line-line intersection algorithm that works equally
      well for all configurations of lines by using polar coordinates.*/
      Vector GetPolarLineIntersection(const Line& OtherLine) const
      {
        //Make copies of the parameters so we can modify them.
        Vector Start1 = a, End1 = b, Start2 = OtherLine.a, End2 = OtherLine.b;

        //Rotate the system such that line 1 is horizontal.
        number InitialAngle = (b - a).Ang();
        Start1.Ang(Start1.Ang() - InitialAngle);
        End1.Ang(End1.Ang() - InitialAngle);
        Start2.Ang(Start2.Ang() - InitialAngle);
        End2.Ang(End2.Ang() - InitialAngle);

        /*At this point Start1.y = End1.y; transpose such that line 1 becomes
        the x-axis.*/
        Vector InitialTransposition = Vector(0, Start1.y);
        Start1 -= InitialTransposition;
        End1 -= InitialTransposition;
        Start2 -= InitialTransposition;
        End2 -= InitialTransposition;

        //Now solve for x-axis intersection of line 2.
        Vector Delta = End2 - Start2;

        if(Delta.x == 0 && Delta.y == 0)
          return Vector(0, 0); //We have a point for a line.
        else if(Delta.y == 0)
          return Vector(0, 0); //We have perfectly parallel lines.

        //Initialize intersection to origin; then just find x-component.
        Vector Intersection(0, 0);
        
        //For lines that are more horizontal than vertical...
        if(Abs(Delta.y) < Abs(Delta.x))
        {
          /*
          Solve x-axis intersection using:
            0 = (dy/dx)x + b
            x = -b / (dy/dx) at y = 0
          with:
            b = y - (dy/dx)x
          */
          number dyOverdx = Delta.y / Delta.x;
          number b = Start2.y - dyOverdx * Start2.x;
          Intersection.x = -b / dyOverdx;
        }
        else //for lines that are as or more vertical than horizontal...
        {
          /*
          Solve x-axis intersection directly:
            x = (dx/dy)y + c
          with:
            c = x - (dx/dy)y
          */
          number dxOverdy = Delta.x / Delta.y;
          number c = Start2.x - dxOverdy * Start2.y;
          Intersection.x = c;
        }

        //Undo the transformations from the beginning.
        Intersection += InitialTransposition;
        Intersection.Ang(Intersection.Ang() + InitialAngle);
        
        return Intersection;
      }

      /**Calculates a line-line intersection quickly. The method assumes that
      a single intersection exists.*/
      Vector GetLineIntersection(const Line& OtherLine) const
      {
        _profile(primProfiles);

        // a1 * x + b1 * y + c1 = 0 is line 1
        number a1 = b.y - a.y;
        number b1 = a.x - b.x;
        number c1 = b.x * a.y - a.x * b.y;

        // a2 * x + b2 * y + c2 = 0 is line 2
        number a2 = OtherLine.b.y - OtherLine.a.y;
        number b2 = OtherLine.a.x - OtherLine.b.x;
        number c2 = OtherLine.b.x * OtherLine.a.y - 
          OtherLine.a.x * OtherLine.b.y;

        number Determinant = a1 * b2 - a2 * b1;
        if(!Abs(Determinant))
        {
          //Lines do not intersect or are the same line.
          _endprofile(primProfiles);
          return Vector(0, 0);
        }
        
        number x = (b1 * c2 - b2 * c1) / Determinant;
        number y = (a2 * c1 - a1 * c2) / Determinant;

        Vector Intersection(x, y);
        
        _endprofile(primProfiles);
        return Intersection;
      }

      /**Returns the clockwise order of three points. Returns 1 if the points
      are clockwise; returns -1 if the points are counterclockwise; returns 0
      if the points are collinear.*/
      static inline bool DetermineClockwiseOrder(
        Vector p0, Vector p1, Vector p2)
      {
        return (p2.y - p0.y) * (p1.x - p0.x) < (p1.y - p0.y) * (p2.x - p0.x);
      }

      ///Determines whether or not this line intersects with another one.
      inline bool Intersects(const Line& OtherLine) const
      {
        _profile(primProfiles);

        number Pax = a.x, Pay = a.y, Pbx = b.x, Pby = b.y;
        number Qax = OtherLine.a.x, Qay = OtherLine.a.y,
          Qbx = OtherLine.b.x, Qby = OtherLine.b.y;

        number Pbx_Pax = Pbx - Pax;
        number Qax_Pax = Qax - Pax;
        number Qbx_Pax = Qbx - Pax;
        number Pby_Pay = Pby - Pay;
        number Qay_Pay = Qay - Pay;
        number Qby_Pay = Qby - Pay;      
        number Qax_Pbx = Qax - Pbx;
        number Qbx_Pbx = Qbx - Pbx;
        number Qay_Pby = Qay - Pby;
        number Qby_Pby = Qby - Pby;

        bool Result = (Qby_Pay * Qax_Pax < Qay_Pay * Qbx_Pax) !=
        (Qby_Pby * Qax_Pbx < Qay_Pby * Qbx_Pbx) &&
        (Qay_Pay * Pbx_Pax < Pby_Pay * Qax_Pax) !=
        (Qby_Pay * Pbx_Pax < Pby_Pay * Qbx_Pax);
        
        _endprofile(primProfiles);
        return Result;
      }
    };

    ///Represents a cubic-spline Bezier curve by its polynomial coefficients.
    struct Bezier
    {
      /*
      Coefficients describing the curve. The curve is defined as a
      polynomial for each dimension:
        f(t){x, y} = {A*t^3 + B*t^2 + C*t + D, E*t^3 + F*t^2 + G*t + H}
      */

      number a;
      number b;
      number c;
      number d;
      number e;
      number f;
      number g;
      number h;

      /**Normally a Bezier curve is defined between the points 0<=t<=1. Of
      course the actual curve is valid at all t. Trim allows you to select
      a new finite domain, so that a different selection of the curve can
      be transposed to the effective domain between zero and one. The
      selection can be greater than or less than the original domain.*/
      void Trim(number tZero, number tOne)
      {
        //Compute the new range.
        number newDelta = tOne - tZero;
        number _a = newDelta;
        number _b = tZero;

        /**\brief Substituting in a function f(x) = a*x + b into the
        parametric curve, solves for the new constants. \details This is
        a generalization of de Casteljau's algorithm which bisects the
        curve.*/
        number a2 = _a * _a;
        number a3 = a2 * _a;

        number b2 = _b * _b;
        number b3 = b2 * _b;

        number newA = a3 * a;
        number newB = 3.0f * a2 * _b * a + a2 * b;
        number newC = 3.0f * _a * b2 * a + 2.0f * _a * _b * b + _a * c;
        number newD = b3 * a + b2 * b + _b * c + d;

        number newE = a3 * e;
        number newF = 3.0f * a2 * _b * e + a2 * f;
        number newG = 3.0f * _a * b2 * e + 2.0f * _a * _b * f + _a * g;
        number newH = b3 * e + b2 * f + _b * g + h;

        //Make the assignments.
        a = newA;
        b = newB;
        c = newC;
        d = newD;

        e = newE;
        f = newF;
        g = newG;
        h = newH;
      }

      /**Creates a cubic Bezier from the requested control points. The
      curve begins at p0 and ends at p3. The points p1 and p2 are
      intermediate points of influence which typically do not lie on
      curve.*/
      void SetControlPoints(Vector p0, Vector p1, Vector p2, Vector p3)
      {
        a = p3.x - p0.x + 3.f * (p1.x - p2.x);
        b = 3.f * (p2.x - 2.f * p1.x + p0.x);
        c = 3.f * (p1.x - p0.x);
        d = p0.x;
        e = p3.y - p0.y + (number)3.f * (p1.y - p2.y);
        f = 3.f * (p2.y - (number)2.f * p1.y + p0.y);
        g = 3.f * (p1.y - p0.y);
        h = p0.y;
      }

      /**Creates a cubic Bezier from two anchors and just one control 
      point. The curve begins at p0 and ends at p2. The point p1 is an 
      intermediate point of influence which typically does not lie on the
      curve.*/
      void SetControlPoints(Vector p0, Vector p1, Vector p2)
      {
        a = 0;
        b = p2.x - (number)2.f * p1.x + p0.x;
        c = (number)2.f * (p1.x - p0.x);
        d = p0.x;
        e = 0;
        f = p2.y - (number)2.f * p1.y + p0.y;
        g = (number)2.f * (p1.y - p0.y);
        h = p0.y;
      }

      /**\brief Returns the control points by converting the coefficients
      into x and y coordinates.*/
      void GetControlPoints(Vector& p0, Vector& p1, Vector& p2, Vector& p3)
      {
        p0.x = d;
        p1.x = d + c / 3.f;
        p2.x = d + (number)2 / (number)3 * c + b / 3.f;
        p3.x = d + c + b + a;
        p0.y = h;
        p1.y = h + g / 3.f;
        p2.y = h + (number)2 / (number)3 * g + f / 3.f;
        p3.y = h + g + f + e;
      }

      /**Attempts to return a bezier curve parallel to this one. The start and
      end control points may not be the same.*/
      Bezier MakeQuasiParallelCurve(number Displacement)
      {
        Vector p0, p1, p2, p3;
        Vector q0, q1, q2, q3;
        GetControlPoints(p0, p1, p2, p3);
        number StartNormalAngle = (p1 - p0).Ang() + HalfPi;
        number EndNormalAngle = (p3 - p2).Ang() + HalfPi;
        q0 = p0 + Vector(StartNormalAngle, Displacement, false);
        q3 = p3 + Vector(EndNormalAngle, Displacement, false);
        number Scale = Line(q0, q3).Length() / Line(p0, p3).Length();
        q1 = q0 + (p1 - p0) * Scale;
        q2 = q3 + (p2 - p3) * Scale;
        Bezier bezier;
        bezier.SetControlPoints(q0, q1, q2, q3);
        return bezier;
      }

      ///Returns the coordinate of the cubic spline for some t.
      Vector Value(number t)
      {
        Vector v;

        number t2 = t * t;
        number t3 = t * t2;

        v.x = a * t3 + b * t2 + c * t + d;
        v.y = e * t3 + f * t2 + g * t + h;

        return v;
      }

      /**\brief Finds the curve's vertical tangents by computing the
      derivative of the x-polynomial and finding the zeroes.*/
      count VerticalTangents(number& Root1, number& Root2)
      {
        number _a = (number)3.0 * a;
        number _b = (number)2.0 * b;
        number _c = c;

        count NumberOfRoots = math::Roots(_a, _b, _c, Root1, Root2);

        /*Assure that we only get roots in the 0 to 1 range of the
        Bezier curve.*/
        if(NumberOfRoots == 2)
        {
          if(Root1 >= (number)0.0 && Root1 <= (number)1.0)
          {
            if(Root2 >= (number)0.0 && Root2 <= (number)1.0)
            {
              return 2;
            }
            else
            {
              Root2 = 0;
              return 1;
            }
          }
          else if(Root2 >= (number)0.0 && Root2 <= (number)1.0)
          {
            Root1 = Root2;
            Root2 = 0;
            return 1;
          }
          else
          {
            Root1 = 0;
            Root2 = 0;
            return 0;
          }
        }
        else if(NumberOfRoots == 1)
        {
          if(Root1 >= (number)0.0 && Root1 <= (number)1.0)
          {
            return 1;
          }
          else
          {
            Root1 = 0;
            return 0;
          }
        }
        
        return 0;
      }

      /**\brief Finds the curve's horizontal tangents by computing the
      derivative of the y-polynomial and finding the zeroes.*/
      count HorizontalTangents(number& Root1, number& Root2)
      {
        number _a = (number)3.0 * e;
        number _b = (number)2.0 * f;
        number _c = g;

        count NumberOfRoots = math::Roots(_a, _b, _c, Root1, Root2);

        /*Assure that we only get roots in the 0 to 1 range of the
        Bezier curve.*/
        if(NumberOfRoots == 2)
        {
          if(Root1 >= (number)0.0 && Root1 <= (number)1.0)
          {
            if(Root2 >= (number)0.0 && Root2 <= (number)1.0)
            {
              return 2;
            }
            else
            {
              Root2 = 0;
              return 1;
            }
          }
          else if(Root2 >= (number)0.0 && Root2 <= (number)1.0)
          {
            Root1 = Root2;
            Root2 = 0;
            return 1;
          }
          else
          {
            Root1 = 0;
            Root2 = 0;
            return 0;
          }
        }
        else if(NumberOfRoots==1)
        {
          if(Root1 >= (number)0.0 && Root1 <= (number)1.0)
          {
            return 1;
          }
          else
          {
            Root1 = 0;
            return 0;
          }
        }
      }

      /**Fits a Bezier curve to a quadrant of an ellipse. Just four cubic
      splines can approximate any ellipse to an error of just one part per
      thousand, which is really convenient for drawing circles that just
      need to look like circles. Even at full screen size, it will only be
      off by about a pixel in some places. Good enough for now!*/
      void Ellipse(Vector Origin, Vector Scale, number Rotation,
        count Quadrant, bool isCounterClockwise)
      {
        /*This is the so-called magic number, which is the distance the
        perpendicular influence points must be from anchors in order to
        produce a curve that is most like a circle/ellipse.*/
        const number FourSplineMagicNumber = (number)0.551784;

        number Width = Scale.x * (number)0.5;
        number Height = Scale.y * (number)0.5;

        Vector p0;
        Vector p1;
        Vector p2;
        Vector p3;

        Quadrant = (count)Mod(Quadrant, 4);
        if(Quadrant == 0)
          Quadrant=4;

        if(Quadrant == 3 || Quadrant == 4)
        {
          Rotation += Pi;
          Quadrant -= 2;
        }

        if(Quadrant == 1)
        {
          p0.Polar(Rotation, Width);
          p3.Polar(Rotation + HalfPi, Height);

          p1.Polar(Rotation + HalfPi, Height * FourSplineMagicNumber);
          p1 += p0;

          p2.Polar(Rotation, Width * FourSplineMagicNumber);
          p2 += p3;
        }
        else if(Quadrant == 2)
        {
          p0.Polar(Rotation + HalfPi, Height);
          p3.Polar(Rotation + Pi, Width);

          p1.Polar(Rotation + Pi, Width * FourSplineMagicNumber);
          p1 += p0;

          p2.Polar(Rotation + HalfPi, Height * FourSplineMagicNumber);
          p2 += p3;
        }

        p0 += Origin;
        p1 += Origin;
        p2 += Origin;
        p3 += Origin;

        if(isCounterClockwise)
          SetControlPoints(p0, p1, p2, p3);
        else
          SetControlPoints(p3, p2, p1, p0);
      }

      number FindSimpleYRoot(number tBisectStart = 0,
        number tBisectEnd = (number)1.0)
      {
        count i = 0;
        while(tBisectEnd - tBisectStart > (number)0.00001 && i < 100)
        {
          i++;
          number v0 = Value(tBisectStart).y;
          number tBisect = (tBisectStart + tBisectEnd) * (number)0.5;
          number v1 = Value(tBisect).y;
          number v2 = Value(tBisectEnd).y;
          if((v0 < v1 && v0 <= 0 && v1 >= 0) || 
            (v0 >= v1 && v1 <= 0 && v0 >= 0))
              tBisectEnd = tBisect;
          else if((v1 < v2 && v1 <= 0 && v2 >= 0) || 
            (v1 >= v2 && v2 <= 0 && v1 >= 0))
              tBisectStart = tBisect;
          else
            return 0;
        }
        return (tBisectStart + tBisectEnd) * (number)0.5;
      }

      number FindLineIntersection(Line l)
      {
        number Rotation = -l.Angle();

        /*Rotate and vertically translate the line and curve such that the line
        is horizontal and use Newton's method to solve for the y-root.*/
        Vector p0, p1, p2, p3;
        GetControlPoints(p0, p1, p2, p3);
        l.a.Ang(l.a.Ang() + Rotation);
        l.b.Ang(l.b.Ang() + Rotation);
        p0.Ang(p0.Ang() + Rotation);
        p1.Ang(p1.Ang() + Rotation);
        p2.Ang(p2.Ang() + Rotation);
        p3.Ang(p3.Ang() + Rotation);

        number Translation = -l.a.y;
        p0.y += Translation;
        p1.y += Translation;
        p2.y += Translation;
        p3.y += Translation;

        Bezier b_root; b_root.SetControlPoints(p0, p1, p2, p3);
        return b_root.FindSimpleYRoot();
      }

      void SplitBezier(Bezier& Left, Bezier& Right)
      {
        Left = *this;
        Right = *this;
        Left.Trim(0, 0.5f);
        Right.Trim(0.5f, 1.0f);
      }

      number CalculateArcLength(number Tolerance = 0.001f)
      {
        Vector v0, v1, v2, v3;
        GetControlPoints(v0, v1, v2, v3);
        number PolygonLength = Dist(v0, v1) + Dist(v1, v2) + Dist(v2, v3);
        number ChordLength = Dist(v0, v3);
        number CurrentError = PolygonLength - ChordLength;
        if(CurrentError > Tolerance)
        {
          Bezier Left, Right;
          SplitBezier(Left, Right);
          return Left.CalculateArcLength(Tolerance) + 
            Right.CalculateArcLength(Tolerance);
        }
        else
          return PolygonLength * (number)0.5 + ChordLength * (number)0.5;
      }

      number Length(number Point1 = 1.0f, number Point2 = 0.0f,
        number Tolerance = 0.001f)
      {
        Ascending(Point1, Point2);
        Bezier SubArc = *this; SubArc.Trim(Point1, Point2);
        return SubArc.CalculateArcLength(Tolerance);
      }
    };

    /**\brief A structure with static-only methods for computing some useful
    information about ellipses, such as where the horizontal and vertical
    tangents are for a rotated ellipse.*/
    struct Ellipse
    {
      /**\brief Computes the vertical tangent of an ellipse given the the
      lengths of the semimajor and semiminor axises (in other words the
      'horizontal radius' and the 'vertical radius').*/
      static inline Vector VerticalTangent(number a, number b,
        number Rotation)
      {
        //Compute the angle t at which the vertical tangent takes place.
        number cos_theta = Cos(Rotation);
        number sin_theta = Sin(Rotation);

        number a_cos_theta = a * cos_theta;
        number a_sin_theta = a * sin_theta;
        number b_cos_theta = b * cos_theta;
        number b_sin_theta = b * sin_theta;

        number t = ArcTan(-b_sin_theta / a_cos_theta);

        /*Apply the matrix rotation algorithm to determine the Cartesian
        components.*/
        number cos_t = Cos(t);
        number sin_t = Sin(t);

        Vector v(a_cos_theta * cos_t - b_sin_theta * sin_t,
             a_sin_theta * cos_t + b_cos_theta * sin_t);

        return v;
      }

      /**\brief Computes the horizontal tangent of an ellipse given the
      the lengths of the semimajor and semiminor axises (in other words
      the 'horizontal radius' and the 'vertical radius').*/
      static inline Vector HorizontalTangent(number a, number b,
        number Rotation)
      {
        //Compute the angle 't' at which the horizontal tangent takes place
        number cos_theta = Cos(Rotation);
        number sin_theta = Sin(Rotation);

        number a_cos_theta = a * cos_theta;
        number a_sin_theta = a * sin_theta;
        number b_cos_theta = b * cos_theta;
        number b_sin_theta = b * sin_theta;

        number t = ArcTan(b_cos_theta / a_sin_theta);

        /*Apply the matrix rotation algorithm to determine the Cartesian
        components.*/
        number cos_t = Cos(t);
        number sin_t = Sin(t);

        Vector v(a_cos_theta * cos_t - b_sin_theta * sin_t,
             a_sin_theta * cos_t + b_cos_theta * sin_t);

        return v;
      }

      /**\brief Finds the intersection of a vertical line and quadrant one
      of an ellipse by converting it to a spline and using a binary search
      algorithm to look for the intersection.
      
      It's a little fickle if you don't use it right, so be careful! This
      algorithm is used for creating an exact path for a note and a stem, so
      that a single contiguous outline may be traced.*/
      static number VerticalIntersection(number a, number b,
        number Rotation, number distFromRightVerticalTangent)
      {
        /*This solves for 't' in the cubic spline in quadrant one.*/

        /*Cache the result so that if the same inputs are given next
        time, it will only be a matter of retrieval.*/
        static number cur_a = 0.0;
        static number cur_b = 0.0;
        static number cur_theta = 0.0;
        static number cur_dist = 0.0;
        static number cur_result = 0.0;

        if(a == cur_a && b == cur_b && Rotation == cur_theta
          && distFromRightVerticalTangent == cur_dist)
          return cur_result;

        //Set the cache input properties
        cur_a = a;
        cur_b = b;
        cur_theta = Rotation;
        cur_dist = distFromRightVerticalTangent;

        //Simply use the origin as the center.
        Vector o(0,0);

        //Scale the ellipse.
        Vector s(cur_a * (number)2.0, cur_b * (number)2.0);

        //Create a cubic spline based off of the input information.
        Bezier c;
        c.Ellipse(o,s,cur_theta,1,true);

        //Compute the vertical tangent.
        Vector i;
        i = VerticalTangent(cur_a,cur_b,cur_theta);
        number lx = i.x - distFromRightVerticalTangent;

        //Set the conditions for exiting.
        const number error_acceptability=(number)0.001;
        const count max_steps = 30;

        //Set the initial search bounds.
        number bound_l = 0.0;
        number bound_r = 1.0;
        number bound_mid = 0.0;

        //Get the values of the curve at the ends.
        Vector bound_l_v = c.Value(0.0);
        Vector bound_r_v = c.Value(1.0);

        //Do a binary subdivision search to quickly hone in.
        for(count steps = 0; steps < max_steps; steps++)
        {
          bound_mid = (bound_r + bound_l) / (number)2.0;
          Vector mid_value = c.Value(bound_mid);

          number mx = Abs(mid_value.x);
          number err = Abs(mx - lx);

          //If the error tolerance is acceptable, leave.
          if(err < error_acceptability)
            break;

          //Depending on which side was closer, adjust the boundaries
          if(mid_value.x < lx)
            bound_r = bound_mid;
          else
            bound_l = bound_mid;
        }

        //Cache the result.
        cur_result = bound_mid;

        //Return the result.
        return cur_result;
      }

      /**A helper method for retrieving a elliptical cubic spline. The
      parameters are the same as the fromEllipse method of the Bezier
      class, but the advantage here is that a new cubic spline is
      returned, where as in the other method, you must already have a
      Bezier object declared.*/
      static Bezier toSpline(Vector& Origin, Vector& Scale,
        number Rotation, count Quadrant, bool isCounterClockwise)
      {
        Bezier c;
        c.Ellipse(Origin, Scale, Rotation, Quadrant, isCounterClockwise);
        return c;
      }
    };
  }
}

#endif
