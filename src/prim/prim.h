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

#ifndef PRIM_H_BELLEBONNESAGE
#define PRIM_H_BELLEBONNESAGE

//The prim header files -- alphabetical order
#include "primArray.h"
#include "primAuto.h"
#include "primBitfield.h"
#include "primColor.h"
#include "primEndian.h"
#include "primFile.h"
#include "primGeometry.h"
#include "primHeap.h"
#include "primLink.h"
#include "primList.h"
#include "primMath.h"
#include "primMacros.h"
#include "primMemory.h"
#include "primOS.h"
#include "primPath.h"
#include "primProfiler.h"
#include "primRandom.h"
#include "primRational.h"
#include "primRectangle.h"
#include "primSound.h"
#include "primStream.h"
#include "primString.h"
#include "primText.h"
#include "primTransform.h"
#include "primTypes.h"
#include "primUnicode.h"
#include "primUnits.h"
#include "primVector.h"
#include "primXML.h"

#endif

/*
Style Guidelines

* All lines should break at 80 characters unless it is syntactically impossible
to do so (almost everything in C++ is capable of being broken)

* If a comment is a statement, then it should not have a period after it; if it
has a verb however, then it must have a period. Also if it contains any type
of punctuation like commas or semicolons, then the comment must have a period.

i.e.

//Length of the array

//Contains the length of the array.

//Length of the array, which must always be greater than zero.

* All variables and method names should be CamelCase with the exception of
certain prefixes that indicate a subproperty of object being described.

i.e.

void ToHTML(void); //Revised: 'To' should be uppercase.

pixelsSize(void);

but...

Object* GetObject(void);

(The verb 'get' must be uppercased, but the preposition 'to' and the unit
'pixels' are lowercased)

* The prim basic data types are lower-case: count, integer, number, etc.

* Simple variables are lowercase: x, i, n, etc.

* The method name 'n' refers to the default length or size of an object. It is
  preferred over "GetLength" or "GetSize".

* Namespaces and sub-namespaces are all lowercase. This ensures that when
calling using namespace, the namespace supplied will be in all lowercase.

* Avoid first-person plural, we.

* Use struct for classes that are completely publicly-scoped.

* Tabs should be inserted as 2 spaces.

* Spaces after commas in parameter lists, and spaces between mathematical
operators.

* Whenever extra white space is used for visual alignment, indicate this
by adding a vertical line of double forward slashes to the right of the block:
  
  i.e.

  uint64 Sum = (uint64)2111111111UL * (uint64)History[3] + //
               (uint64)1492 * (uint64)(History[2]) +       //
               (uint64)1776 * (uint64)(History[1]) +       //
               (uint64)5115 * (uint64)(History[0]) +       //
               (uint64)History[4];                         //

   This is to help a future code beautifier which may be able to use this
   information to bypass its normal spacing routines. It also brings attention
   to the block as a whole.

* Commenting:
   All methods and class members must be commented.

   When describing a method, in general let the code speak for itself, and
   only comment when reading the code does not immediately tell why it is doing
   what it is doing.

   Avoid statements like:
   //Returns the result. (This is totally unnecessary)
   return Result;

   More informative:
   //Returns a copy of the string. (The string will be copied not referenced)
   return NewString;
*/
