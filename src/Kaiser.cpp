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

#include "Kaiser.h"
  
float64 Kaiser::BesselI0(float64 z)
{
  /*The BesselI[0, z] function, also known as the zeroth-order modified Bessel
  function of the first kind, is defined by:
  
          oo                    oo  
          __  (z ^ 2 / 4) ^ k   __  [(z / 2) ^ k ]
  I0(z) = \   _______________ = \   |____________| ^ 2  =
          /_     (k!) ^ 2       /_  [     k!     ]        
  
         k = 0                 k = 0
         
             oo                            oo
             __  [(z / 2) ^ k ]            __              
        1 +  \   |____________| ^ 2  = 1 + \   P_z[k] ^ 2, where 
             /_  [     k!     ]            /_
        
            k = 1                         k = 1
            
                              z / 2
        P_z[k] = P_z[k - 1] * -----, P_z[0] = 1
                                k
  
  We note that the function is evenly symmetric and I0(0) = 1 is the function's
  minimum. The function grows monotonically at an exponential pace. I0(700) is
  above 10^300, and since this is the near the limit of the double floating-
  point arithmetic, the valid domain of this numeric function is [-700, 700].
  Beyond that, input values are clamped to z = +/- 700.
  
  The numerical accuracy of the function depends on the value of z chosen, but
  it appears that over the given range 15 digit accuracy is typical. The
  approach taken is to continue the summation so long as it has an effect on the
  output. When the number added is too small to affect the result, the loop
  exits. Since calculations are stable, there should not be any input in the
  domain which would not eventually exit. Still, in case there is some number
  that could cause this to occur, a conservative value of 500 summations is
  used to prevent a dead-lock. It was calculated that under normal operations,
  the maximum number of summations before total convergence is 465. Note as
  well that the number of summations is essentially proportional to the input z,
  something like Summations ~= 3 + z * 0.66.
    
  Note that in the Kaiser window which uses this Bessel function, the domain
  used is [0, beta], where beta is the alpha-pi constant that determines the
  window's sidelobe attenuation. For example, a beta of 20 (i.e., a Kaiser-20
  window) has sidelobe attenuation of about -190 dB.
  
  Source:
  http://mathworld.wolfram.com/ModifiedBesselFunctionoftheFirstKind.html
  http://en.wikipedia.org/wiki/Kaiser_window
  */
  
  //1) Simple case: for z = 0, the result is exactly 1.
  if(z == 0.)
    return 1.;
    
  /*2) The function is evenly symmetric, so to make things simpler, wrap the
  domain so that it is always used positively.*/
  if(z < 0.)
    z = -z;
    
  /*3) For x > 700, the exponent of the Bessel is > 10^302 which is near the
  exponent limits of 64-bit floating point math.*/
  if(z > 700.)
    return 1.52959334767187748231187662062e302; //Result for z = 700.
  
  //Determine initial conditions.
  float64 z2 = z * 0.5, Result = 1.0, PreviousResult = 0.0, Pz_k = 1.0, k = 1.0;
  
  /*Calculate until the series converges on a single decimal value. This happens
  because at some point a very small number is added to a large number and there
  is no change. This event causes the loop to stop.*/
  while(PreviousResult != Result && k < 500.)
  {
    PreviousResult = Result;
    Pz_k = Pz_k * (z2 / k++);
    Result += Pz_k * Pz_k;
  }
  return Result;
  
  /* For test suite:
  Kaiser k;
  std::cout.precision(30);
  for(float64 z = -10.0; z <= 710.0; z += 0.25)
    std::cout << z << ": " << k.BesselI0(z) << std::endl;
  */
}

float64 Kaiser::GetKaiserValue(int64 i)
{ 
  //The following is the definition of the Kaiser window, Q = BesselI0(fx_beta)
  if(i < 0 || i > M)
    return fx_0;
  else
  {
    float64 val = fx_2_div_M * fx(i) - fx_1;
    return BesselI0(fx_beta * sqrt(fx_1 - val * val)) / Q;
  }
}

int64 Kaiser::EstimateOrder(float64 TransitionWidth,
  float64 dB_StopbandAttenuation)
{
  float64 ExactOrder = (dB_StopbandAttenuation - 7.95) /
    (2.285 * 3.141592654 * TransitionWidth);
  
  /*Round order to nearest integer and then round up to next odd integer.
  We use odd filter lengths so that the group delay will be an integer.*/
  int64 ActualOrder = (int64)(ExactOrder + 0.5);
  if(ActualOrder % 2 == 0)
    ActualOrder++;
  if(ActualOrder < 11) ActualOrder = 11;
  return ActualOrder;
}

