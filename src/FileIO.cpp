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
#include "Kaiser.h"
#include "Render.h"
#include "Parameters.h"

String FileIO::GetFormat(SF_INFO& s_info, String* Description)
{
  String Temp;
  if(!Description) Description = &Temp;
  String& d = *Description;
  switch(s_info.format & SF_FORMAT_TYPEMASK)
  {
    case SF_FORMAT_WAV: d = "Microsoft WAV format (little endian)";return "WAV";
    case SF_FORMAT_AIFF: d = "Apple/SGI AIFF format (big endian)";return "AIFF";
    case SF_FORMAT_AU: d = "Sun/NeXT AU format (big endian)"; return "AU";
    case SF_FORMAT_RAW: d = "RAW PCM data"; return "RAW";
    case SF_FORMAT_PAF: d = "Ensoniq PARIS file format"; return "PAF";
    case SF_FORMAT_SVX: d = "Amiga IFF / SVX8 / SV16 format"; return "SVX";
    case SF_FORMAT_NIST: d = "Sphere NIST format"; return "NIST";
    case SF_FORMAT_VOC: d = "VOC files"; return "VOC";
    case SF_FORMAT_IRCAM: d = "Berkeley/IRCAM/CARL"; return "IRCAM";
    case SF_FORMAT_W64: d = "Sonic Foundry's 64 bit RIFF/WAV"; return "W64";
    case SF_FORMAT_MAT4: d = "Matlab (tm) V4.2 / GNU Octave 2.0"; return "MAT4";
    case SF_FORMAT_MAT5: d = "Matlab (tm) V5.0 / GNU Octave 2.1"; return "MAT5";
    case SF_FORMAT_PVF: d = "Portable Voice Format"; return "PVF";
    case SF_FORMAT_XI: d = "Fasttracker 2 Extended Instrument"; return "XI";
    case SF_FORMAT_HTK: d = "HMM Tool Kit format"; return "HTK";
    case SF_FORMAT_SDS: d = "Midi Sample Dump Standard"; return "SDS";
    case SF_FORMAT_AVR: d = "Audio Visual Research"; return "AVR";
    case SF_FORMAT_WAVEX: d = "MS WAVE with WAVEFORMATEX"; return "WAVEX";
    case SF_FORMAT_SD2: d = "Sound Designer 2"; return "SD2";
    case SF_FORMAT_FLAC: d = "FLAC lossless file format"; return "FLAC";
    case SF_FORMAT_CAF: d = "Core Audio File format"; return "CAF";
    case SF_FORMAT_WVE: d = "Psion WVE format"; return "WVE";
    case SF_FORMAT_OGG: d = "Xiph OGG container"; return "OGG";
    case SF_FORMAT_MPC2K: d = "Akai MPC 2000 sampler"; return "MPC2K";
    case SF_FORMAT_RF64: d = "RF64 WAV file"; return "RF64";
  }
  return "Unsupported";
}

String FileIO::GetSampleType(SF_INFO& s_info, String* Description)
{
  String Temp;
  if(!Description) Description = &Temp;
  String& d = *Description;
  switch(s_info.format & SF_FORMAT_SUBMASK)
  {
    case SF_FORMAT_PCM_S8: d = "8-bit signed PCM"; return "int8";
    case SF_FORMAT_PCM_16: d = "16-bit signed PCM"; return "int16";
    case SF_FORMAT_PCM_24: d = "24-bit signed PCM"; return "int24";
    case SF_FORMAT_PCM_32: d = "32-bit signed PCM"; return "int32";
    case SF_FORMAT_PCM_U8: d = "8-bit unsigned PCM"; return "uint8";
    case SF_FORMAT_FLOAT: d = "32-bit float PCM"; return "float32";
    case SF_FORMAT_DOUBLE: d = "64-bit float PCM"; return "float64";
    case SF_FORMAT_ULAW: d = "U-law encoded"; return "U-law";
    case SF_FORMAT_ALAW: d = "A-law encoded"; return "A-law";
    case SF_FORMAT_IMA_ADPCM: d = "IMA ADPCM"; return "IMA ADPCM";
    case SF_FORMAT_MS_ADPCM: d = "Microsoft ADPCM"; return "Microsoft ADPCM";
    case SF_FORMAT_GSM610: d = "GSM 6.10 encoding"; return "GSM 6.10";
    case SF_FORMAT_VOX_ADPCM: d = "Oki Dialogic ADPCM encoding"; return "Oki ADPCM";
    case SF_FORMAT_G721_32: d = "32 kbps G721 ADPCM encoding"; return "32kbps G721";
    case SF_FORMAT_G723_24: d = "24 kbps G723 ADPCM encoding"; return "24kpbs G723";
    case SF_FORMAT_G723_40: d = "40 kbps G723 ADPCM encoding"; return "40kbps G723";
    case SF_FORMAT_DWVW_12: d = "12-bit delta width variable"; return "12-bit DWVW";
    case SF_FORMAT_DWVW_16: d = "16-bit delta width variable"; return "16-bit DWVW";
    case SF_FORMAT_DWVW_24: d = "24-bit delta width variable"; return "24-bit DWVW";
    case SF_FORMAT_DWVW_N: d = "N-bit delta width variable"; return "N-bit DWVW";
    case SF_FORMAT_DPCM_8: d = "8-bit differential PCM"; return "8-bit DPCM";
    case SF_FORMAT_DPCM_16: d = "16-bit differential PCM"; return "16-bit DPCM";
    case SF_FORMAT_VORBIS: d = "Xiph Vorbis encoding"; return "Vorbis";
  }
  return "Unsupported";
}

