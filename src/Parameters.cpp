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

#include "Parameters.h"
#include "Kaiser.h"
  
bool Parameters::InitializeDerivedParameters(void)
{
  //Create a Kaiser window object and determine the filter order.
  if(!ConvolveHandle)
  {
    Kaiser KaiserLPF;
    KaiserLPF.Initialize(P, Q, AllowableBandwidthLoss, StopbandAttenuation);
    idealM = KaiserLPF.GetOrder();
    idealM_1 = idealM - 1;
  }
  else
  {
    idealM = ConvolveInfo.frames;
    idealM_1 = idealM - 1;
  }

  //Algorithmically determine the best overlap-and-add FFT size.
  int64 MinPowerOfTwo = (int64)math::Log(2.0, (float64)idealM + 1.0);
  
  //Get out of here if design requirements are insane.
  if(MinPowerOfTwo > 37)
  {
    Console c;
    c += "The design requirements are too high (filter is over a terabyte).";
    return false;
  }
  
  int64 Min_i = MinPowerOfTwo + 2;
  int64 i_2 = 1;
  float64 MinEstimate = -1.;
  int64 AcceptableEstimateIndex = -1;
  for(int64 i = 1; i <= MinPowerOfTwo; i++)
    i_2 *= 2;
  
  //Search within five powers-of-two.
  for(int64 i = MinPowerOfTwo; i <= MinPowerOfTwo + 5; i++, i_2 *= 2)
  {
    float64 Estimate;
    if(i_2 > idealM_1)
    {
      AcceptableEstimateIndex++;
      Estimate = (float64)i_2 * (math::Log(2.0, (float64)i_2) + 1.0) / 
        (float64)(i_2 - idealM_1);
      if(Estimate < MinEstimate || MinEstimate < 0)
      {
        MinEstimate = Estimate;
        Min_i = i;
      }
      
      /*Break zero or negative allows the estimate to continue until it finds
      the best. If break occurs at 1, then minimum (reasonable) FFT size is
      used. Breaking at 2, a good balance is reached. If a high break is used,
      then the best performance (blind to memory) is used. This could in
      practice be much slower if the swap file turns on. To mitigate the
      performance hit of this scenario, we will break up the convolution to
      our own disk based chunks working on a scratch file. It is always 
      preferable to use a lower estimate than to use a disk-based method.*/
      if(BCOptimizationLevel > 0 &&
        AcceptableEstimateIndex == BCOptimizationLevel)
          break;
    }
  }
  
  idealFFTSize = 1;
  for(int64 i = 1; i <= Min_i; i++)
    idealFFTSize *= 2;
  idealL = idealFFTSize - idealM_1;
  idealL_1 = idealL - 1;
  S = 1;
  
  if(Min_i > MaxFFTSize)
  {
    for(int64 i = 0; i < (Min_i - MaxFFTSize); i++)
      S *= 2;
  }
  
  paddedM = idealM + S - (idealM % S);
  paddedM_1 = paddedM - 1;
  
  FFTSize = idealFFTSize / S;
  M = paddedM / S;
  M_1 = M - 1;
  L = FFTSize - M_1;
  L_1 = L - 1;
  
  InPFrames = Frames * P;
  OutPFrames = InPFrames + paddedM_1;
  if(OutPFrames % Q == 0)
    OutPQFrames = OutPFrames / Q;
  else
    OutPQFrames = (OutPFrames + (Q - (OutPFrames % Q))) / Q;
  
  ScratchFileSize = OutPQFrames * Channels * sizeof(float64);

  return true;
}

void Parameters::Print(void)
{
  Console c;
  c += "-------------------------------------------------";
  c += "Resample Parameters";
  c += "-------------------------------------------------";
  c += "Frames: "; c &= Frames;
  c += "Channels: "; c &= Channels;
  c += "P: "; c &= P;
  c += "Q: "; c &= Q;
  c += "Filtering: "; c &= (!SkipFilter ? "yes" : "no");
  if(!SkipFilter)
  {
    c += "Allowable Bandwidth Loss: "; c &= (number)AllowableBandwidthLoss;
    c += "Stopband Attenuation: "; c &= (number)StopbandAttenuation;
    c += "MaxFFTSize: "; c &= MaxFFTSize;
    c += "BCOptimizationLevel: "; c &= BCOptimizationLevel;
  }
  c += "-------------------------------------------------";
  /*c += "idealM: "; c &= idealM;
  c += "idealFFTSize: "; c &= idealFFTSize;
  c += "idealL: "; c &= idealL;
  c += "paddedM: "; c &= paddedM;
  c += "-------------------------------------------------";
  c += "Chunk groups (passes): "; c &= S;
  c += "M: "; c &= M;
  c += "FFTSize: "; c &= FFTSize;
  c += "L: "; c &= L;
  c += "-------------------------------------------------";
  c += "In P-Frames: "; c &= InPFrames;
  c += "Out P-Frames: "; c &= OutPFrames;
  c += "Out PQ-Frames: "; c &= OutPQFrames;
  c += "Scratch File Size: "; c &= ScratchFileSize;
  c += "-------------------------------------------------";*/
}
