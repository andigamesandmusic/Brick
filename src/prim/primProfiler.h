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

#ifndef primProfiler
#define primProfiler

#include "primTypes.h"
#include "primFile.h"

//#define UseProfiler
//#define UseWindowsHighPrecisionTimer

#ifdef UseProfiler
  #define _profile(_profiler) static prim::count _pid = \
    _profiler.AddProfile(__LINE__,__FILE__); \
    _profiler.Increment(_pid);
  #define _endprofile(_profiler) _profiler.AddDuration(_pid)
#else
  #define _profile(_profiler)
  #define _endprofile(_profiler)
#endif

namespace prim
{
  class String;

  struct Profiler
  {
    static const integer   MaximumProfiles = 1000;
    integer    LineNumbers[MaximumProfiles];
    const ascii* Filenames[MaximumProfiles];
    integer    Frequencies[MaximumProfiles];
    uint64       Durations[MaximumProfiles];
    uint64      StartTimes[MaximumProfiles];

    count ProfileCount;
    static bool Running;

    inline count AddProfile(integer LineNumber, const ascii* Filename)
    {
      if(!Running)
        return 0;

      const ascii* JustFile = Filename;
      while(*Filename!=0)
      {
        if(*Filename=='/' || *Filename=='\\')
          JustFile = Filename + 1;
        Filename++;
      }

      LineNumbers[ProfileCount] = LineNumber;
      Filenames[ProfileCount] = JustFile;
      Frequencies[ProfileCount] = 0;
      Durations[ProfileCount] = 0;
      StartTimes[ProfileCount] = 0;
      ProfileCount++;
      return ProfileCount-1;
    }

#if defined(UseProfiler) && defined(UseWindowsHighPrecisionTimer)
    inline static uint64 _RDTSC()
    {
      _asm _emit 0x0F
      _asm _emit 0x31
    }
#endif

    inline void Increment(count ID)
    {
      if(Running)
        Frequencies[ID]++;

#if defined(UseProfiler) && defined(UseWindowsHighPrecisionTimer)
      StartTimes[ID]=_RDTSC();
#endif
    }

    inline void AddDuration(count ID)
    {
#if defined(UseProfiler) && defined(UseWindowsHighPrecisionTimer)
      uint64 tick = _RDTSC();
      if(Running)
        Durations[ID] += (tick - StartTimes[ID]);
#else
      ID = 0; //Set to zero to avoid compiler warning.
#endif
    }

    void GetSummaryAsString(String& out, bool sortByFrequency = false);
    void SendSummaryToConsole(bool sortByFrequency = false);

    Profiler() : ProfileCount(0){}
  };

  extern Profiler primProfiles;
}

#endif
