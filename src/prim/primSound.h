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

#ifndef primSound
#define primSound

#include "primEndian.h"
#include "primFile.h"
#include "primMemory.h"
#include "primTypes.h"

namespace prim
{
  ///A partial representation of the 80-bit floating point format.
  struct float80
  {
    ///The 80-bits stored in big-endian format.
    uint8 Bytes[10];
    
    ///Constructor zeroes out Bytes.
    float80()
    {
      Memory::Set(&Bytes[0], 0, 10);
    }

    ///Converts an uint32 to an 80 bit IEEE Standard 754 floating point.
    void ConvertFromLong(uint32 value)
    {
      uint32 exp;
      uint8 i;
      uint8* buffer = &Bytes[0];
      Memory::Set(buffer, 0, 10);

      exp = value;
      exp >>= 1;
      for(i = 0; i < 32; i++)
      {
        exp >>= 1;
        if(!exp)
          break;
      }
      
      *(buffer + 1) = i;

      for(i = 32; i; i--)
      {
        if(value & 0x80000000)
          break;
        value <<= 1;
      }
      Endian::ConvertObjectToBigEndian(value);
      *(uint32*)(buffer + 2) = value;
      buffer[0] = 64; //Don't know why I need to do this...
    }
  };
}

namespace prim { namespace sound
{
  ///Represents a 16-bit PCM audio channel.
  class Channel16Bit
  {
    int16* Samples;
    count SampleCount;
    
    public:
    ///Initializes the channel with a certain number of samples.
    Channel16Bit(count SampleCount)
    {
      if(SampleCount < 0)
        SampleCount = 0;
      
      if(SampleCount)
      {
        Samples = new int16[SampleCount];
        Memory::ClearArray(Samples, SampleCount);
      }
      
      Channel16Bit::SampleCount = SampleCount;
    }
    
    ///Cleans up the channel and deletes the memory associated with it.
    ~Channel16Bit()
    {
      if(SampleCount)
        delete [] Samples;
    }
    
    ///Gets a pointer to the sample at a particular location.
    inline int16* GetSamples(count i = 0)
    {
      return &Samples[i];
    }
    
    ///Gets the value of a particular sample.
    inline int16 GetSample(count i)
    {
      if(i >= 0 && i < SampleCount)
        return Samples[i];
    }
    
    ///Sets the value of a particular sample.
    inline void SetSample(count i, int16 Value)
    {
      if(i >= 0 && i < SampleCount)
        Samples[i] = Value;
    }
    
    ///Adds a value to a particular sample's existing value.
    inline void SumToSample(count i, int16 Value)
    {
      if(i >= 0 && i < SampleCount)
        Samples[i] += Value;
    }
    
    ///Returns the number of samples in this channel.
    inline count GetSampleCount(void)
    {
      return SampleCount;
    }
  };
  
  ///A wrapper class for quickly writing multi-channel 16-bit AIFF files.
  class MultiChannel16BitAIFF
  {
    count ChannelCount;
    Array<Channel16Bit*> Channels;
    count SampleCount;
    count SampleRate;
  public:
    ///Initializes the AIFF with channels, a sample rate, and duration.
    MultiChannel16BitAIFF(count ChannelCount, count SampleRate, number Seconds)
    {
      if(ChannelCount < 1)
        ChannelCount = 1;
        
      SampleCount = (count)((number)SampleRate * Seconds);
      
      Channels.n(ChannelCount);
      
      for(count i = 0; i < ChannelCount; i++)
        Channels[i] = new Channel16Bit(SampleCount);
        
      MultiChannel16BitAIFF::SampleRate = SampleRate;
      MultiChannel16BitAIFF::ChannelCount = ChannelCount;
    }
    
    ///Deletes the memory associated with the AIFF.
    ~MultiChannel16BitAIFF()
    {
      for(count i = 0; i < ChannelCount; i++)
        delete Channels[i];
    }
    
    ///Changes the internal sample rate (does not resample).
    void SetSampleRate(count NewSampleRate)
    {
      SampleRate = NewSampleRate;
    }
    
    ///Returns a pointer to one of the 16-bit channels.
    Channel16Bit* GetChannel(count i)
    {
      if(i < 0 || i >= ChannelCount)
        return 0;
      return Channels[i];
    }
    