String FileIO::CheckFileError(SNDFILE* s)
{
  switch(sf_error(s))
  {
    case SF_ERR_UNRECOGNISED_FORMAT:
    return "Audio file is an unrecognized format";
    
    case SF_ERR_SYSTEM:
    return "Audio file could not be read. Check the path and filename.";
    
    case SF_ERR_MALFORMED_FILE:
    return "Audio file is malformed.";
    
    case SF_ERR_UNSUPPORTED_ENCODING:
    return "Audio file uses an unsupported encoding.";
  }
  return "";
}

int FileIO::GetFormatEnum(String Format)
{
  if(Format == "int8")
    return SF_FORMAT_PCM_S8;
  else if(Format == "int16")
    return SF_FORMAT_PCM_16;
  else if(Format == "int24")
    return SF_FORMAT_PCM_24;
  else if(Format == "int32")
    return SF_FORMAT_PCM_32;
  else if(Format == "float32")
    return SF_FORMAT_FLOAT;
  else if(Format == "float64")
    return SF_FORMAT_DOUBLE;
  else
    return SF_FORMAT_PCM_24;
}

bool FileIO::IsFormatInt(String Format)
{
  if(Format == "int8")
    return true;
  else if(Format == "int16")
    return true;
  else if(Format == "int24")
    return true;
  else if(Format == "int32")
    return true;
  else if(Format == "float32")
    return false;
  else if(Format == "float64")
    return false;
  else
    return false;
}

int FileIO::GetFormatBits(String Format)
{
  if(Format == "int8")
    return 8;
  else if(Format == "int16")
    return 16;
  else if(Format == "int24")
    return 24;
  else if(Format == "int32")
    return 32;
  else if(Format == "float32")
    return 32;
  else if(Format == "float64")
    return 64;
  else
    return 24;
}

int FileIO::GetEffectiveFormatBits(String Format)
{
  if(Format == "int8")
    return 8;
  else if(Format == "int16")
    return 16;
  else if(Format == "int24")
    return 24;
  else if(Format == "int32")
    return 32;
  else if(Format == "float32")
    return 23;
  else if(Format == "float64")
    return 53;
  else
    return 24;
}

math::Ratio FileIO::GetPitchShiftRatio(String p, float64 CentsTolerance)
{
  using namespace prim::math;
  Console c;
  if(p == "")
    return 1;
  count i;
  if(p.Find("/", i))
  {
    Ratio pr = p;
    if(pr.Num() <= 0)
    {
      c += "Ratio '"; c &= p; c &= "' could not be understood. Assuming 1/1.";
      pr = 1;
    }
    return pr;
  }
  else
  {
    Ratio pr;
    float64 Sign = 1;
    if(p[0] == '+')
      p.Replace("+", "");
    else if(p[0] == '-')
    {
      p.Replace("-", "");
      Sign = -1;
    }
    
    float64 S = 0;
    float64 Octave;
    float64 Cents;
    if(p.Find("P1",i)) {S = 0; p.Replace("P1", "|");}
    if(p.Find("m2",i)) {S = 1; p.Replace("m2", "|");}
    if(p.Find("M2",i)) {S = 2; p.Replace("M2", "|");}
    if(p.Find("m3",i)) {S = 3; p.Replace("m3", "|");}
    if(p.Find("M3",i)) {S = 4; p.Replace("M3", "|");}
    if(p.Find("P4",i)) {S = 5; p.Replace("P4", "|");}
    if(p.Find("A4",i)) {S = 6; p.Replace("A4", "|");}
    if(p.Find("D5",i)) {S = 6; p.Replace("D5", "|");}
    if(p.Find("TT",i)) {S = 6; p.Replace("TT", "|");}
    if(p.Find("P5",i)) {S = 7; p.Replace("P5", "|");}
    if(p.Find("m6",i)) {S = 8; p.Replace("m6", "|");}
    if(p.Find("M6",i)) {S = 9; p.Replace("M6", "|");}
    if(p.Find("m7",i)) {S = 10; p.Replace("m7", "|");}
    if(p.Find("M7",i)) {S = 11; p.Replace("M7", "|");}
    p.Find("|", i);
    if(i >= 0 && i < p.n())
    {
      Octave = p.Prefix(i).ToNumber();
      Cents = p.Suffix(p.n() - i - 1).ToNumber() * 100.;
    }
    else
    { Octave = p.ToNumber();
      Cents = 0;
    }
    
    c += "Pitch Shift Information";
    c += "----------------------------------------------------------------------";
    c += "Interval interpreted as: ";
    c &= (Sign == 1 ? "up " : "down ");
    c &= (number) Octave;
    c &= " octave(s), ";
    c &= (number) S;
    c &= " semitone(s), and ";
    c &= (number) Cents;
    c &= " cents";
    
    //Coerce...
    /*
    if(Sign == -1)
    {
      float64 SC = S + Cents / 100.;

      if(SC != 0)
      {
        SC = 12. - SC; //Invert
        S = (int64)SC;
        Cents = (SC - (float64)S) * 100.;
        Octave++; //Add one more octave (downward).
      }
    }*/
    
    float64 IdealCents = Cents + S * 100.;
    float64 BestCentsDeviation = 0;
    int64 BestN = 1, BestD = 1;
    int64 HighestTry = 10000;
    if(IdealCents != 0)
    {
      BestCentsDeviation = 1200.;
      for(int64 d = 1; d < HighestTry; d++)
      {
        for(int64 n = d; n < d * 2; n++)
        {
          number RatioTry = (number)((float64)n / (float64)d);
          number ActualCents = Log(2.f, RatioTry) * 1200.f;
          number Deviation = ActualCents - IdealCents;
          if(Abs(Deviation) < Abs((number)BestCentsDeviation))
          {
            BestN = n;
            BestD = d;
            BestCentsDeviation = Deviation;
          }
          if(Abs(Deviation) <= Abs((number)CentsTolerance))
          {
            BestN = n;
            BestD = d;
            BestCentsDeviation = Deviation;
            d = HighestTry;
            break;
          }
        }
      }
    }
    
    pr = Ratio(BestN, BestD);
    for(int64 i = 0; i < Octave; i++)
      pr *= 2;
    
    if(BestCentsDeviation != 0)
    {
      c += "Coerced interval ratio to: "; c &= pr.ToString(); c &= " (";
      c &= (number)BestCentsDeviation; c &= " cents away)";
    }
    else
    {
      c += "Interval ratio is exactly: "; c &= pr.ToString();
    }
    c++;
    
    if(pr.Num() <= 0)
    {
      c += "Ratio '"; c &= p; c &= "' could not be understood. Assuming 1/1.";
      pr = 1;
    }
    
    if(Sign == 1)
      return pr;
    else
      return Ratio(1, 1) / pr;
  }
  return 1;
}

