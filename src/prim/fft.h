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

/* How to use FFTW3:
1) You need FFTW3. Download the latest tar from http://fftw.org and do:
./configure
make
sudo make install

2) In your makefile or premake4 file you need to add the paths for:
Header: /usr/local/include
Libraries: /usr/local/lib

3) You need to link with fftw3 (-lfftw3)
*/

#ifndef AUDIO_FFT
#define AUDIO_FFT

///Wrapper for FFTW (Fastest Fourier Transforms in the West)
class AudioFFT
{
  ///Length of the FFT.
  int N_TimeDomain;
  int N_FreqDomain;

  ///The real-input time domain.
  double* TimeDomain;
  
  //Note: Using void* below to isolate FFTW3.h to just fftw.cpp.
  
  ///The complex-output frequency domain.
  void* FreqDomain;
  
  ///The plan created using FFTW's wisdom feature.
  void* PlanTimeToFreq;
  void* PlanFreqToTime;
  
  ///Frees all data.
  void Deinitialize(void);

  public:
  
  ///Constructor zeroes out structure.
  AudioFFT() : N_TimeDomain(0), N_FreqDomain(0), TimeDomain(0), FreqDomain(0),
    PlanTimeToFreq(0), PlanFreqToTime(0) {}
  
  ///Destructor frees all data.
  ~AudioFFT() {Deinitialize();}
  
  ///Gets the length of the time domain portion of the FFT.
  inline int N_Time(void) {return N_TimeDomain;}
  
  ///Gets the length of the time domain portion of the FFT.
  inline int N_Freq(void) {return N_FreqDomain;}
  
  /**Sets up the input and output arrays and makes FFTW plans. N is the length
  of the FFT, which does not need to be a power-of-two. PlanTime is the total
  amount of time to spend planning in seconds. (Actual plan time may exceed
  this if it is in the middle of testing an algorithm when the time is over.)
  Returns the number of flops (floating-point operations) needed. If InPlace is
  true, then the time domain and frequency domain will share the same memory
  (and calling TimeToFreq or FreqToTime overwrites the other.)*/
  double Initialize(int N, int PlanType, double PlanTime, bool InPlace = false);
  
  ///Calculates forwards transform and divides by the length of the FFT.
  void TimeToFreq(void);
  
  /**Performs the forwards transform from time to frequency. This operation,
  unlike its inverse, preserves data in the input (time-domain here), so this
  operation is always safe.*/  
  void TimeToFreqUnnormalized(void);
  
  /**Performs the forwards transform from time to frequency. Note well: this
  operation destroys the data in the frequency-domain. Either keep a copy of it,
  if it is still needed, or use TimeToFreq to get it back.*/
  void FreqToTime(void);
  
  ///Gets the value in the time-domain at index i.
  inline double Time(int i) {return TimeDomain[i];}
  
  ///Sets the value in the time-domain at index i.
  inline void Time(int i, double Value) {TimeDomain[i] = Value;}
  
  ///Gets the real value in the freq-domain at index i.
  double FreqReal(int i);
  
  ///Sets the real value in the freq-domain at index i.
  void FreqReal(int i, double Value);
    
  ///Gets the real value in the freq-domain at index i.
  double FreqImag(int i);
  
  ///Sets the real value in the freq-domain at index i.
  void FreqImag(int i, double Value);
  
  ///Gets the magnitude in the freq-domain at index i.
  double Mag(int i);
  
  ///Gets the angle in the freq-domain at index i.
  double Ang(int i);
  
  ///Gets a pointer to the time domain data.
  double* GetTimeDomain(void);
  
  ///Gets a pointer to the frequency domain data.
  double* GetFreqDomain(void);
};
#endif