    ///Writes this AIFF to file.
    void WriteToFile(const ascii* Filename)
    {
      uint8 FORMAIFF[12] = {'F', 'O', 'R', 'M', 0, 0, 0, 0, 'A', 'I', 'F', 'F'};
      
      uint8 COMMONCHUNK[26];
      COMMONCHUNK[0] = 'C';
      COMMONCHUNK[1] = 'O';
      COMMONCHUNK[2] = 'M';
      COMMONCHUNK[3] = 'M';
      
      int32 chunkSize = 18;
      int16 numChannels = (int16)ChannelCount;
      uint32 numSampleFrames = (uint32)SampleCount;
      int16 sampleSize = 16;
      float80 sampleRate; sampleRate.ConvertFromLong((uint32)SampleRate);
      
      Endian::ConvertObjectToBigEndian(chunkSize);
      Endian::ConvertObjectToBigEndian(numChannels);
      Endian::ConvertObjectToBigEndian(numSampleFrames);
      Endian::ConvertObjectToBigEndian(sampleSize);
      
      *(int32*)&COMMONCHUNK[4] = chunkSize;
      *(int16*)&COMMONCHUNK[8] = numChannels;
      *(uint32*)&COMMONCHUNK[10] = numSampleFrames;
      *(int16*)&COMMONCHUNK[14] = sampleSize;
      
      for(int i = 0; i < 10; i++)
        COMMONCHUNK[i + 16] = sampleRate.Bytes[i];
        
      uint8 SSNDCHUNK[16] = {'S', 'S', 'N', 'D', 0,0,0,0, 0,0,0,0, 0,0,0,0};
      
      int32 ssndChunkSize = (int32)(8 + 2 * SampleCount * ChannelCount);
      Endian::ConvertObjectToBigEndian(ssndChunkSize);
      *(int32*)&SSNDCHUNK[4] = ssndChunkSize;
      
      uint64 FileLength = 8 + chunkSize + 8 + ssndChunkSize;
      *(uint32*)&FORMAIFF[4] = (uint32)FileLength;
      
      uint8 HEADER[12 + 26 + 16];
      for(count i = 0; i < 12; i++) HEADER[i] = FORMAIFF[i];
      for(count i = 0; i < 26; i++) HEADER[i + 12] = COMMONCHUNK[i];
      for(count i = 0; i < 16; i++) HEADER[i + 12 + 26] = SSNDCHUNK[i];
      count HeaderLength = 12 + 26 + 16;
      
      File::Write(Filename, &HEADER[0], HeaderLength);

      count BufferSamples = 1024 * 256;
      int16* Buffer = new int16[BufferSamples * ChannelCount];
      for(count i = 0; i < SampleCount; i += BufferSamples)
      {
        count CurrentSamples = BufferSamples;
        if(SampleCount - i < BufferSamples)
          CurrentSamples = SampleCount - i;
        
        for(count c = 0; c < ChannelCount; c++)
        {
          Channel16Bit* channel = Channels[c];
          int16* source = channel->GetSamples(i);
          int16* sourceend = source + CurrentSamples;
          int16* dest = &Buffer[c];
          if(Endian::IsLittleEndian())
          {
            while(source != sourceend)
            {
              uint16 v = (uint16)*source;
              uint8 a = (uint8)v;
              uint8 b = (uint8)(v >> 8);
              uint16 v_big_endian = (uint16)b +
                ((uint16)a << (uint16)8);
              *dest = (int16)v_big_endian;
              dest += ChannelCount;
              source++;
            }
          }
          else
          {
            while(source != sourceend)
            {
              *dest = *source;
              dest += ChannelCount;
              source++;
            }           
          }
        }
        
        File::Append(Filename, (byte*)&Buffer[0],
          CurrentSamples * ChannelCount * 2);
      }
      delete [] Buffer;
    }
  };
  
  ///Represents a 32-bit PCM audio channel.
  class Channel32Bit
  {
    int32* Samples;
    count SampleCount;
    
    public:
    ///Initializes the channel with a certain number of samples.
    Channel32Bit(count SampleCount)
    {
      if(SampleCount < 0)
        SampleCount = 0;
      
      if(SampleCount)
      {
        Samples = new int32[SampleCount];
        Memory::ClearArray(Samples, SampleCount);
      }
      
      Channel32Bit::SampleCount = SampleCount;
    }
    
    ///Cleans up the channel and deletes the memory associated with it.
    ~Channel32Bit()
    {
      if(SampleCount)
        delete [] Samples;
    }
    
    ///Gets a pointer to the sample at a particular location.
    inline int32* GetSamples(count i = 0)
    {
      return &Samples[i];
    }
    
    ///Gets the value of a particular sample.
    inline int32 GetSample(count i)
    {
      if(i >= 0 && i < SampleCount)
        return Samples[i];
    }
    
    ///Sets the value of a particular sample.
    inline void SetSample(count i, int32 Value)
    {
      if(i >= 0 && i < SampleCount)
        Samples[i] = Value;
    }
    
    ///Adds a value to a particular sample's existing value.
    inline void SumToSample(count i, int32 Value)
    {
      if(i >= 0 && i < SampleCount)
        Samples[i] += Value;
    }
    
    ///Returns the number of samples in this channel.
    inline count GetSampleCount(void)
    {
      return SampleCount;
    }
  };
  
  ///A wrapper class for quickly writing multi-channel 16-bit AIFF files.
  class MultiChannel32BitAIFF
  {
    count ChannelCount;
    Array<Channel32Bit*> Channels;
    count SampleCount;
    count SampleRate;
  public:
    ///Initializes the AIFF with channels, a sample rate, and duration.
    MultiChannel32BitAIFF(count ChannelCount, count SampleRate, number Seconds)
    {
      if(ChannelCount < 1)
        ChannelCount = 1;
        
      SampleCount = (count)((number)SampleRate * Seconds);
      
      Channels.n(ChannelCount);
      
      for(count i = 0; i < ChannelCount; i++)
        Channels[i] = new Channel32Bit(SampleCount);
        
      MultiChannel32BitAIFF::SampleRate = SampleRate;
      MultiChannel32BitAIFF::ChannelCount = ChannelCount;
    }
    