//=============================BEGIN GRADIENT===================================
class ColorGradient
{
  List<colors::RGB> GradientPoints;
  List<number> GradientWidths;
  
  public:
  
  void AddColor(colors::RGB Color, number Width)
  {
    if(Width <= 0)
      Width = 0.001;
    GradientPoints.Add() = Color;
    GradientWidths.Add() = Width;
  }
  
  colors::RGB GetColorAtPoint(number x)
  {
    if(GradientPoints.n() == 0)
      return colors::Black;
      
    if(x <= 0)
      return GradientPoints.first();
    
    number CurrentPoint = 0;
    number NextPoint = 0;
    colors::RGB c1, c2;
    
    for(count i = 0; i < GradientWidths.n(); i++)
    {
      number CurrentWidth = GradientWidths[i];
      CurrentPoint = NextPoint;
      NextPoint += CurrentWidth;
      if(x >= CurrentPoint && x <= NextPoint)
      {
        number f2 = (x - CurrentPoint) / CurrentWidth;
        number f1 = 1.0f - f2;
        c1 = GradientPoints[i];
        if(i != GradientPoints.n() - 1)
          c2 = GradientPoints[i + 1];
        else
          c2 = GradientPoints.last();
        uint32 r = (uint32)((colors::Component::r(c1) * f1 +
          colors::Component::r(c2) * f2) * 255.0f + 0.5f);
        uint32 g = (uint32)((colors::Component::g(c1) * f1 +
          colors::Component::g(c2) * f2) * 255.0f + 0.5f);
        uint32 b = (uint32)((colors::Component::b(c1) * f1 +
          colors::Component::b(c2) * f2) * 255.0f + 0.5f);
        r <<= 16;
        g <<= 8;
        
        return r + g + b;
      }
    }
    return GradientPoints.last();
  }
};

static unsigned int ColorGradientWidth = 304;

#define GET_GRADIENT_PIXEL(data, pixel) {\
pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
data += 4; \
}

static const char *ColorGradientData =
	"]P`L]P`K]P`J]P`H]@`G]@`F]@`E]@`C]@`B]@`A]@`?]@`>]@`=]0\\;]0\\:]0\\9"
	"]0\\8]0\\6]0\\5]0\\4]0\\2\\`\\1\\`\\0\\`\\.\\`\\-\\`\\,]0\\)]0X']@X$]@X\"]PS_]`S]"
	"]`OZ^0OX^0OU^@KS^PKP^PKN^`GK^`GI_0GF_@CD_@CA_PC?_P?<_`?:`0;7`0;5"
	"`@;2`@70`P7-`P/+`P#)`OW&`OK$`O?\"`O6_`O*]`N^[`NRX`NFV`N>T`N2Q`>&O"
	"`=ZM`=NJ`=FH`=:F`=.C`=\"A`<V?`<N<`<B:`<68`<*5`;^3`;R1`;B/`;2-`;\"+"
	"`:R)`:B'`:2%`:\"#`9R!`9A_`91]`9![`8QY`(AW`(1U`(!S`'QQ`'AO`'1M`'!L"
	"`&QJ`&AH`&1F`&!D`%QB`%E@_U9A_E-A_5!B_$UB^TMC^DAD^45D^4)E^$!E]SUF"
	"]CIG]3=G]#5H\\S)H\\B]I\\2QJ\\\"IJ\\\"=K[R1K[B%L[1]M[!QMZQENZA9NZ1-OYQ-Q"
	"YA-SY!-UXA-WX!-ZWA-\\W!-^VA-`V1.#UQ.%U1.'TQ.)T1.+SQ..S1.0S!.2RA.4"
	"R!.6QA.9Q!.;PA.=P!.?OQ.AO1.DNQ.FN1.HN!.IMA.JM1.KLQ.MLA.NL!.OKQ.P"
	"K1.QK!.SJA.TJ1.UIQ.VIA.XI!.YHQ.ZH1.[H!.\\GA.^G1._FQ.`FA/!F!/#EQ/$"
	"E1/%E!/&D1/%CQ/$C!/#B1/\"AQ/!A!.`@A._?Q._?1.^>A.]>!.\\=1.[<Q*Z<!*Y"
	";A*X:Q*W:1*V9A*V9!*U81*T7Q*S7!*R61*Q5Q*P5!*O4A*N4!*M3A*K3!*J2A*I"
	"2!*H1A*F1!*E0A*D0!*C/A*A/!*@.A*?.!*>-A*<-!*;,1*:+Q*8+1*7*Q*6*1*5"
	")Q*3)1*2(Q*1(1*0'Q*/'A*.'1*-'!*,&Q*+&A**&1*)&!*(%Q*'%A*&%1*%%!*$"
	"$Q*#$A.\"$1.!#Q-`#A-_#1-^#!-]\"Q-\\\"A-[\"1-Z\"!-Y!Q-X!A-W!1-V!1-U!1-T"
	"!1-S!1-R!1-Q!1-P!1-O!1-O!1-N!1-M!1-L!1-K!1-J!1-I!1-H!1-G!1-F!1-F"
	"!1-E!1-D!1-C!1-B!1-A!1-@!1-?!1->!1-=!1-<!1-:!1)9!1)8!1)7!1)6!1)5"
	"!1)3!1)2!1)1!!%0!!%/!!%-!!%,!!%+!!%*!!%)!!%(!!%&!!!%!!!$!!!#!!!\""
	"";
