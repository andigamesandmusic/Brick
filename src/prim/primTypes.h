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

#ifndef primTypes
#define primTypes

/*
This header file standardizes the prim library data types according to their 
purpose. For example, you sometimes need a data type of an exact number of 
bytes. Other times you need mathematical precision or simply the ability to 
count things. You can quickly switch the data model using the primCompiler 
define or make a custom data model appropriate for your compiler. In general,
you don't need to worry about this file unless you are compiling on a non-
standard compiler or platform. The purpose of using standardized data types
is to make porting of Belle, Bonne, Sage as simple as setting the appropriate
data model and recompiling.

Here are the assumptions about your compiler's data model. It must have:

1) Integral data types for 8-bit, 16-bit, 32-bit, and 64-bit word lengths.
2) Floating point data types of 32-bit and 64-bit precisions.
3) A 32-bit or 64-bit pointer model.

/----------------------------------------------------------\
|                   Standard Data Models                   |
|----------------------------------------------------------|
|         char | short | int | long | long long | pointers |
| ILP32    1   |   2   |  4  |  4   |     8     |    4     |
| LP64     1   |   2   |  4  |  8   |     8     |    8     |
| LLP64    1   |   2   |  4  |  4   |     8     |    8     |
| ILP64    1   |   2   |  8  |  8   |     8     |    8     |
\----------------------------------------------------------/

More information on cross-platform issues and data models:
http://en.wikipedia.org/wiki/64-bit
http://www.developerfusion.co.uk/show/7220/
http://www.unix.org/version2/whatsnew/lp64_wp.html
*/

//Data model enumeration
#define primILP32           1 //
#define primLP64            2 //
#define primLLP64           3 //
#define primILP64           4 //
#define primCustomDataModel 5 //

//Set the correct compiler data type model here:
#if defined (__LP64__) || defined (_LP64)
  #define primDataModel primLP64
#endif

#ifdef _WIN64
  #define primDataModel primLLP64
#endif

#ifdef PRIM_USING_ILP32
  #define primDataModel primILP32
#elif defined PRIM_USING_LP64
  #define primDataModel primLP64
#elif defined PRIM_USING_LLP64
  #define primDataModel primLLP64
#elif defined PRIM_USING_ILP64
  #define primDataModel primILP64
#endif

#ifndef primDataModel
  #define primDataModel primILP32
#endif

namespace prim
{

  //-----------------//
  //CUSTOM DATA MODEL//
  //-----------------//

  #if primDataModel == primCustomDataModel
    typedef unsigned char uint8;
    typedef char int8;
    typedef unsigned short uint16;
    typedef short int16;
    typedef unsigned int uint32;
    typedef int int32;
    typedef long long int64;
    typedef unsigned long long uint64;
    typedef float float32;
    typedef double float64;
    typedef int64 integer;
    typedef float64 number;
    typedef int32 count;
    typedef uint32 uintptr;
  #endif

  //------------------------//
  //FIXED BYTE INTEGER TYPES//
  //------------------------//

  #if primDataModel != primCustomDataModel
    ///An unsigned 8-bit integer
    typedef unsigned char uint8;

    ///A signed 8-bit integer
    typedef char int8;

    ///An unsigned 16-bit integer
    typedef unsigned short uint16;

    ///A signed 16-bit integer
    typedef short int16;

    #if primDataModel != primILP64
      ///An unsigned 32-bit integer
      typedef unsigned int uint32;

      ///A signed 32-bit integer
      typedef int int32;
    #else
      /*If you compile on an ILP64 platform (which has no native 32-bit integer
      types) then you will need to make sure that the below type assignments are
      correct.*/
      
      typedef _int32 int32;
      typedef unsigned _int32 uint32;
    #endif

    ///A signed 64-bit integer
    typedef long long int64;

    ///An unsigned 64-bit integer
    typedef unsigned long long uint64;

    //--------------------//
    //FLOATING-POINT TYPES//
    //--------------------//

    ///A 32-bit single precision floating point type
    typedef float float32;

    ///A 64-bit double precision floating point type
    typedef double float64;

    //------------------//
    //MATHEMATICAL TYPES//
    //------------------//

    ///A data type for doing general purpose math with integers
    typedef int64 integer; //Use the (faster) 32-bit integers if you like.

    ///A data type for doing general purpose math with floating point numbers
    typedef float32 number; //Use the (faster) 32-bit floats if you like.

    //-------------------------//
    //COUNTING AND MEMORY TYPES//
    //-------------------------//

    #if primDataModel == primILP32
      /**A signed counting integer of at least pointer size. On a 32-bit 
      platform, it should be able to count up to approximately 2.1 billion. On a
      64-bit platform it should be able to count to practically any integer. 
      Since negative numbers are often useful in for-loops and indexing, the 
      integer must be signed.*/
      typedef int32 count; //Could be changed to a 64-bit signed integer.

      /**A data type for doing pointer arithmetic. Only low-level memory
      operations should use this type.*/
      typedef uint32 uintptr;
    #else
      typedef int64 count;
      typedef uint64 uintptr;
    #endif
  #endif

  //--------------------//
  //ASCII CHARACTER TYPE//
  //--------------------//

  ///A signed eight-bit integer used for storing low-level char strings
  typedef int8 ascii; //Do not change this!

  //----------------//
  //BINARY DATA TYPE//
  //----------------//

  ///An unsigned eight-bit integer used for storing low-level binary data
  typedef uint8 byte; //Do not change this!

  //------------//
  //BOOLEAN TYPE//
  //------------//

  //There's no reason to make a boolean type since C++ has a native bool type!
  
  ///Class to perform run-time detection of the data model. 
  struct DataModel
  {
    /**Confirms at runtime that the compiler data type model is correct. This
    method produces an intentional segmentation-fault to halt execution if the
    data model is incorrect.*/
    static bool Validate(void);
  };
}
#endif
