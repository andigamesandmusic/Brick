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

#ifndef primMacros
#define primMacros

//You can disable these macros by specifying:
//#define PRIM_DO_NOT_USE_MACROS
//before you declare prim.h and bbs.h
#ifndef PRIM_DO_NOT_USE_MACROS

#include "primTypes.h"

/**A simple for-each loop. This works for any object that has n() declared as
its element count method, i.e. prim::List.*/
#define foreach(iterator, object) for(prim::count iterator = 0; \
                                    iterator < object.n(); iterator++)
#define loop(iterator, iterations) for(prim::count iterator = 0; \
                                     iterator < iterations; iterator++)

#endif

#endif