//==============================END GRADIENT====================================

void FileIO::MakeSpectrogram(Parameters& p, SNDFILE* s)
{
  Console c;

  //Parameters
  int64 Size = p.SpectrogramSize;
  float64 Beta = p.SpectrogramBeta;
  integer Step = p.SpectrogramStep;
  integer Frames = (integer)p.Frames / Step + 1;
  
  //Create the gradient.
  ColorGradient cg;
  if(p.Gradient == "color" && p.Channels == 1)
  {
    uint32 pix[3];
    for(count i = 0; i < ColorGradientWidth; i++)
    {
      GET_GRADIENT_PIXEL(ColorGradientData, pix);
      cg.AddColor((pix[0] << 16) + (pix[1] << 8) + (pix[2] << 0),
        (number)1.0f / (number)ColorGradientWidth * p.GradientRange);
    }
  }
  else// if(p.Gradient == "gray")
  {
    cg.AddColor(prim::colors::Black, p.GradientRange);
    cg.AddColor(prim::colors::White, 0.1f);
  }
  
  //The analysis objects.
  Array<float64> Input, Window;
  AudioFFT FFT;
  Kaiser K;
  
  //Initialize the FFT and the Kaiser window.
  FFT.Initialize(Size, FFTW_PATIENT, 0, true);
  K.Initialize(Size, Beta);
  float64 WindowPower = K.CreateWindow(Window);
  
  //Adjust for the power lost in the window.
  for(count i = 0; i < Window.n(); i++)
    Window[i] /= WindowPower;
  
  //Size the input.
  Input.n(p.Channels * Size);
  
  //Initialize loop variables.
  int64 CopyFrames = Size;
  int64 ImageHeight = Size / 2 + 1;
  int64 CurrentPercent = 0, PreviousPercent = 0;
  
  Array<juce::Colour> slice;
  slice.n((count)ImageHeight);
  
  //Create the bitmap.
  juce::Image img(juce::Image::RGB, Frames, Size / 2 + 1, true);
  
  //Step through the audio file and create analysis columns at each step.
  c += "Analyzing..."; std::cout.flush();
  for(count Frame = 0; Frame < Frames;
    Frame++)
  {
    //Display progress at each percent.
    CurrentPercent = Frame * 100 / Frames;
    if(CurrentPercent > PreviousPercent)
    {
      PreviousPercent = CurrentPercent;
      c &= (integer)CurrentPercent; c &= "%...";
      std::cout.flush();
    }
    
    //Manually seek to the correct location.
    sf_seek(s, Frame * Step, SEEK_SET);
    
    //Read the frames.
    sf_readf_double(s, &Input[0], (sf_count_t)CopyFrames);
    
    
    count k_max = 1;
    if(p.Channels == 2)
      k_max = 3;
    for(count k = 0; k < k_max; k++)
    {
      //Apply the window and transfer to the FFT.
      count chan = p.Channels;
      
      if(k < 2)
      {
        for(count i = 0; i < Size; i++)
          FFT.Time(i, Input[i * chan + k] * Window[i]);
      }
      else if(k == 2)
      {
        for(count i = 0; i < Size; i++)
          FFT.Time(i, (Input[i * chan] - Input[i * chan + 1]) * Window[i]);
      }
        
      //Perform the FFT.
      FFT.TimeToFreq();
      
      //Copy FFT results to column of image.
      for(count j = 0; j < ImageHeight; j++)
      {
        //Calculate the attenuation in dB.
        float64 dBAtten = math::Abs(math::Log(10.0f, FFT.Mag(j)) * -20.0f);
        //Gradient can be displayed using:
        //dBAtten = p.GradientRange - (number)j / 
        //  (number)ImageHeight * p.GradientRange;
        
        
        if(k_max == 3)
        {
          dBAtten *= 2;
        }
        
        //Calculate the gradient.
        colors::RGB rgb = cg.GetColorAtPoint(dBAtten);
        uint8 r = (uint8)((rgb >> 16) % 256);
        uint8 g = (uint8)((rgb >> 8) % 256);
        uint8 b = (uint8)(rgb % 256);
        
        if(k_max == 3)
        {
          if(k == 0)
          {
            b = g = 0;
            r = 255 - r;
          }
          else if(k == 1)
          {
            r = g = 0;
            b = 255 - b;
          }
          else
          {
            r = slice[j].getRed();
            b = slice[j].getBlue();
            if(r > b)
              g = r - b;
            else
              g = b - r;
            r = b = 0;
          }
        }
        //Create a JUCE color.
        juce::Colour color = juce::Colour::fromRGBA(r, g, b, (uint8)255);
        
        if(k == 0)
          slice[j] = juce::Colours::black;
        slice[j] = juce::Colour(slice[j].getARGB() + color.getARGB());
      }
    }
    for(count j = 0; j < ImageHeight; j++)
    {
      //Set the pixel.
      img.setPixelAt(Frame, ImageHeight - 1 - j, slice[j]);
    }
  }
  
  //Delete any existing file first.
  juce::File f(p.OutputFilename.Merge());
  f.deleteFile();
  juce::FileOutputStream fos(f);
  
  //Write spectrogram to the appropriate file.
  c += "Writing spectrogram to '"; c &= p.OutputFilename;
  c &= "'..."; std::cout.flush();
  if(p.SpectrogramFormat == "png")
  {
    juce::PNGImageFormat png;
    png.writeImageToStream(img, fos);
  }
  else if(p.SpectrogramFormat == "jpg")
  {
    juce::JPEGImageFormat jpeg;
    jpeg.setQuality(1.0f);
    jpeg.writeImageToStream(img, fos);
  }
}

