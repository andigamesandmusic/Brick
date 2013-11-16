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

#ifndef primStream
#define primStream

#include "primRational.h"
#include "primString.h"
#include "primTypes.h"
#include "primVector.h"

namespace prim
{
  /**\brief Stream is a std::stream replacement that enables you to derive a
  class that implements the Out method, and then receive calls from the client
  to push string data to the stream using the standard appending operators
  defined by the prim library. \details A good example of this is Console,
  which simply redirects Out to std::cout. Stream must be derived as it is an
  abstract base class.*/
  class Stream
  {
  protected:
    /**\brief Stream::Out() is a pure method requiring that the derived
    class override it.*/
    virtual void Out(const ascii* s) = 0;

    /**\brief Remembers whether or not anything has been added yet to adjust
    for new lines.*/
    static bool BeginningOfStream;

  public:
    ///The virtual destructor
    virtual ~Stream() {}
    
    ///Outputs a string.
    void operator &= (const ascii* s)
    {
      Out(s);
    }

    ///Outputs a constant string.
    void operator &= (const String& s)
    {
      Out(s);
    }

    ///Outputs an integer.
    void operator &= (integer n)
    {
      String s(n);
      Out(s.Merge());
    }

    ///Outputs a number.
    void operator &= (number n)
    {
      String s(n);
      Out(s.Merge());
    }

    ///Outputs a vector.
    void operator &= (math::Vector v)
    {
      String s(v);
      Out(s.Merge());
    }

    ///Outputs a ratio.
    void operator &= (math::Ratio r)
    {
      String s(r.ToPrettyString());
      Out(s.Merge());
    }

    ///Outputs a space.
    void operator -- (int)
    {
      String s;
      s--;
      Out(s.Merge());
    }

    ///Outputs a space followed by a constant string.
    void operator -= (const ascii* s)
    {
      String s2;
      if(!BeginningOfStream)
        s2--;
      s2 &= s;
      Out(s2);
    }

    ///Outputs a space followed by a string.
    void operator -= (const String& s)
    {
      String s2;
      if(!BeginningOfStream)
        s2--;
      s2 &= s;
      Out(s2);
    }

    ///Outputs a space followed by an integer.
    void operator -= (integer n)
    {
      String s;
      if(!BeginningOfStream)
        s--;
      s&=n;
      Out(s.Merge());
    }

    ///Outputs a space followed by a number.
    void operator -= (number n)
    {
      String s;
      if(!BeginningOfStream)
        s--;
      s&=n;
      Out(s.Merge());
    }

    ///Outputs a space followed by a vector.
    void operator -= (math::Vector v)
    {
      String s;
      if(!BeginningOfStream)
        s--;
      s&=v;
      Out(s.Merge());
    }

    ///Outputs a space followed by a ratio.
    void operator -= (math::Ratio r)
    {
      String s;
      if(!BeginningOfStream)
        s--;
      s&=r.ToPrettyString();
      Out(s.Merge());
    }

    ///Outputs a new line.
    void operator ++ (int)
    {
      String s;
      s++;
      Out(s.Merge());
    }

    ///Outputs a new line followed by a constant string.
    void operator += (const ascii* s)
    {
      String s2;
      if(!BeginningOfStream)
        s2++;
      s2&=s;
      Out(s2.Merge());
    }

    ///Outputs a new line followed by a string.
    void operator += (const String& s)
    {
      String s2;
      if(!BeginningOfStream)
        s2++;
      s2&=s;
      Out(s2.Merge());
    }

    ///Outputs a new line followed by an integer.
    void operator += (integer n)
    {
      String s;
      if(!BeginningOfStream)
        s++;
      s&=n;
      Out(s.Merge());
    }

    ///Outputs a new line followed by a number.
    void operator += (number n)
    {
      String s;
      if(!BeginningOfStream)
        s++;
      s&=n;
      Out(s.Merge());
    }

    ///Outputs a new line followed by a vector.
    void operator += (math::Vector v)
    {
      String s;
      if(!BeginningOfStream)
        s++;
      s&=v;
      Out(s.Merge());
    }

    ///Outputs a new line followed by a vector.
    void operator += (math::Ratio r)
    {
      String s;
      if(!BeginningOfStream)
        s++;
      s&=r.ToPrettyString();
      Out(s.Merge());
    }
  };

  ///The Console class redirects output to std::cout.
  class Console : public Stream
  {
  protected:
    ///Method to output incoming stream data to the console window.
    virtual void Out(const ascii* s);
  public:
    ///Waits until the user types a letter and presses return.
    void WaitForKeyStroke(void);
    
    ///The virtual destructor
    virtual ~Console() {};
  };
}

#endif