    ///Deletes the memory associated with the AIFF.
    ~MultiChannel32BitAIFF()
    {
      for(count i = 0; i < ChannelCount; i++)
        delete Channels[i];
    }
    
    ///Changes the internal sample rate (does not resample).
    void SetSampleRate(count NewSampleRate)
    {
      SampleRate = NewSampleRate;
    }
    
    ///Returns a pointer to one of the 16-bit channels.
    Channel32Bit* GetChannel(count i)
    {
      if(i < 0 || i >= ChannelCount)
        return 0;
      return Channels[i];
    }
    
    ///Writes this AIFF to file.
    void WriteToFile(const ascii* Filename)
    {
      uint8 FORMAIFF[12] = {'F', 'O', 'R', 'M', 0, 0, 0, 0, 'A', 'I', 'F', 'F'};
      
      uint8 COMMONCHUNK[26];
      COMMONCHUNK[0] = 'C';
      COMMONCHUNK[1] = 'O';
      COMMONCHUNK[2] = 'M';
      COMMONCHUNK[3] = 'M';
      
      int32 chunkSize = 18;
      int16 numChannels = (int16)ChannelCount;
      uint32 numSampleFrames = (uint32)SampleCount;
      int16 sampleSize = 32;
      float80 sampleRate; sampleRate.ConvertFromLong((uint32)SampleRate);
      
      Endian::ConvertObjectToBigEndian(chunkSize);
      Endian::ConvertObjectToBigEndian(numChannels);
      Endian::ConvertObjectToBigEndian(numSampleFrames);
      Endian::ConvertObjectToBigEndian(sampleSize);
      
      *(int32*)&COMMONCHUNK[4] = chunkSize;
      *(int16*)&COMMONCHUNK[8] = numChannels;
      *(uint32*)&COMMONCHUNK[10] = numSampleFrames;
      *(int16*)&COMMONCHUNK[14] = sampleSize;
      
      for(int i = 0; i < 10; i++)
        COMMONCHUNK[i + 16] = sampleRate.Bytes[i];
        
      uint8 SSNDCHUNK[16] = {'S', 'S', 'N', 'D', 0,0,0,0, 0,0,0,0, 0,0,0,0};
      
      int32 ssndChunkSize = (int32)(8 + 4 * SampleCount * ChannelCount);
      Endian::ConvertObjectToBigEndian(ssndChunkSize);
      *(int32*)&SSNDCHUNK[4] = ssndChunkSize;
      
      uint64 FileLength = 8 + chunkSize + 8 + ssndChunkSize;
      *(uint32*)&FORMAIFF[4] = (uint32)FileLength;
      
      uint8 HEADER[12 + 26 + 16];
      for(count i = 0; i < 12; i++) HEADER[i] = FORMAIFF[i];
      for(count i = 0; i < 26; i++) HEADER[i + 12] = COMMONCHUNK[i];
      for(count i = 0; i < 16; i++) HEADER[i + 12 + 26] = SSNDCHUNK[i];
      count HeaderLength = 12 + 26 + 16;
      
      File::Write(Filename, &HEADER[0], HeaderLength);

      count BufferSamples = 1024 * 256;
      int32* Buffer = new int32[BufferSamples * ChannelCount];
      for(count i = 0; i < SampleCount; i += BufferSamples)
      {
        count CurrentSamples = BufferSamples;
        if(SampleCount - i < BufferSamples)
          CurrentSamples = SampleCount - i;
        
        for(count c = 0; c < ChannelCount; c++)
        {
          Channel32Bit* channel = Channels[c];
          int32* source = channel->GetSamples(i);
          int32* sourceend = source + CurrentSamples;
          int32* dest = &Buffer[c];
          if(Endian::IsLittleEndian())
          {
            while(source != sourceend)
            {
              uint32 v = (uint32)*source;
              uint8 a = (uint8)v;
              uint8 b = (uint8)(v >> 8);
              uint8 c = (uint8)(v >> 16);
              uint8 d = (uint8)(v >> 24);
              uint32 v_big_endian =
                (uint32)d +
                ((uint32)c << (uint32)8) +
                ((uint32)b << (uint32)16) +
                ((uint32)a << (uint32)24);
              *dest = (int32)v_big_endian;
              
              dest += ChannelCount;
              source++;
            }
          }
          else
          {
            while(source != sourceend)
            {
              *dest = *source;
              dest += ChannelCount;
              source++;
            }           
          }
        }
        
        File::Append(Filename, (byte*)&Buffer[0],
          CurrentSamples * ChannelCount * 4);
      }
      delete [] Buffer;
    }
  };
}}
#endif