void FileIO::Go(Parameters& p)
{
  Console c;
  
  //Set up file info structures.
  SF_INFO s_info, s_out_info, s_scratch_info;
  Memory::ClearObject(s_info);
  Memory::ClearObject(s_out_info);
  Memory::ClearObject(s_scratch_info);
  Memory::ClearObject(p.ConvolveInfo);
  p.ConvolveHandle = 0;
  
  /*Make absolutely certain the input file is not the ouput file. Brick should
  never, ever work with in = out.*/
  if(juce::File(p.InputFilename.Merge()) ==
    juce::File(p.OutputFilename.Merge()))
  {
    c += "Input file is the same as the output file. Only non-destructive ";
    c &= "operation is allowed.";
    return;
  }
  
  //Open the input file.
  if(p.IsRaw)
  {
    if(p.MakeSpectrogram)
      s_info.samplerate = 44100; //Doesn't matter for spectrogram.
    else
      s_info.samplerate = p.InputSampleRate;
    s_info.channels = p.InputChannels;
    s_info.format = SF_FORMAT_RAW + SF_ENDIAN_CPU +
      GetFormatEnum(p.InputSampleFormat);
  }
  
  c += "Opening '"; c &= p.InputFilename; c &= "' for reading...";
  SNDFILE* s = sf_open(p.InputFilename, SFM_READ, &s_info);
  String s_error = CheckFileError(s);
  if(s_error)
  {
    c += s_error;
    return;
  }
  c++;
  
  /*Turning on normalization for reading will ensure that when importing integer
  formats, they are first divided by a power of two to put the data in the range
  of [-1.0, 1.0]. For writing, we will do our own normalization since libsndfile
  has a weird way of converting back to floats.*/
  sf_command(s, SFC_SET_NORM_DOUBLE, 0, SF_TRUE);
  
  //Get description of input file.
  c += "Input Information";
  c += "----------------------------------------------------------------------";
  c += "Frames: "; c &= (integer)s_info.frames;
  c += "Sample Rate: "; c &= (integer)s_info.samplerate;
  c += "Channels: "; c &= (integer)s_info.channels;
  
  String format, format_desc;
  format = GetFormat(s_info, &format_desc);
  c += "Format Type: "; c &= format;
  c += "Format Description: "; c &= format_desc;
  
  String sampletype, sampletype_desc;
  sampletype = GetSampleType(s_info, &sampletype_desc);
  c += "Sample Type: "; c &= sampletype;
  c += "Sample Type Description: "; c &= sampletype_desc;
  c++;
  
  //Set the basics.
  p.Channels = s_info.channels;
  p.Frames = s_info.frames;
  p.OldSampleRate = s_info.samplerate;
  
  //If doing a spectrogram, fork off to do that here.
  if(p.MakeSpectrogram)
  {
    if(s_info.channels > 2)
    {
      c += "Spectrograms can currently only be created from monoaural "
        "and stereo audio files.";
      return;
    }
    MakeSpectrogram(p, s);
    sf_close(s);
    return;
  }
  
  //Calculate the sample rate ratio
  math::Ratio SampleRate;
  if(p.OutputSampleRate <= 0)
    p.OutputSampleRate = s_info.samplerate;
    
  SampleRate = p.OutputSampleRate;
  SampleRate = SampleRate / s_info.samplerate;

  math::Ratio PitchRate = GetPitchShiftRatio(p.PitchShift, p.CentsTolerance);
  math::Ratio TotalRateChange = SampleRate / PitchRate;
  p.P = TotalRateChange.Num();
  p.Q = TotalRateChange.Den();
  if((p.P == 1 && p.Q == 1) && !(p.ConvolveFilename))
    p.SkipFilter = true;

  //Calculate maximum FFT size.
  int Megs = juce::SystemStats::getMemorySizeInMegabytes();
  p.MaxFFTSize = (int64)(math::Log(2.0, (float64)Megs) + 0.1) - 6 + 20;
  if(p.MaxFFTSize > 26)
    p.MaxFFTSize = 26; //FFTW will not uses sizes higher due to malloc failing.

  //Set other parameters.
  p.BCOptimizationLevel = 2;
  p.NewSampleRate = p.OutputSampleRate;
  
  /*If keeping the input sample format, then copy the input format to the
  output format.*/
  if(!p.OutputSampleFormat)
    p.OutputSampleFormat = sampletype;
  p.OutFormat = p.OutputSampleFormat;
  
  //Set convolution.
  if(p.ConvolveFilename)
  {
    p.ConvolveHandle = sf_open(p.ConvolveFilename, SFM_READ, &p.ConvolveInfo);
    String s_error = CheckFileError(p.ConvolveHandle);
    if(s_error)
    {
      c += s_error;
      sf_close(s);
      return;
    }
    if(p.ConvolveInfo.channels > 1)
    {
      c += "Currently only monoaural (one-channel) impulse responses are "
        "supported for convolution. The one-channel impulse response is applied"
        " to each channel of input to create each channel of output. Try using "
        "a mono impulse response.";
      return;
    }
    sf_command(p.ConvolveHandle, SFC_SET_NORM_DOUBLE, 0, SF_TRUE);
  }
  
  if(!p.SkipFilter && !p.InitializeDerivedParameters())
  {
    sf_close(s);
    return;
  }
  
  //Print resampling information.
  c += "Resample Information";
  c += "----------------------------------------------------------------------";
  c += "Upsample by: "; c &= p.P;
  c += "Downsample by: "; c &= p.Q; 
  c += "Filtering: "; c &= (!p.SkipFilter ? "yes" : "no");
  c += "Convolving: "; c &= (p.ConvolveFilename ? "yes" : "no");
  if(!p.SkipFilter)
  {
    c += "Allowable Bandwidth Loss: ";
    c &= (number)p.AllowableBandwidthLoss * 100.f; c &= "%";
    c += "Stopband Attenuation: "; c &= (number)p.StopbandAttenuation;
      c &= "dB";
    c += "Scratch File Size: "; c &= (number)p.ScratchFileSize /
      (number)(1024 * 1024); c &= " MB";
    c += "Filter Length: "; c &= p.idealM;
    c += "Passes: "; c &= p.S;
    c += "FFT Size: "; c &= (number)p.FFTSize / (number)1024; c &= " K";
  }
  c++;
  
  c += "Working";
  c += "----------------------------------------------------------------------";
  
  //Open the scratch file.
  String ScratchFilename;
  juce::File ScratchFile = juce::File::createTempFile(".aiff");
  ScratchFilename = ScratchFile.getFullPathName().toUTF8();
  c += "Opening scratch file '"; c &= ScratchFilename;
  c &= "' for reading/writing...";
  s_scratch_info.samplerate = p.NewSampleRate;
  s_scratch_info.channels = p.Channels;
  s_scratch_info.format = SF_FORMAT_DOUBLE | SF_FORMAT_AIFF;
  SNDFILE* s_scratch = sf_open(ScratchFilename, SFM_RDWR, &s_scratch_info);
  String s_scratch_error = CheckFileError(s_scratch);
  if(s_scratch_error)
  {
    c += s_scratch_error;
    sf_close(s);
    ScratchFile.deleteFile();
    return;
  }
  
  /*Zero out the scratch file, or if no filter is being used, just copy the
  audio data from the input file into the scratch file.*/
  
  if(!p.SkipFilter)
  {
    int64 BlankFrames = 1024 * 128;
    float64* BlankMemory = new float64[p.Channels * BlankFrames];
    Memory::ClearArray(BlankMemory, p.Channels * BlankFrames);
    int64 FramesLeft = p.OutPQFrames;
    while(FramesLeft > 0)
    {
      if(FramesLeft < BlankFrames)
        BlankFrames = FramesLeft;
      FramesLeft -= BlankFrames;
      sf_writef_double(s_scratch, BlankMemory, (sf_count_t)BlankFrames);
    }
    delete [] BlankMemory;
  }
  else
  {
    //Copy data into scratch file.
    int64 CopyFrames = 1024 * 128;
    float64* CopyMemory = new float64[p.Channels * CopyFrames];
    int64 FramesRead = 0;
    do
    {
      FramesRead = sf_readf_double(s, CopyMemory, (sf_count_t)CopyFrames);
      sf_writef_double(s_scratch, CopyMemory, (sf_count_t)FramesRead);
    } while(FramesRead > 0);
    delete [] CopyMemory;
  }
  
  //Open the output file.
  c += "Opening '"; c &= p.OutputFilename; c &= "' for writing...";
  s_out_info.samplerate = p.NewSampleRate;
  s_out_info.channels = p.Channels;
  s_out_info.format = GetFormatEnum(p.OutFormat);
  if(p.OutputFilename.Suffix(4) == ".wav")
    s_out_info.format = s_out_info.format | SF_FORMAT_WAV;
  else if(p.OutputFilename.Suffix(5) == ".aiff")
    s_out_info.format = s_out_info.format | SF_FORMAT_AIFF;
  else if(p.OutputFilename.Suffix(3) == ".au")
    s_out_info.format = s_out_info.format | SF_FORMAT_AU;
  else if(p.OutputFilename.Suffix(4) == ".raw")
    s_out_info.format = s_out_info.format | SF_FORMAT_RAW | SF_ENDIAN_CPU;
  else
    s_out_info.format = s_out_info.format | SF_FORMAT_AIFF;
  File::Replace(p.OutputFilename, ""); //Zero out the file.
  SNDFILE* s_out = sf_open(p.OutputFilename, SFM_WRITE, &s_out_info);
    String s_out_error = CheckFileError(s_out);
  if(s_out_error)
  {
    c += s_out_error;
    sf_close(s);
    sf_close(s_scratch);
    ScratchFile.deleteFile();
    return;
  }
  
  //We'll handle our own output clipping.
  sf_command(s_out, SFC_SET_CLIPPING, 0, SF_FALSE);
  
  //Resample!
  if(!p.SkipFilter)
  {
    Renderer R;
    R.Initialize(&p);
    R.Go(s, s_scratch);
  }
  
  //Copy the scratch file to the output file.
  c += "Writing scratch file to output.";
  int64 FramesRead;
  int64 NumChannels = p.Channels;
  int64 NumFramesPerChunk = 1024 * 128;
  InitializeChunks(NumFramesPerChunk, NumChannels, IsFormatInt(p.OutFormat),
     GetFormatBits(p.OutFormat));
  bool IsIntegerFormat = IsFormatInt(p.OutFormat);
  
  /*Semi-disable dithering if we are only upconverting. Note: in weird cases,
  like float32 -> int32, it is still a good idea to at least do a rectangular
  dither because a very small signal would still need dithering upon
  conversion. By forcing triangle to be rectangle, we eliminate the possibility
  of integer to integer upconversions resulting in changes. Upconversion should
  always be bit transparent.*/
  if(p.SkipFilter && GetEffectiveFormatBits(p.OutputSampleFormat) >=
      GetEffectiveFormatBits(sampletype))
  {
    if(p.DitherType == "triangle")
      p.DitherType = "rectangle";
    if(p.DitherBits >= 0.99)
      p.DitherBits = 0.99; //Just to make sure we don't catch the end of the
      //interval and jump up or down an integer.
  }
  
  
  //Determine the peak value.
  float64 MostPositiveValue = 0.0;
  float64 MostNegativeValue = 0.0;
  bool UsedNormalization = false;
  sf_seek(s_scratch, 0, SEEK_SET);
  do
  {
    //Read in a block from the scratch file.
    FramesRead = (int64)sf_readf_double(
      s_scratch, OutChunk, (sf_count_t)NumFramesPerChunk);
    int64 SamplesRead = FramesRead * NumChannels;
    
    //Look for peak.
    for(count i = 0; i < SamplesRead; i++)
    {
      float64 Value = OutChunk[i];
      if(Value < MostNegativeValue) MostNegativeValue = Value;
      if(Value > MostPositiveValue) MostPositiveValue = Value;
    }
      
  } while(FramesRead > 0);
  
  float64 Amplification1 = 0, Amplification2 = 0;
  
  if(MostNegativeValue != 0.)
    Amplification1 = GetNormalizedMinValue() / MostNegativeValue;
  if(MostPositiveValue != 0.)
    Amplification2 = GetNormalizedMaxValue() / MostPositiveValue;
    
  float64 Amplification = math::Min(Amplification1, Amplification2);
  if(Amplification >= 1.0)
    Amplification = 1.0;
  else
    UsedNormalization = true;
  
  sf_seek(s_scratch, 0, SEEK_SET);
  do
  {
    //Read in a block from the scratch file.
    FramesRead = (int64)sf_readf_double(
      s_scratch, OutChunk, (sf_count_t)NumFramesPerChunk);
    int64 SamplesRead = FramesRead * NumChannels;
    
    //Zero out any portion that was not read.
    int64 ChunkSamples = NumFramesPerChunk * NumChannels;
    Memory::ClearArray(&OutChunk[SamplesRead], ChunkSamples - SamplesRead);
    
    if(IsIntegerFormat)
    {
      //Convert to integer format.
      OutChunkToOutChunkInt(p.DitherType, p.DitherBits, Amplification);
      
      //Write the data to file.
      sf_writef_int(s_out, OutChunkInt, (sf_count_t)FramesRead);
    }
    else
    {
      //Check for clipping in the normalized double.
      float64* ptr_OutChunkEnd = &OutChunk[ChunkSamples - 1];
      for(float64* ptr_OutChunk = OutChunk;
        ptr_OutChunk < ptr_OutChunkEnd - 1; ptr_OutChunk++)
      {
        float64 Value = *ptr_OutChunk;
        if(Value < -1.0 || Value > 1.0)
          Clipped = true;
      }
      
      //Write the data to file.
      sf_writef_double(s_out, OutChunk, (sf_count_t)FramesRead);
    }
      
  } while(FramesRead > 0);
  
  //Warn about clipping.
  if(UsedNormalization)
  {
    c += "Warning: the waveform was normalized to prevent clipping by "
      "attenuating by ";
    c &= (number)math::Log(10, Amplification) * 20.0f;
    c &= " dB.";
  }
  else if(Clipped)
    c += "Warning: the waveform clipped.";
  
  //Close the files.
  //c += "Closing files.";
  sf_close(s);
  sf_close(s_out);
  sf_close(s_scratch);
  ScratchFile.deleteFile();
  juce::File::getSpecialLocation(juce::File::tempDirectory).deleteRecursively();
  c += "Finished.";
}

