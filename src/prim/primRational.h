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

#ifndef primRational
#define primRational

#include "primMath.h"
#include "primString.h"

namespace prim
{
  namespace math
  {
    ///A templated class for expressing rational numbers.
    template <class T>
    class Rational
    {
    protected:
      /**The numerator of the Rational number. It is always expressed in its
      simplest form.*/
      T n;

      /**The denominator of the Rational number. It is always expressed in its
      simplest form.*/
      T d;

      void SimplifySign(void)
      {
        if(d < 0)
        {
          n = -n;
          d = -d;
        }
      }

      void SimplifyRatio(void)
      {
        if(d == 0) //Indeterminate form
        {
          n = 0;
          return;
        }

        if(n == 0) //Zero: assume denominator of one for consistency.
        {
          d = 1;
          return;
        }

        T g = GCD(n, d);
        while(g != 1)
        {
          n = n / g;
          d = d / g;
          g = GCD(n, d);
        }
      }

      void Simplify(void)
      {
        SimplifySign();
        SimplifyRatio();
      }

    public:
      Rational() : n(0), d(1) {}

      Rational(T Numerator, T Denominator)
      {
        n = Numerator;
        d = Denominator;
        Simplify();
      }

      Rational(T WholeNumber)
      {
        n = WholeNumber;
        d = 1;
      }

      T Num(void)
      {
        return n;
      }

      T Den(void)
      {
        return d;
      }

      bool IsDeterminate(void)
      {
        return (d == 0);
      }

      bool IsWhole(void)
      {
        return (d == 1);
      }

      static T GCD(T a, T b)
      {
        //Test for zeroes.
        if(a == 0 || b == 0)
          return 0;

        //Naturalize the numbers.
        if(a < 0) a = -a;
        if(b < 0) b = -b;

        /*Use the iterative Euclidean algorithm. For more information, see:
        http://en.wikipedia.org/wiki/Euclidean_algorithm#Using_iteration
        */
        T t;
        while(b != 0)
        {
          t = b;
          b = a % b;
          a = t;
        }
        return a;
      }

      static T LCM(T a, T b)
      {
        //Test for zeroes.
        if(a == 0 || b == 0)
          return 0;

        //Naturalize the numbers.
        if(a < 0) a = -a;
        if(b < 0) b = -b;

        //Use the GCD to calculate the LCM
        return (a * b) / GCD(a, b);
      }

      static Rational<T> Mod(Rational<T> a, 
        Rational<T> b)
      {
        T new_d = LCM(a.d, b.d);
        T a_mult = new_d / a.d;
        T b_mult = new_d / b.d;
        a.n *= a_mult;
        b.n *= b_mult;
        a.d = new_d;
        b.d = new_d;

        //Now a and b have the same denominator.
        T new_n = a.n % b.n;

        return Rational<T>(new_n, new_d);
      }

      //--------------------//
      //Operator definitions//
      //--------------------//

      Rational<T> operator + (Rational<T> other)
      {
        T lcd = LCM(d, other.d);
        T new_d = lcd;
        T new_n = n * (lcd / d) + other.n * (lcd / other.d);
        return Rational<T>(new_n, new_d);
      }

      Rational<T> operator + (T other)
      {
        return *this + Rational<T>(other, 1);
      }

      Rational<T> operator - (Rational<T> other)
      {
        T lcd = LCM(d,other.d);
        T new_d = lcd;
        T new_n = n * (lcd / d) - other.n * (lcd / other.d);
        return Rational<T>(new_n, new_d);
      }

      Rational<T> operator - (T other)
      {
        return *this - Rational<T>(other, 1);
      }

      Rational<T> operator * (Rational<T> other)
      {
        T new_n = n * other.n;
        T new_d = d * other.d;
        return Rational<T>(new_n, new_d);
      }

      Rational<T> operator * (T other)
      {
        return *this * Rational<T>(other, 1);
      }

      Rational<T> operator / (Rational<T> other)
      {
        T new_n = n * other.d;
        T new_d = d * other.n;
        return Rational<T>(new_n, new_d);
      }

      Rational<T> operator / (T other)
      {
        return *this / Rational<T>(other, 1);
      }

      Rational<T> operator += (Rational<T> other)
      {
        *this = *this + other;
        return *this;
      }

      Rational<T> operator += (T other)
      {
        *this = *this + Rational<T>(other, 1);
        return *this;
      }

      Rational<T> operator -= (Rational<T> other)
      {
        *this = *this + other;
        return *this;
      }

      Rational<T> operator -= (T other)
      {
        *this = *this - Rational<T>(other, 1);
        return *this;
      }

      Rational<T> operator *= (Rational<T> other)
      {
        *this = *this * other;
        return *this;
      }

      Rational<T> operator *= (T other)
      {
        *this = *this * Rational<T>(other, 1);
        return *this;
      }

      Rational<T> operator /= (Rational<T> other)
      {
        *this = *this / other;
        return *this;
      }

      Rational<T> operator /= (T other)
      {
        *this = *this / Rational<T>(other, 1);
        return *this;
      }

      Rational<T> operator = (Rational<T> other)
      {
        n = other.n;
        d = other.d;
        return *this;
      }

      Rational<T> operator = (T other)
      {
        n = other;
        d = 1;
        return *this;
      }

      Rational<T> operator ++ (int dummy)
      {
        n = n + d;
        return *this;
      }

