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

#ifndef primTransform
#define primTransform

#include "primMath.h"
#include "primString.h"
#include "primList.h"

namespace prim
{
  namespace math
  {
    /**\brief Stores a two-dimensional common transformation matrix.
    
    The matrix stores combinations of translation, scaling, and rotation.
    Safeguards have been implemented to prevent creating a non-invertible matrix
    which can occur if the matrix is scaled by zero. (Non-invertible matrices
    have undefined behavior.) Multiplication is done with the + operator.
    Multiplying by an inverse and taking the inverse are done with the
    - operator. Transforming a vector by the matrix is accomplished with the
    << operator, and likewise untransforming a vector is accomplished
    with the >> operator.*/
    struct AffineMatrix
    {
      //Coefficients
      number a; //   Matrix
      number b; // .        .
      number c; // | a  b  0|
      number d; // | c  d  0|
      number e; // | e  f  1|
      number f; // '        '

      //------------------//
      //Basic Constructors//
      //------------------//

      ///Creates a default matrix at the origin with a unit scale.
      AffineMatrix()
      {
        a = 1; b = 0; c = 0;
        d = 1; e = 0; f = 0;
      }

      ///Constructor via coefficients.
      AffineMatrix(number a, number b, number c, number d, number e, number f)
      {
        AffineMatrix::a = a;
        AffineMatrix::b = b;
        AffineMatrix::c = c;
        AffineMatrix::d = d;
        AffineMatrix::e = e;
        AffineMatrix::f = f;
      }
      
      /**Determines if there is a rotation that is not a multiple of 90 degrees.
      Assumes that the matrix does not have a skewing operation.*/
      bool HasNonTrivialRotation(void) const
      {
        return !IsApproximatelyEqual(b, 0.f) &&
          !IsApproximatelyEqual(Abs(b), 1.f);
      }

      //----------------------------//
      //Multiplication and Inversion//
      //----------------------------//

      /**Multiplies this transform with another and returns the result. Note
      that the given matrix is premultiplied with the current one. In other
      words New X Old (matrix multiplication is not commutative).*/
      AffineMatrix operator + (const AffineMatrix& other) const
      {
        AffineMatrix M;

        //Read in this matrix to local variables.
        number this_a = a, this_b = b, this_c = c;
        number this_d = d, this_e = e, this_f = f;

        //Read in other matrix to local variables.
        number other_a = other.a, other_b = other.b, other_c = other.c;
        number other_d = other.d, other_e = other.e, other_f = other.f;

        //Do matrix multiplication.
        M.a = other_a * this_a + other_b * this_c;
        M.b = other_a * this_b + other_b * this_d;

        M.c = other_c * this_a + other_d * this_c;
        M.d = other_c * this_b + other_d * this_d;

        M.e = other_e * this_a + other_f * this_c + this_e;
        M.f = other_e * this_b + other_f * this_d + this_f;

        return M;
      }

      /**Calculates the inverse of the transform (unary minus). If the matrix is
      not invertible, then the same matrix is returned.*/
      AffineMatrix operator - (void) const
      {
        /*
        Matrix Inversion Algorithm:
        .         .
        | a' b' 0 |                |   d        -b      0  |
        | c' d' 0 |  =   1 / DET * |  -c         a      0  |
        | e' f' 1 |                |cf - de  -af - be  DET |
        '         '
           with DET  =  a * d - c * b
        */

        //Calculate the determinant.
        number DET = a * d - b * c;

        //Make sure the matrix is invertible. If not return the same matrix. 
        if(!DET)
          return *this;

        //Precalculate a few things.
        number DETinv = (number)1.0 / DET;
        number neg_f = -f;

        //Calculate the new values.
        number a1 = d * DETinv;
        number b1 = -b * DETinv;
        number c1 = -c * DETinv;
        number d1 = a * DETinv;
        number e1 = neg_f * c1 - e * a1;
        number f1 = e * b1 + neg_f * d1;

        //Fill the new matrix.
        return AffineMatrix(a1, b1, c1, d1, e1, f1);
      }

      //-----------------------//
      //Transformation Matrices//
      //-----------------------//

      AffineMatrix static Unit(void)
      {
        return AffineMatrix(1, 0, 0, 1, 0, 0);
      }

      AffineMatrix static Translation(Vector TranslateBy)
      {
        return AffineMatrix(1, 0, 0, 1, TranslateBy.x, TranslateBy.y);
      }

      AffineMatrix static Scale(number ScaleBy)
      {
        return AffineMatrix(ScaleBy, 0, 0, ScaleBy, 0, 0);
      }

      AffineMatrix static Scale(Vector ScaleBy)
      {
        return AffineMatrix(ScaleBy.x, 0, 0, ScaleBy.y, 0, 0);
      }

      AffineMatrix static Rotation(number RotateBy, bool UseDegrees = false)
      {
        if(UseDegrees)
          RotateBy *= DegToRad;

        number cos_t = Cos(RotateBy);
        number sin_t = Sin(RotateBy);

        return AffineMatrix(cos_t, sin_t, -sin_t, cos_t, 0, 0);
      }