void FileIO::GetNormalizationScaleAndBitShift(float64& Scale, int64& BitShift)
{
  if(IntBits == 32)
  {
    Scale = 256. * 256. * 256. * 128.;
    BitShift = 1;
  }
  else if(IntBits == 24)
  {
    Scale = 256. * 256. * 128.;
    BitShift = 256;
  }
  else if(IntBits == 16)
  {
    Scale = 256. * 128.;
    BitShift = (int32)256 * (int32)256;
  }
  else if(IntBits == 8)
  {
    Scale = 128.;
    BitShift = (int32)256 * (int32)256 * (int32)256;
  }
  else
  {
    Scale = 1.;
    BitShift = 1.;
  }
}

float64 FileIO::GetNormalizedMinValue(void)
{
  return -1.;
}

float64 FileIO::GetNormalizedMaxValue(void)
{
  float64 Scale;
  int64 BitShift;
  if(OutputIsIntType)
  {
    GetNormalizationScaleAndBitShift(Scale, BitShift);
    return 1. - 1. / Scale;
  }
  else
    return 1.;
}

void FileIO::OutChunkToOutChunkInt(String Dither, float64 DitherBits,
  float64 NormalizationScale)
{
  //Precalculate some constants.
  float64 Scale;
  int64 Shift;
  GetNormalizationScaleAndBitShift(Scale, Shift);
  
  //These are exact clipping cut-offs for the double representation.
  float64 MinFloatValue = GetNormalizedMinValue();
  float64 MaxFloatValue = GetNormalizedMaxValue();
  
  /*The goal is turn a 64-bit floating-point value into a 32-bit integer. There
  are several stages here in order to do this correctly:
  
  1) Floating-point value must be clipped to the appropriate normalization
  range. Since there are more negative integers than positive integers in an
  int (i.e. -32768 to 32767 for the int16) we must clip the floating-point value
  to [-1, 0.99...] where the upper bound is precisely computed using
  GetNormalizedMaxValue().
  
  2) The floating-point value is scaled to the appropriate rounding range.
  
  3) Dithering is then applied in the form of a bias which is added to the
  floating-point value.
  
  4) The floating-point value is rounded to a 64-bit integer using llrint(). It
  is important to note that llrint() uses a round to even algorithm which is
  statistically uniform. Therefore, 1.25->1, 1.5->2, 2.5->2, 2.75->3. The
  tie-breaker case will practically never occur when dithering is used. Without
  dither, it is possible that a quantized signal plus a 0.5 DC would lose an
  entire LSB in rounding, due to rounding even.
  
  5) The 64-bit integer must be clipped to make sure it does not overflow when
  converted to 32-bits. This can easily happen in triangular dither, and even
  in rectangular dither due to a rare double rounding scenario where
  32767 + [0.5, 0.5) can sometimes equal exactly 32767.5 and round to the
  nearest even 32768 which would cause overflow (if this were a 16-bit int, but
  here we are actually using 32-bit ints).
  
  6) After 32-bit clipping, the 64-bit integer may be cast to a 32-bit integer
  and returned as the final value of the conversion.
  
  Note: clipping is only reported to the user in the case that floating-point
  clipping occurred. Dither clipping is not reported because the same audio may
  not always clip given a different sequence of dither noise.*/
  
  //Initialize the loop.
  int64 Samples = FramesPerChunk * ChannelsPerChunk;
  float64* ptr_OutChunk = &OutChunk[0];
  int32* ptr_OutChunkInt = &OutChunkInt[0];
  
  if(Dither == "rectangle")
  {
    for(int64 i = 0; i < Samples; i++)
    {
      //Perform clipping.
      float64 Value = *ptr_OutChunk * NormalizationScale;
      if(Value < MinFloatValue) {Value = MinFloatValue; Clipped = true;}
      if(Value > MaxFloatValue) {Value = MaxFloatValue; Clipped = true;}
      
      //Dither the double to an int.
      *ptr_OutChunkInt = ClipInt64(RectangularDitherDoubleToInt(
        Value * Scale, DitherBits) * Shift);
      
      //Step pointers.
      ptr_OutChunk++;
      ptr_OutChunkInt++;
    }
  }
  else if(Dither == "triangle")
  {
    for(int64 i = 0; i < Samples; i++)
    {
      //Perform clipping.
      float64 Value = *ptr_OutChunk * NormalizationScale;
      if(Value < MinFloatValue) {Value = MinFloatValue; Clipped = true;}
      if(Value > MaxFloatValue) {Value = MaxFloatValue; Clipped = true;}
      
      //Dither the double to an int.
      *ptr_OutChunkInt = ClipInt64(TriangularDitherDoubleToInt(
        Value * Scale, DitherBits) * Shift);
      
      //Step pointers.
      ptr_OutChunk++;
      ptr_OutChunkInt++;
    }
  }
  else
  {
    for(int64 i = 0; i < Samples; i++)
    {
      //Perform clipping.
      float64 Value = *ptr_OutChunk * NormalizationScale;
      if(Value < MinFloatValue) {Value = MinFloatValue; Clipped = true;}
      if(Value > MaxFloatValue) {Value = MaxFloatValue; Clipped = true;}
      
      //Round the double to an int.
      *ptr_OutChunkInt = ClipInt64(llrint(Value * Scale) * Shift);
      
      //Step pointers.
      ptr_OutChunk++;
      ptr_OutChunkInt++;
    }
  }
}

