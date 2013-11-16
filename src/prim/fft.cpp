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

#include "prim.h"
#include "fft.h"

#include <fftw3.h>
#include <cmath>

void AudioFFT::Deinitialize(void)
{
  if(PlanTimeToFreq)
  {
    fftw_destroy_plan(*((fftw_plan*)PlanTimeToFreq));
    fftw_destroy_plan(*((fftw_plan*)PlanFreqToTime));
    
    fftw_free((fftw_complex*)FreqDomain);
    
    if((void*)TimeDomain != (void*)FreqDomain)
      fftw_free((double*)TimeDomain);
    
    delete ((fftw_plan*)PlanTimeToFreq);
    delete ((fftw_plan*)PlanFreqToTime);
    PlanTimeToFreq = 0;
    PlanFreqToTime = 0;
  }
}

double AudioFFT::Initialize(int N, int PlanType, double PlanTime, bool InPlace)
{
  using namespace prim;
  
  //Wipe out any previous initialization.
  Deinitialize();
  
  //Get out of here if requested length is invalid.
  if(N < 1)
    return 0;
  N_TimeDomain = N;
  N_FreqDomain = N / 2 + 1;
  
  //Allocate memory.
  PlanTimeToFreq = (void*)new fftw_plan;
  PlanFreqToTime = (void*)new fftw_plan;
  
  //Create the freq domain array (which is slightly padded)
  FreqDomain = (void*)fftw_malloc(sizeof(fftw_complex) * N_FreqDomain);
  Memory::ClearArray((double*)FreqDomain, N_FreqDomain * 2);
  
  //If in place, we reuse the FreqDomain for TimeDomain, else allocate an array.
  if(InPlace)
    TimeDomain = (double*)FreqDomain;
  else
  {
    TimeDomain = (double*)fftw_malloc(sizeof(double) * N_TimeDomain);
    Memory::ClearArray(TimeDomain, N_TimeDomain);
  }
  
  //Set the amount of time to spend planning.
  fftw_set_timelimit(PlanTime);
  
  //Create the plan for time to frequency domain.
  *((fftw_plan*)PlanTimeToFreq) = fftw_plan_dft_r2c_1d(N_TimeDomain,
    TimeDomain, (fftw_complex*)FreqDomain, PlanType);
    
  /*Create the plan for frequency to time domain. N is given in terms of the
  time-domain length (not frequency-domain, even though that is the input.*/
  *((fftw_plan*)PlanFreqToTime) = fftw_plan_dft_c2r_1d(N_TimeDomain,
    (fftw_complex*)FreqDomain, TimeDomain, PlanType);
  
  //Return number of flops needed.
  double mul1 = 0, add1 = 0, fma1 = 0, mul2 = 0, add2 = 0, fma2 = 0;
  fftw_flops(*((fftw_plan*)PlanTimeToFreq), &add1, &mul1, &fma1);
  fftw_flops(*((fftw_plan*)PlanFreqToTime), &add2, &mul2, &fma2);
  return mul1 + add1 + mul2 + add2 + 2. * (fma1 + fma2);
  
  /*  
  char* allwisdom = fftw_export_wisdom_to_string();
  File::Replace("/generated/wisdom.txt", allwisdom);
  free(allwisdom);*/
}

void AudioFFT::TimeToFreqUnnormalized(void)
{
  //Execute the time-to-freq FFTW plan.
  fftw_execute(*((fftw_plan*)PlanTimeToFreq));
}

void AudioFFT::TimeToFreq(void)
{
  //First do the unnormalized forwards transform.
  TimeToFreqUnnormalized();
  
  ///Normalize the frequency domain by dividing out the FFT length.
  double N_inv = 1.0 / (double)N_TimeDomain;
  for(int i = 0; i < N_FreqDomain; i++)
  {
    double& re = ((fftw_complex*)FreqDomain)[i][0];
    double& im = ((fftw_complex*)FreqDomain)[i][1];
    re *= N_inv;
    im *= N_inv;
  }
}

void AudioFFT::FreqToTime(void)
{
  //Execute the freq-to-time FFTW plan.
  fftw_execute(*((fftw_plan*)PlanFreqToTime));
}

double AudioFFT::FreqReal(int i)
{
  return ((fftw_complex*)FreqDomain)[i][0];
}

void AudioFFT::FreqReal(int i, double Value)
{
  ((fftw_complex*)FreqDomain)[i][0] = Value;
}

double AudioFFT::FreqImag(int i)
{
  return ((fftw_complex*)FreqDomain)[i][1];
}

void AudioFFT::FreqImag(int i, double Value)
{
  ((fftw_complex*)FreqDomain)[i][1] = Value;
}

double AudioFFT::Mag(int i)
{
  if(i >= 0 && i < N_FreqDomain)
  {
    double re = ((fftw_complex*)FreqDomain)[i][0];
    double im = ((fftw_complex*)FreqDomain)[i][1];
    return sqrt(re * re + im * im) * 2.0;
  }
  else
    return 0;    
}

double AudioFFT::Ang(int i)
{
  if(i >= 0 && i < N_FreqDomain)
  {
    double re = ((fftw_complex*)FreqDomain)[i][0];
    double im = ((fftw_complex*)FreqDomain)[i][1];
    return atan2(im, re);
  }
  else
    return 0;    
}

double* AudioFFT::GetTimeDomain(void)
{
  return TimeDomain;
}

double* AudioFFT::GetFreqDomain(void)
{
  return (double*)FreqDomain;
}