      Rational<T> operator -- (int dummy)
      {
        n = n - d;
        return *this;
      }

      Rational<T> operator - ()
      {
        return (*this * -1);
      }

      bool operator == (Rational<T> other)
      {
        return (n == other.n && d == other.d);
      }

      bool operator == (T other)
      {
        return (n == other && d == 1);
      }

      bool operator != (Rational<T> other)
      {
        return (n != other.n || d != other.d);
      }

      bool operator != (T other)
      {
        return (n != other || d != 1);
      }

      bool operator > (Rational<T> other)
      {
        Rational<T> difference = *this - other;
        return (difference.n > 0);
      }

      bool operator > (T other)
      {
        Rational<T> difference = *this - 
          Rational<T>(other, 1);
        return (difference.n > 0);
      }

      bool operator >= (Rational<T> other)
      {
        Rational<T> difference = *this - other;
        return (difference.n >= 0);
      }

      bool operator >= (T other)
      {
        Rational<T> difference = *this - 
          Rational<T>(other, 1);
        return (difference.n >= 0);
      }

      bool operator < (Rational<T> other)
      {
        Rational<T> difference = *this - other;
        return (difference.n < 0);
      }

      bool operator < (T other)
      {
        Rational<T> difference = *this - 
          Rational<T>(other, 1);
        return (difference.n < 0);
      }

      bool operator <= (Rational<T> other)
      {
        Rational<T> difference = *this - other;
        return (difference.n <= 0);
      }

      bool operator <= (T other)
      {
        Rational<T> difference = *this - 
          Rational<T>(other, 1);
        return (difference.n <= 0);
      }

      template <class Numeric>
      operator Numeric (void)
      {
        return To<Numeric>();
      }

      template <class Numeric>
      Numeric To(void)
      {
        if(d == 0)
          return 0; //Indeterminate case
        else
          return ((Numeric)n) / ((Numeric)d);
      }

      //-------------------------//
      //String Conversion Helpers//
      //-------------------------//

      Rational(prim::String str)
      {
        *this = FromString(str);
      }

      Rational<T> operator + (const prim::String& other)
      {
        return *this + Rational<T>(other);
      }

      Rational<T> operator - (const prim::String& other)
      {
        return *this - Rational<T>(other);
      }

      Rational<T> operator * (const prim::String& other)
      {
        return *this * Rational<T>(other);
      }

      Rational<T> operator / (const prim::String& other)
      {
        return *this / Rational<T>(other);
      }

      Rational<T> operator += (const prim::String& other)
      {
        *this = *this + Rational<T>(other);
        return *this;
      }

      Rational<T> operator -= (const prim::String& other)
      {
        *this = *this - Rational<T>(other);
        return *this;
      }

      Rational<T> operator *= (const prim::String& other)
      {
        *this = *this * Rational<T>(other);
        return *this;
      }

      Rational<T> operator /= (const prim::String& other)
      {
        *this = *this / Rational<T>(other);
        return *this;
      }

      Rational<T> operator = (const prim::String& other)
      {
        *this = FromString(other);
        return *this;
      }

      bool operator == (const prim::String& other)
      {
        Rational<T> r(other);
        return (n == r.n && d == r.d);
      }

      bool operator != (const prim::String& other)
      {
        Rational<T> r(other);
        return (n != r.n || d != r.d);
      }

      bool operator > (const prim::String& other)
      {
        Rational<T> r(other);
        Rational<T> difference = *this - r;
        return (difference.n > 0);
      }

      bool operator >= (const prim::String& other)
      {
        Rational<T> r(other);
        Rational<T> difference = *this - r;
        return (difference.n >= 0);
      }

      bool operator < (const prim::String& other)
      {
        Rational<T> r(other);
        Rational<T> difference = *this - r;
        return (difference.n < 0);
      }

      bool operator <= (const prim::String& other)
      {
        Rational<T> r(other);
        Rational<T> difference = *this - r;
        return (difference.n <= 0);
      }

      prim::String ToString(void)
      {
        if(d == 0)
          return "NaN";

        prim::String str;
        str &= (prim::integer)n;
        str &= "/";
        str &= (prim::integer)d;

        return str;
      }

      prim::String ToPrettyString(void)
      {
        if(d == 0)
          return "indeterminate";

        prim::String str;
        str &= (integer)n;

        if (n != 0 && d != 1)
        {
          str &= "/";
          str &= (integer)d;
        }

        return str;
      }

      static Rational<T> FromString(const prim::String& str)
      {
        prim::count len = str.n();
        T numerator = 0;
        T denominator = 0;
        bool IsPastSlash = false;

        for(prim::count i = 0; i < len; i++)
        {
          prim::ascii c = str[i];
          if(c == '/') 
          {
            IsPastSlash = true;
          }
          else if(!IsPastSlash) 
          {
            if(c >= 48 && c <= 57)
              numerator = numerator * 10 + (T)(c - 48);
          }
          else 
          {
            if(c >= 48 && c <= 57)
              denominator = denominator * 10 + (T)(c - 48);
          }
        }

        if(!IsPastSlash)
          denominator = 1;

        return Rational<T>(numerator, denominator);
      }

      operator prim::String(void)
      {
        return ToPrettyString();
      }
    };

    //Template instantiations
    typedef Rational<int32> Ratio64;
    typedef Rational<int16> Ratio32;
    typedef Rational<int8>  Ratio16;
    typedef Ratio64 Ratio;
  }
}
#endif