int64 FileIO::RectangularDitherDoubleToInt(
  float64 SampleValue, float64 DitherBits)
{
  //For example: 0.6 + [-0.5, 0.5) = [0.1, 1.1)
  float64 Pick1 = RandomNumberGenerator.NextRawFloat64(); //In range [1, 2)
  float64 RectangularPick = Pick1 - 1.5; //In range [-0.5, 0.5)
  RectangularPick *= DitherBits;
  float64 DitheredSampleValue = SampleValue + RectangularPick;
  return llrint(DitheredSampleValue);
}

int64 FileIO::TriangularDitherDoubleToInt(
  float64 SampleValue, float64 DitherBits)
{
  float64 Pick1 = RandomNumberGenerator.NextRawFloat64(); //In range [1, 2)
  float64 Pick2 = RandomNumberGenerator.NextRawFloat64(); //In range [1, 2)
  
  float64 TriangularPick = Pick1 + Pick2 - 3.; //In range [-1, 1)
  TriangularPick *= DitherBits;
  float64 DitheredSampleValue = SampleValue + TriangularPick;
  return llrint(DitheredSampleValue);
}

void FileIO::InitializeChunks(int64 Frames, int64 Channels, bool IsInt, 
  int64 IntBits)
{
  FramesPerChunk = Frames;
  ChannelsPerChunk = Channels;
  FileIO::IntBits = IntBits;
  OutputIsIntType = IsInt;
  
  CleanupChunks();
  
  OutChunk = new float64[Frames * Channels];
  if(IsInt)
    OutChunkInt = new int32[Frames * Channels];
  else
    OutChunkInt = 0;
}

FileIO::FileIO() : OutChunk(0), OutChunkInt(0), Clipped(false),
  UseDither(true)
{
  /*We want dithering to always return the same result on consecutive runs. In
  dithering we are only concerned with the stochastic distribution of the
  random number sequence, not whether it is unique.*/
  RandomNumberGenerator.PickSequence(10271985);
}

void FileIO::CleanupChunks(void)
{
  delete [] OutChunk;
  delete [] OutChunkInt;
  
  OutChunk = 0;
  OutChunkInt = 0;
}

FileIO::~FileIO()
{
  CleanupChunks();
}
