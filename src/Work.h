/*
  ==============================================================================

   This file is part of Brick
   Copyright 2010 by Andi
   
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

#ifndef BRICK_WORK_H
#define BRICK_WORK_H

#include "Libraries.h"

class GlobalWorkInfo
{
  static juce::CriticalSection myCriticalSection;
  
  static float64 PercentComplete;
  static int64 PassNumber;
  static int64 TotalPasses;
  
  public:
  static void reset(void)
  {
    const juce::ScopedLock sl(myCriticalSection);
    PercentComplete = 0;
    PassNumber = 0;
    TotalPasses = 0;
  }
  
  static void setPercentComplete(float64 x)
  {
    const juce::ScopedLock sl(myCriticalSection);
    PercentComplete = x;
  }
  
  static float64 getPercentComplete(void)
  {
    const juce::ScopedLock sl(myCriticalSection);
    return PercentComplete;
  }
  
  static void setPassNumber(int64 x)
  {
    const juce::ScopedLock sl(myCriticalSection);
    PassNumber = x;
  }
  
  static int64 getPassNumber(void)
  {
    const juce::ScopedLock sl(myCriticalSection);
    return PassNumber;
  }

  static void setTotalPasses(int64 x)
  {
    const juce::ScopedLock sl(myCriticalSection);
    TotalPasses = x;
  }
  
  static int64 getTotalPasses(void)
  {
    const juce::ScopedLock sl(myCriticalSection);
    return TotalPasses;
  }
};

#endif
