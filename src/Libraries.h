/*
  ==============================================================================

   This file is part of Brick
   Copyright 2010 by William Andrew Burnson
   
  ------------------------------------------------------------------------------

   Brick can be redistributed and/or modified under the terms of
   the GNU General Public License, as published by the
   Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Brick is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Brick; if not, visit www.gnu.org/licenses or write:
   
   Free Software Foundation, Inc.
   59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ==============================================================================
*/
//Make sure that floatx is supported.
#ifndef USING_FLOATX
  #error MUST COMPILE WITH FLOATX SUPPORT
#else

#ifndef BRICK_LIBRARIES_H
#define BRICK_LIBRARIES_H

//The Belle, Bonne, Sage prim library with FFT extensions.
#include "prim.h"
#include "fft.h"
using namespace prim;
using namespace sound;

//The JUCE library
#include "juce.h"

//The libsndfile library
#include <sndfile.h>

//The FFTW library
#include <fftw3.h>

/*According to: http://www.mega-nerd.com/FPcast/
lrint() can be much faster than casting. Need these defines:*/
#define _ISOC9X_SOURCE 1
#define _ISOC99_SOURCE 1

#include <math.h>
#include <stdio.h>
#include <time.h>

#endif
#endif
