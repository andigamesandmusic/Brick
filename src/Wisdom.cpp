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

#include "Wisdom.h"

bool FFTMultithread::Init(void)
{
  Console c;
  c++;
  if(fftw_init_threads() == 0)
  {
    c += "There was a problem beginning the multithreading FFT engine.";
    return false;
  }
  
  int NumCPUs = juce::SystemStats::getNumCpus();
  c += "Initializing multithreading FFT engine to make use of ";
  c &= (integer)NumCPUs;
  c &= " cores or CPUs in parallel for maximum performance.";
  fftw_plan_with_nthreads(NumCPUs);
  c++;
  return true;
}

void FFTMultithread::Cleanup(void)
{
  fftw_cleanup_threads();
}

FFTMultithread::FFTMultithread()
{
  Init();
}

FFTMultithread::~FFTMultithread()
{
  Cleanup();
}
  
Wisdom::Wisdom()
{
  Name = "Wisdom";
  Extension = "xml";
  Folder = "Brick";
}

void Wisdom::LoadWisdomFromCache(void)
{
  juce::PropertiesFile* pf =
    juce::PropertiesFile::createDefaultAppPropertiesFile(Name, Extension,
    Folder, false, -1, juce::PropertiesFile::storeAsXML);
  WisdomText = pf->getValue("Wisdom").toUTF8();
  fftw_import_wisdom_from_string(WisdomText.Merge());
}

void Wisdom::SaveWisdomToCache(void)
{
  juce::PropertiesFile* pf =
    juce::PropertiesFile::createDefaultAppPropertiesFile(Name, Extension,
    Folder, false, -1, juce::PropertiesFile::storeAsXML);
  pf->setValue("Wisdom", juce::String(WisdomText.Merge()));
  pf->save();
}

void Wisdom::AcquireWisdom(void)
{
  Console c;
  c += "Acquiring wisdom and storing in: ";
  c += juce::PropertiesFile::getDefaultAppSettingsFile(Name, Extension,
    Folder, false).getFullPathName().toUTF8();
    
  c++;
  c += "Note: this could take many, many hours, but you can safely terminate "
    "the program at any point as wisdom is saved at each checkpoint. When "
    "you start --acquirewisdom again, it will pick up where it left off. The "
    "higher powers-of-two FFTs (millions of points) can take longer to "
    "measure, but the wisdom may be useful for higher quality sample rate "
    "conversions.";
  c++;
  
  prim::float64 StartTick = juce::Time::getMillisecondCounterHiRes();
  
  int Megs = juce::SystemStats::getMemorySizeInMegabytes();
  
  c += (integer)Megs; c &= "MB of RAM detected. Acquiring wisdom for FFTs "
  "that are 1/64 the size of available RAM (to account for the additional "
  "memory allocated by FFTW).";
  c++;
  int ExtraPowersOfTwo = (int)(math::Log(2.0, (float64)Megs) + 0.1) - 6;
  
  
  int Limit = 20 + ExtraPowersOfTwo;
  {
    int64 p = 0;
    c += "STAGE 1 / 3 (10 Second Measure)";
    c += "-------------------------------";
    for(int64 i = 1; p <= Limit; i *= 2)
    {
      c += "Acquiring wisdom for power-of-two "; c &= p;
        c &= " / "; c &= (integer)Limit; p++;
      AudioFFT afft;
      integer flops = afft.Initialize(i, FFTW_PATIENT, 5);
      
      //Save wisdom now in case of crash...
      {
        char* allwisdom = fftw_export_wisdom_to_string();
        WisdomText = allwisdom;
        free(allwisdom);
      }
      SaveWisdomToCache();
    }
  }
  
  {
    int64 p = 0;
    c += "STAGE 2 / 3 (1 Minute Measure)";
    c += "----------------------------";
    for(int64 i = 1; p <= Limit; i *= 2)
    {
      c += "Acquiring wisdom for power-of-two "; c &= p;
        c &= " / "; c &= (integer)Limit; p++;
      AudioFFT afft;
      integer flops = afft.Initialize(i, FFTW_MEASURE, 30);
      
      //Save wisdom now in case of crash...
      {
        char* allwisdom = fftw_export_wisdom_to_string();
        WisdomText = allwisdom;
        free(allwisdom);
      }
      SaveWisdomToCache();
    }
  }
  
  {
    int64 p = 0;
    c += "STAGE 3 / 3 (2 Minute Measure)";
    c += "-----------------------------";
    for(int64 i = 1; p <= Limit; i *= 2)
    {
      c += "Acquiring wisdom for power-of-two "; c &= p;
        c &= " / "; c &= (integer)Limit; p++;
      AudioFFT afft;
      integer flops = afft.Initialize(i, FFTW_MEASURE, 60);
      
      //Save wisdom now in case of crash...
      {
        char* allwisdom = fftw_export_wisdom_to_string();
        WisdomText = allwisdom;
        free(allwisdom);
      }
      SaveWisdomToCache();
    }
  }
  
  prim::float64 EndTick = juce::Time::getMillisecondCounterHiRes();
  prim::float64 TicksElapsed = (prim::float64)(EndTick - StartTick);
  prim::float64 TicksPerSecond = (prim::float64)1000.0;
  prim::float64 SecondsElapsed = TicksElapsed / TicksPerSecond;
  c += "Acquiring wisdom took "; c &= (number)SecondsElapsed;
  c &= " seconds";
}

void Wisdom::ForgetWisdom(void)
{
  WisdomText = "";
  SaveWisdomToCache();
}
