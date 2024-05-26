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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "Libraries.h"

struct Parameters
{
  //Params...
  String InputFilename;
  String OutputFilename;
  int64 OutputSampleRate;
  String OutputSampleFormat;
  String ConvolveFilename;
  SF_INFO ConvolveInfo;
  SNDFILE* ConvolveHandle;
  String ExportFilterFilename;
  
  bool IsRaw;
  int64 InputChannels;
  String InputSampleFormat;
  int64 InputSampleRate;
  
  String PitchShift;
  float64 CentsTolerance;
  
  String DitherType;
  float64 DitherBits;
  
  //Ins...
  int64 Frames; //Number of frames
  int64 Channels; //Number of channels
  int64 P; //Upsample ratio
  int64 Q; //Downsample ratio
  bool SkipFilter; //Whether or not to skip the filter.
  float64 AllowableBandwidthLoss; //Transition width
  float64 StopbandAttenuation; //dB of attenuation
  int64 MaxFFTSize; //In powers of two.
  int64 BCOptimizationLevel; //How many powers-of-two to go up to find the best.
  int64 OldSampleRate; //Old sample rate (Hz)
  int64 NewSampleRate; //New sample rate (Hz)
  
  bool MakeSpectrogram;
  String SpectrogramFormat;
  int64 SpectrogramSize; //FFT size of the gradient
  int64 SpectrogramStep; //Sample step stride
  float64 SpectrogramBeta; //Beta value of the Kaiser window
  String Gradient; //Gradient color scheme, i.e. "gray", "color"
  float64 GradientRange; //dB range of the gradient
  
  //Derived...
  int64 S; //Number of segments (chunk groups) to use
  int64 idealM; //Unsegmented filter size
  int64 idealM_1; //Unsegmented filter size
  int64 idealFFTSize; //Ideal FFT size
  int64 idealL; //Unsegmented audio chunk size
  int64 idealL_1; //Unsegmented audio chunk size minus one
  int64 paddedM; //Size of unsegmented filter size padded to S boundary.
  int64 paddedM_1; //paddedM - 1
  
  int64 M; //Actual filter size
  int64 FFTSize; //Actual FFT size
  int64 M_1; //Actual filter size minus one
  int64 L; //Actual audio chunk size
  int64 L_1; //Actual audio chunk size minus one
  
  int64 InPFrames; //Number of input frames
  int64 OutPFrames; //Number of output P-frames
  int64 OutPQFrames; //Number of output P/Q-frames
  int64 ScratchFileSize; //Size of scratch file in bytes
  
  String OutFormat; //int8, int16, int24, int32, float32, float64
  
  bool InitializeDerivedParameters(void);
  
  void Print(void);
};

#endif
