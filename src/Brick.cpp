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

#include "FileIO.h"
#include "Globals.h"
#include "Kaiser.h"
#include "Parameters.h"
#include "Wisdom.h"

#include "Help.h"

bool DisplayHelp(prim::List<prim::String>& Arguments)
{
  if(Arguments.n() == 1)
  {
    Console c;
    c += "Type --help for help on using parameters.";
    c++;
    return true;
  }
  if((Arguments.n() == 2 &&
    (Arguments[1] == "--help" || Arguments[1] == "-help" ||
    Arguments[1] == "help")))
  {
    DoHelp();
    Console c;
    c++;
  /*
    Console c;
    String s;
    File::ReadAsUTF8("brick/Help.txt", s);
    c += s;
  */
    return true;
  }
  return false;
}

bool SetGlobals(prim::List<prim::String>& Arguments, GlobalInfo& g)
{
  if(!g.ImportFilesAndParameters(Arguments))
    return false;
  g.CreateParameterKeyValues();
  g.CreateListOfParameters();
  if(!g.SetAllParameters())
    return false;
  if(!g.CheckParameterExclusions())
    return false;
  g.PrintAllParameters();
  return true;
}

void DoWisdom(Wisdom& w, GlobalInfo& g, FFTMultithread*& fftm)
{
  if(!g.IsSpecified("donotloadwisdom") && !g.IsSpecified("forgetwisdom"))
  {
    if(!fftm)
      fftm = new FFTMultithread;
    w.LoadWisdomFromCache();
  }
  
  if(g.IsSpecified("acquirewisdom"))
  {
    if(!fftm)
      fftm = new FFTMultithread;
    w.AcquireWisdom();
  }
  
  if(g.IsSpecified("forgetwisdom"))
  {
    fftm = 0;
    w.ForgetWisdom();
  }
}