float64 Kaiser::EstimateBeta(float64 dB_StopbandAttenuation)
{
  if(dB_StopbandAttenuation < 21.)
    return 0.;
  else if(dB_StopbandAttenuation <= 50.)
    return (float64)
      (fx(0.5842) * pow(fx(dB_StopbandAttenuation - 21.), fx(0.4))
      + fx(0.07886 * (dB_StopbandAttenuation - 21.)));
  else
    return 0.1102 * (dB_StopbandAttenuation - 8.7);
}

Kaiser::Kaiser() : fx_0(fx(0)),
  fx_1(fx(1)), fx_2(fx(2)), fx_pi(acos(fx_0) * fx_2), N(0), M(0), beta(0),
  fx_2_div_M(fx(0)), fx_beta(fx(0)), Q(fx(0)), wc(0), tw(0), atten(0)
{
}

void Kaiser::Initialize(float64 CutoffFrequency, float64 TransitionWidth, 
  float64 dB_StopbandAttenuation)
{
  CutoffFrequency += TransitionWidth * 0.5;
  N = EstimateOrder(TransitionWidth, dB_StopbandAttenuation);
  M = N - 1;
  fx_2_div_M = fx(2) / fx(M);
  beta = EstimateBeta(dB_StopbandAttenuation);
  fx_beta = fx(beta);
  Q = BesselI0(fx_beta);
  
  wc = CutoffFrequency;
  tw = TransitionWidth;
  atten = dB_StopbandAttenuation;
}

void Kaiser::Initialize(int64 P, int64 Q, float64 AllowableBandwidthLoss,
  float64 dB_StopbandAttenuation)
{
  int64 MaxPQ = math::Max(P, Q);
  float64 Reciprocal_BW = 1.0 / (float64)MaxPQ;
  float64 TransitionWidth = Reciprocal_BW * AllowableBandwidthLoss;
  Initialize(Reciprocal_BW - TransitionWidth, TransitionWidth,
    dB_StopbandAttenuation);
}

void Kaiser::Initialize(int64 Length, float64 Beta)
{
  //Set the parameters to the Kaiser window directly.
  N = Length;
  M = N - 1;
  fx_2_div_M = fx(2) / fx(M);
  beta = Beta;
  fx_beta = fx(beta);
  Q = BesselI0(fx_beta);
  
  /*These don't matter -- just set them to something reasonable in case user
  accidentally tries to create a low-pass filter.*/
  wc = 0.5;
  tw = 0.1;
  atten = 100;
}

int64 Kaiser::GetOrder(void)
{
  return N;
}

float64 Kaiser::GetBeta(void)
{
  return beta;
}

float64 Kaiser::CreateWindow(Array<float64>& Window)
{
  Window.Clear();
  Window.n(N);
  float64 Area = 0;
  for(int64 i = 0; i < N; i++)
  {
    float64 Value = GetKaiserValue(i);
    Area += Value;
    Window[i] = Value;
  }
  return Area / (float64)N;
}

void Kaiser::CreateLowPassFilter(Array<float64>& Filter)
{
  Filter.n(N);
  int64 MiddleSample = (N - 1) / 2;
  Filter[MiddleSample] = 1 * fx(wc);
  float64 fx_freq = fx(wc) * fx_pi;
  for(int64 i = MiddleSample + 1; i < N; i++)
  {
    float64 fx_i = fx(i - MiddleSample);
    Filter[i] = sin(fx_i * fx_freq) / (fx_i * fx_freq) * fx(wc);
    Filter[2 * MiddleSample - i] = Filter[i];
  }
}

void Kaiser::CreateWindowedFilter(Array<float64>& Window,
  Array<float64>& Filter, Array<float64>& WindowedFilter)
{
  WindowedFilter.n(Window.n());
  for(int64 i = 0; i < Window.n(); i++)
    WindowedFilter[i] = Window[i] * Filter[i];
}

void Kaiser::CreateLPFInPlace(float64* Head, count Start, count Samples)
{
  int64 MiddleSample = (N - 1) / 2;
  float64 fx_freq = wc * fx_pi;
  int64 i_End = Start + Samples;
  int64 j = 0;
  for(int64 i = Start; i < i_End; i++, j++)
  {
    float64 fx_i = (float64)i - MiddleSample;
    float64 x = fx_i * fx_freq;
    float64 Value;
    if(i == MiddleSample)
      Value = wc;
    else
      Value = GetKaiserValue(i) * wc * (sin(x) / x);
    Head[j] = Value;
  }
}
