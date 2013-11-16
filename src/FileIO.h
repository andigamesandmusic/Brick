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

#ifndef BRICK_FILEIO_H
#define BRICK_FILEIO_H

#include "Libraries.h"
#include "Parameters.h"

struct FileIO
{
  int64 FramesPerChunk;
  int64 ChannelsPerChunk;
  int64 IntBits;
  bool OutputIsIntType;
  
  bool Clipped;
  bool UseDither;
  
  float64* OutChunk;
  int32* OutChunkInt;
  
  Random RandomNumberGenerator;
  
  FileIO();
  ~FileIO();

  void GetNormalizationScaleAndBitShift(float64& Scale, int64& BitShift);
  float64 GetNormalizedMinValue(void);
  float64 GetNormalizedMaxValue(void);
  int64 RectangularDitherDoubleToInt(double SampleValue, float64 DitherBits);
  int64 TriangularDitherDoubleToInt(double SampleValue, float64 DitherBits);
  void CleanupChunks(void);
  void OutChunkToOutChunkInt(String Dither, float64 DitherBits,
    float64 NormalizationScale);
  void InitializeChunks(int64 Frames, int64 Channels, bool IsInt, 
    int64 IntBits);

  int GetFormatEnum(String Format);
  bool IsFormatInt(String Format);
  int GetFormatBits(String Format);
  int GetEffectiveFormatBits(String Format);
  String GetFormat(SF_INFO& s_info, String* Description = 0);
  String GetSampleType(SF_INFO& s_info, String* Description = 0);
  math::Ratio GetPitchShiftRatio(String p, float64 CentsTolerance);
  String CheckFileError(SNDFILE* s);
  
  void Go(Parameters& p);
  
  void MakeSpectrogram(Parameters& p, SNDFILE* s);
  
  //Inline functions
  static inline int32 ClipInt64(int64 x)
  {
    /*Note: since there are no negative constants in C++, we have to subtract
    to numbers in order to get the lower range for int32.*/
    if(x < -2147483648LL)
      return ((int32)-2147483647) - ((int32)1);
    else if(x > 2147483647LL)
      return 2147483647;
    else
      return (int32)x;
  }
};

#endif