void CommandLine(prim::List<prim::String>& Arguments)
{
  Console c;
  if(Arguments.n() == 2 && Arguments[1] == "test")
  {
    //Function to generate line from -0.5 to 0.5
    if(false)
    {
      int64 samples = 1024 * 1024;
      float64* data = new float64[samples];
      Random r;
      for(int64 i = 0; i < samples; i++)
      {
        float64 value = 0.5 / 32768. + math::Sin((float64)(i % 100) * 441. / 
          44100. * 2. * 3.14159265) / 32768. / 4.;
        data[i] = value;
      }
      File::Write("/generated/dithertest.raw", (byte*)data, (count)samples * 
        sizeof(float64));
    }
    
    //Function to test Bessel functions
    if(false)
    {
      Kaiser k;
      std::cout.precision(30);
      for(float64 z = 0; z <= 700.0; z += 0.25)
      {
        std::cout << z << ": " << k.BesselI0(z) << std::endl;
      }
    }
    
    //Function to test Kaiser window on FFT
    if(false)
    {
      //DoSpectrogram();
    }
    
    if(false)
    {
      //Double rounding does weird things!
      float64 j = 1.0;
      while(true)
      {
        j *= 0.5;
        float64 n = 1.0 - j;
        if(n >= 1.0) break;
        if(lrint(32766.5 + n) == 32768)
        {
          std::cout.precision(30);
          std::cout << n << std::endl;
        }
      }
    }
    
    if(true)
    {
      count samples = 80 * 96000 + 1;
      double* d = new double[samples];
      
      //double samples = 80 * 96000 + 1;
      /*Mathematica code:
      f = 2 Pi*(1/E);
      N[f, 20]
      N[2 Cos[f], 20]
      N[Sin[-f], 20]
      N[Sin[-2 f], 20]
      */
      double f = 2.3114546995818434358;
      double twocosf = -1.3495479061189383385;
      double prev = -0.73802446590373770493;
      double pprev = 0.99599937262493702099;

      for(int i = 0; i < samples; i++)
      {
        double sinnf = twocosf * prev - pprev;
        d[i] = 0.5 * sinnf;
        pprev = prev;
        prev = sinnf;
      }
      
      /*
      for(count i = 0; i < samples; i++)
      {
        double x = (double)i;
        double x2 = 2.0453077171808549730e-07 * x * x;
        const double twopi = 6.2831853071795864769;
        d[i] = 0.5 * sin(fmod(x2, twopi));
      }
      */
      File::Write("/generated/mathematica/_Tone80s_Recursive.raw", (byte*)d,
        samples * sizeof(float64));
      c += "Wrote recursive sin tone.";
    }
    return;
  }
  
  if(DisplayHelp(Arguments))
    return;
  
  GlobalInfo g;
  if(!SetGlobals(Arguments, g))
    return;
    
  juce::initialiseJuce_NonGUI();
  
  FFTMultithread* fftm = 0;
  Wisdom w;
  DoWisdom(w, g, fftm);
  
  FileIO fio;
  Parameters p;
  
  //Set the parameters.
  if(g.Files.n() != 2)
    return;

  p.InputFilename = g.Files[0];
  p.OutputFilename = g.Files[1];
  
  String v;
  count i;
  
  if(p.OutputFilename.Suffix(4) == ".png")
  {
    p.MakeSpectrogram = true;
    p.SpectrogramFormat = "png";
  }
  else if(p.OutputFilename.Suffix(4) == ".jpg")
  {
    p.MakeSpectrogram = true;
    p.SpectrogramFormat = "jpg";
  }
  
  if(p.InputFilename.Suffix(4) == ".raw")
  {
    if(!p.MakeSpectrogram)
    {
      if(!g.IsSpecified("inputsamplerate") ||
        !g.IsSpecified("inputsampleformat") || !g.IsSpecified("inputchannels"))
      {
        c += "To read raw input files you must specify --inputsamplerate, "
          "--inputsampleformat, and --inputchannels.";
        return;
      }
    }
    else
    {
      if(!g.IsSpecified("inputsampleformat") || !g.IsSpecified("inputchannels"))
      {
        c += "To read raw input files for spectrogram analysis you must specify"
          " --inputsampleformat and --inputchannels";
        return;
      }
    }
    
    v = g.GetValue("inputsampleformat");
    if(v == "int8" || v == "int16" || v == "int24" || v == "int32" ||
      v == "float32" || v == "float64")
    {
      p.InputSampleFormat = v;
    }
    else
    {
      c += "Input sample format not understood. Must be one of: [int8 ";
      c &= "int16 int24 int32 float32 float64]";
      return;
    }
    
    p.IsRaw = true;

    if(!p.MakeSpectrogram)
    {
      v = g.GetValue("inputsamplerate");
      if(!v.Find("Hz", i))
      {
        c += "Input sample rate must be specified in Hz, i.e. 44100Hz";
        return;
      }
      v.Replace("Hz", "");
      p.InputSampleRate = v.ToInteger();
      if(p.InputSampleRate < 0)
      {
        c += "Input sample rate is not a valid integer.";
        return;
      }
      
      v = g.GetValue("inputchannels");
      if(v == String(v.ToInteger()) && v.ToInteger() > 0 && v.ToInteger() < 128)
      {
        p.InputChannels = v.ToInteger();
      }
      else
      {
        c += "Input channels not understood. Must be an integer 1 to 128.";
        return;
      }
    }
  }
  
  v = g.GetValue("convolve");
  if(v)
  {
    if(p.MakeSpectrogram)
    {
      c += "Convolution is incompatible with the spectrogram feature. First "
        "convolve the file to a new file, and then create spectrogram from the "
        "new file.";
      return;
    }
    if(v.Suffix(4) == ".raw")
    {
      c += "Convolution currently does not support raw input. First convert the"
        " raw data to an audio format, and then proceed with convolution.";
      return;
    }
    if(!juce::File(v.Merge()).existsAsFile())
    {
      c += "The impulse response file '"; c &= v; c &= "' could not be found.";
      return;
    }
      
    p.ConvolveFilename = juce::File(v.Merge()).getFullPathName().toUTF8();
  }
  
  
  v = g.GetValue("samplerate");
  if(v == "keep")
    p.OutputSampleRate = 0;
  else
  {
    if(!v)
      v = "0Hz";
      
    if(!v.Find("Hz", i))
    {
      c += "Sample rate must be specified in Hz, i.e. 44100Hz";
      return;
    }
    v.Replace("Hz", "");
    p.OutputSampleRate = v.ToInteger();
  }
  
  v = g.GetValue("sampleformat");
  if(v == "int8" || v == "int16" || v == "int24" || v == "int32" ||
    v == "float32" || v == "float64" || v == "keep" || v == "")
  {
    if(v == "keep") v = "";
    p.OutputSampleFormat = v;
  }
  else
  {
    c += "Output sample format not understood. Must be one of: [int8 ";
    c &= "int16 int24 int32 float32 float64 keep]";
    return;
  }
  
  v = g.GetValue("pitchshift");
  p.PitchShift = v;
  
  v = g.GetValue("centstolerance");
  if(!v)
    v = "0.1";
  p.CentsTolerance = v.ToNumber();
  
  v = g.GetValue("dither");
  if(v == "triangle" || v == "")
    v = "triangle";
  else if(v == "rectangle")
    v = "rectangle";
  else
    v = "";
  p.DitherType = v;
  
  v = g.GetValue("ditherbits");
  if(!v)
    v = "1.0";
  p.DitherBits = v.ToNumber();
  if(p.DitherBits < 0 || p.DitherBits > 1.0)
  {
    c += "Dither bits must be between 0.0 and 1.0";
    return;
  }
  
  v = g.GetValue("allowablebandwidthloss");
  if(!v)
    v = "0.1%";
  if(!v.Find("%", i))
  {
    c += "Allowable bandwidth loss must be specified as a percentage, "
      "i.e. 0.1%";
    return;
  }
  v.Replace("%", "");
  p.AllowableBandwidthLoss = v.ToNumber();
  if(p.AllowableBandwidthLoss <= 0 || p.AllowableBandwidthLoss >= 50)
  {
    c += "Allowable bandwidth loss must be a percentage greater than 0%, "
      "and less than 50%. Typical: 0.1%";
    return;
  }
  p.AllowableBandwidthLoss *= 0.01;
  
  
  v = g.GetValue("depth");
  if(!v)
    v = "200dB";
  if(!v.Find("dB", i))
  {
    c += "Depth must be specified in dB, "
      "i.e. 200dB";
    return;
  }
  v.Replace("dB", "");
  p.StopbandAttenuation = v.ToNumber();
  if(p.StopbandAttenuation < 6 || p.StopbandAttenuation > 300)
  {
    c += "Depth must be at least 6dB and at most 300dB. "
      "Typical value is 200dB.";
    return;
  }
  
  //Spectrogram parameters
  v = g.GetValue("spectrogramsize");
  if(!v)
    v = "4096";
  p.SpectrogramSize = v.ToInteger();
  if(p.SpectrogramSize < 128 || p.SpectrogramSize > 65536)
  {
    c += "Spectrogram step must be between 128 and 65536. Powers of two and "
      "powers of small primes be faster to calculate.";
    return;
  }
  
  v = g.GetValue("spectrogramstep");
  if(!v)
    v = "1000";
  p.SpectrogramStep = v.ToInteger();
  if(p.SpectrogramStep < 1 || p.SpectrogramStep > p.SpectrogramSize)
  {
    c += "Spectrogram step must be between 1 and the spectrogram size.";
    return;
  }
  
  v = g.GetValue("spectrogrambeta");
  if(!v)
    v = "35.0";
  p.SpectrogramBeta = v.ToNumber();
  if(p.SpectrogramBeta < 5 || p.SpectrogramBeta > 40)
  {
    c += "Spectrogram beta must be between 5.0 and 40.0. Typical value is "
      "35.0 (190 dB dynamic range).";
    return;
  }

  v = g.GetValue("gradient");
  if(!v)
    v = "gray";
  
  if(v == "grey") v = "gray";
  if(v == "colour") v = "color";
  
  if(v != "gray" && v != "color")
  {
    c += "Gradient type must be one of: [gray/grey color/colour]";
  }
  p.Gradient = v;

  v = g.GetValue("gradientrange");
  if(!v)
    v = "default";
  if(v == "default")
  {
    if(p.Gradient == "color")
      v = "180.0dB";
    else
      v = "255.0dB";
  }
  if(!v.Find("dB", i))
  {
    c += "Gradient range must be specified in dB, i.e. 200dB";
    return;
  }
  v.Replace("dB", "");
  p.GradientRange = v.ToNumber();
  if(p.GradientRange < 6 || p.GradientRange > 300)
  {
    c += "Gradient range must be at least 6dB and at most 300dB. "
      "Typical value is 180dB.";
    return;
  }
  
  v = g.GetValue("exportfilter");
  if(v)
  {
    if(v.Suffix(4) != ".fft")
    {
      c += "Export filter must use a .fft extension. The result will be complex"
        " float64 pairs of the current filter's frequency response.";
      return;
    }
      
    p.ExportFilterFilename = juce::File(v.Merge()).getFullPathName().toUTF8();
  }
  
  p.SkipFilter = false;
  if(g.IsSpecified("nofilter"))
    p.SkipFilter = true;
  
  
  //Begin timer.
  prim::float64 StartTick = juce::Time::getMillisecondCounterHiRes();
  
  //Finally convert the file.
  fio.Go(p);
  
  //Check timer.
  prim::float64 EndTick = juce::Time::getMillisecondCounterHiRes();
  prim::float64 TicksElapsed = (prim::float64)(EndTick - StartTick);
  prim::float64 TicksPerSecond = (prim::float64)1000.0;
  prim::float64 SecondsElapsed = TicksElapsed / TicksPerSecond;
  c += "Operation took "; c &= (number)SecondsElapsed;
  c &= " seconds";
  
  delete fftm;
}
