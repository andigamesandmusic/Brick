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

#ifndef KAISER_H
#define KAISER_H

#include "Libraries.h"

class Kaiser
{
  ///Constants to precalculate
  float64 fx_0, fx_1, fx_2, fx_pi;

  ///Initialization constants
  int64 N, M;
  float64 beta;
  float64 fx_2_div_M;
  float64 fx_beta;
  float64 Q;
  
  ///Initialization parameters
  float64 wc, tw, atten;
  
  ///Converts constant value into floatx with appropriate precision.
  inline float64 fx(float64 x)
  {
    return x;
  }
  
  public:
  
  ///Zeroth-order modified Bessel function of the first kind. (For Kaiser.)
  float64 BesselI0(float64 z);
  
  /**Gets the value of the Kaiser window at some index, given its order N, and
  its beta parameter.*/
  float64 GetKaiserValue(int64 i);
  
  ///Estimate the order of the filter. Always returns an odd integer.
  int64 EstimateOrder(float64 TransitionWidth, float64 dB_StopbandAttenuation);
  
  ///Estimates beta.
  float64 EstimateBeta(float64 dB_StopbandAttenuation);
  
  public:
  
  ///Constructor specifies precision of the Kaiser window.
  Kaiser();
  
  ///Initializes the window.
  void Initialize(float64 CutoffFrequency, float64 TransitionWidth, 
    float64 dB_StopbandAttenuation);
  
  ///Initialize using a resampling specification.
  void Initialize(int64 P, int64  Q, float64 AllowableBandwidthLoss,
    float64 dB_StopbandAttenuation);
  
  ///Initialize by specify the parameters directly.
  void Initialize(int64 Length, float64 Beta);
  
  ///Returns the length of the window that has been initialized.
  int64 GetOrder(void);
  
  ///Returns the length of the window that has been initialized.
  float64 GetBeta(void);
  
  /**Creates the window and returns the normalized area under the window. For
  a rectangular window, this would be 1. A Kaiser window will always be less
  than 1. You can use this value to correctly scale frequency domain results
  for analysis purposes.*/
  float64 CreateWindow(Array<float64>& Window);
  
  ///Creates low-pass filter.
  void CreateLowPassFilter(Array<float64>& Filter);
  
  ///Creates a windowed filter.
  void CreateWindowedFilter(Array<float64>& Window, Array<float64>& Filter,
    Array<float64>& WindowedFilter);
    
  ///Same as CreateWindowedFilter, but creates to pre-allocated memory.
  void CreateLPFInPlace(float64* Head, count Start, count Samples);
};
#endif