      //-------------------//
      //Hybrid Constructors//
      //-------------------//

      ///Creates a rotated, scaled, and translated affine matrix.
      AffineMatrix(Vector TranslateBy, Vector ScaleBy,
        number RotateBy = (number)0.0, bool UseDegrees = false)
      {
        if(UseDegrees)
          RotateBy *= DegToRad;
        
        *this = Unit() + Translation(TranslateBy) +
          Scale(ScaleBy) + Rotation(RotateBy);
      }

      ///Creates a rotated, scaled, and translated affine matrix.
      AffineMatrix(Vector TranslateBy, number ScaleBy = (number)1.0,
        number RotateBy = (number)0.0, bool UseDegrees = false)
      {
        if(UseDegrees)
          RotateBy *= DegToRad;
        
        *this = Unit() + Translation(TranslateBy) +
          Scale(ScaleBy) + Rotation(RotateBy);
      }

      //--------------------//
      //Additional Operators//
      //--------------------//

      ///Stores the multiplication of the left matrix with the right.
      void operator += (const AffineMatrix& other)
      {
        *this = *this + other;
      }

      ///Returns the left matrix multiplied by the inverse of right.
      AffineMatrix operator - (AffineMatrix other) const
      {
        return *this + -other;
      }

      ///Stores the left matrix multiplied by the inverse of right.
      void operator -= (AffineMatrix other)
      {
        *this = *this + -other;
      }

      //--------------------//
      //Point Transformation//
      //--------------------//

      ///Transforms a vector point using the matrix.
      Vector operator << (const Vector& Untransformed) const
      {
        number x = Untransformed.x;
        number y = Untransformed.y;

        return Vector(a * x + c * y + e, b * x + d * y + f);
      }

      ///Untransforms a vector point using the matrix.
      Vector operator >> (const Vector& Transformed) const
      {
        number x = Transformed.x;
        number y = Transformed.y;
        AffineMatrix i = -(*this);

        return Vector(i.a * x + i.c * y + i.e, i.b * x + i.d * y + i.f);
      }

      //----------------//
      //String Operation//
      //----------------//

      ///Prints the matrix to a string.
      operator String(void) const
      {
        String m;

        m += "/                                         \\";
        m += "|"; m &= a; m &= ", "; m &= b;
        m += "|"; m &= c; m &= ", "; m &= d;
        m += "|"; m &= e; m &= ", "; m &= f;
        m += "\\                                         /";
        return m;
      }
    };

    template <class S = count>
    struct StateMatrix : public AffineMatrix
    {
      mutable S State;

      StateMatrix()
      {
      }

      StateMatrix(const AffineMatrix& BaseObject) : AffineMatrix(BaseObject)
      {
      }      
    };
    
    /**\brief AffineTransform keeps a stack of Matrix objects in a List.
    
    It provides easy manuevering between different spaces. It is also able to
    be templated so that you can store information about each Matrix as a
    state. For example the current pen color could be stored in a structure
    that you use for the templated type.*/
    template <class S = count>
    class AffineTransform
    {
    protected:
      List< StateMatrix<S> > Stack;
    public:
      /**\brief Produces a StateMatrix containing a transformation which when
      applied using its << operator, will transform a vector from the
      start state to the end state. \details The state information for End
      will be copied into the new StateMatrix.*/
      StateMatrix<S> Transform(count Start, count End) const
      {
        //Fix bound problems as necessary.
        Start = Min(Max(Start, (count)0), Stack.n() - (count)1);
        End = Min(Max(End, (count)0), Stack.n() - (count)1);

        //Begin with identity matrix.
        StateMatrix<S> M;

        if(Start < End)
          //Traverse forward and compute matrix.
          for(count i = Start + 1; i <= End; i++)
            M += Stack[i];
        else if(End < Start)
          //Traverse backwards and compute matrix.
          for(count i = Start; i >= End + 1; i--)
            M -= Stack[i];

        //Copy the state information of the final matrix.
        M.State = Stack[End].State;

        return M;
      };

      ///Creates a matrix to transform a vector through the entire stack.
      StateMatrix<S> Forwards(void) const
      {
        return Transform(0, Stack.n() - 1);
      }

      ///Creates a matrix to undo Forwards.
      StateMatrix<S> Backwards(void) const
      {
        return Transform(Stack.n() - 1, 0);
      }

      ///Pushes a transformation onto the end of the stack.
      void Push(StateMatrix<S> Transformation)
      {
        Stack.Append(Transformation);
      }

      ///Pops the last transformation off the stack.
      void Pop(void)
      {
        Stack.Remove(Stack.n() - 1);
      }

      ///Returns the state of the matrix on top the stack.
      S& TopState(void) const
      {
        return Stack.last().State;
      }

      ///Returns the number of states in the AffineTransform.
      prim::count States(void) const
      {
        return Stack.n();
      }

      ///Default constructor begins with an identity matrix.
      AffineTransform()
      {
        //Create identity matrix.
        StateMatrix<S> M;

        //Push identity matrix to the stack.
        Push(M);
      }
    };
  }
}
#endif
