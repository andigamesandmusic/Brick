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

#ifndef primOS
#define primOS

namespace prim
{
  /**Determines basic information about the current operating system. The prim
  library does not actually use these for anything; the information is only here
  as a convenience for developers using the library. In order for these to work,
  when prim.cpp is compiled it must have one of the following macros defined:
  PRIM_WINDOWS, PRIM_MACOSX, PRIM_LINUX, or PRIM_BSD otherwise the methods will
  all return false.*/
  struct OS
  {
    ///Returns true if operating is some variant of Windows.
    static bool Windows(void);
    
    ///Returns true if operating is some variant of Mac OS X.
    static bool MacOSX(void);
    
    ///Returns true if operating is some variant of Linux.
    static bool Linux(void);
    
    ///Returns true if operating is some variant of BSD.
    static bool BSD(void);
  };
}

#endif
