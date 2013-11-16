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

#include "Render.h"

#include "Kaiser.h"
#include "Parameters.h"

#include "Work.h"

#include <iostream>
#include <sstream>
#include <string>

void Renderer::Initialize(Parameters* p)
{  
  Renderer::p = p;
  
  if(!p->ConvolveFilename)
  {
    //Create a Kaiser window object and determine the filter order.
    KaiserLPF = new Kaiser;
    KaiserLPF->Initialize(p->P, p->Q, p->AllowableBandwidthLoss,
      p->StopbandAttenuation);
  }
  else
  {
    KaiserLPF = 0;
  }
  
  //Initialize the FFT object.
  FFTer.Initialize(p->FFTSize, FFTW_PATIENT, 0, true);
}

void Renderer::Go(SNDFILE* s_in, SNDFILE* s_scratch)
{
  Console c;
  
  //Allocate arrays.
  FilterFFT = new float64[FFTer.N_Freq() * 2];
  
  int64 NChunkFramesMax = p->L / p->P + 1;
  int64 NChunkSamplesMax = NChunkFramesMax * p->Channels;
  float64* NChunk = new float64[NChunkSamplesMax];
  
  float64* PChunk = new float64[p->L];
  
  int64 PQChunkFramesMax = p->L / p->Q + 1;
  int64 PQChunkSamplesMax = PQChunkFramesMax * p->Channels;
  float64* PQChunk = new float64[PQChunkSamplesMax];
  
  float64* OverlapChunk = new float64[p->M_1 * p->Channels];
  
  //For plotting the filter.
  bool DoFilterPlot = p->ExportFilterFilename;
  AudioFFT PlotFFTer;
  float64* PlotFFTData = 0;
  int64 PlotFFTSize = 0;
  if(DoFilterPlot)
  {
    PlotFFTSize = (int64)pow(2.0,
      ceil(log((float64)(p->M * p->S)) / log(2.)));
    PlotFFTData = new float64[PlotFFTSize];
  }
  
  //Go through the Kaiser LPF in chunks (can be 1 chunk).
  for(int64 Pass = 0; Pass < p->S; Pass++)
  {
    c += "Pass: "; c &= Pass + 1; c &= "/"; c &= p->S;
    c++;
    GlobalWorkInfo::setPassNumber(Pass + 1);
    GlobalWorkInfo::setTotalPasses(p->S);
    GlobalWorkInfo::setPercentComplete(0);
    
    //Get rid of the bogus stuff leftover in the overlap chunk.
    Memory::ClearArray(OverlapChunk, p->M_1 * p->Channels);
    
    //Retrieve the filter.
    if(KaiserLPF)
    {
      //Create the Kaiser chunk for this pass.
      int64 KaiserSectionWidth = p->M;
      int64 KaiserSectionStart = Pass * KaiserSectionWidth;
      KaiserLPF->CreateLPFInPlace(FFTer.GetTimeDomain(), KaiserSectionStart,
        KaiserSectionWidth);
      Memory::ClearArray(&FFTer.GetTimeDomain()[KaiserSectionWidth],
        p->FFTSize - KaiserSectionWidth);
    }
    else
    {
      /*We are convolving with a file instead of using an LPF because the
      --convolve setting was used. Note only mono IR is supported right now.
      Need to figure out what channel combinations are possible (i.e. what do
      you do if stereo impulse response is convolved on quad file, and is this
      even allowed. Certainly if the number of channels matches then it should
      be legal. Also, we need to create an array of FilterFFTs that the channel
      loop will iterate over, but only in the case of convolution.*/
      int64 ConvolveSectionWidth = p->M;
      int64 ConvolveSectionStart = Pass * ConvolveSectionWidth;
      sf_seek(p->ConvolveHandle, ConvolveSectionStart, SEEK_SET);
      sf_readf_double(p->ConvolveHandle, FFTer.GetTimeDomain(),
        (sf_count_t)ConvolveSectionWidth);
      Memory::ClearArray(&FFTer.GetTimeDomain()[ConvolveSectionWidth],
        p->FFTSize - ConvolveSectionWidth);
        
      //Close file on last pass since we are done with it.
      if(Pass == p->S - 1)
        sf_close(p->ConvolveHandle);
    }
    
    //Copy in the data for the plot.
    if(DoFilterPlot)
      Memory::CopyArray(&PlotFFTData[Pass * p->M],
        FFTer.GetTimeDomain(), p->M);
    
    //Do the FFT of the filter so that it can be saved for later.
    FFTer.TimeToFreq();
    int64 FFTer_N_Freq_2 = FFTer.N_Freq() * 2;
    for(int64 i = 0, j = 0; i < FFTer_N_Freq_2; i += 2, j++)
    {
      FilterFFT[i] = FFTer.FreqReal(j);
      FilterFFT[i + 1] = FFTer.FreqImag(j);
    }
    
    /*Solve the pass delay problem (find a set of input and output shifts, that
    allow the data to be read and written without the use of fractional 
    indexes.*/
    int64 S = Pass * p->M;
    int64 P = p->P;
    int64 Q = p->Q;
    int64 M = p->M;
    int64 SOnQ, Offset, InputShift = -1, OutputShift;
    Console c;
    c += "Pass: "; c &= Pass;
    c += "M: "; c &= M;
    c += "S: "; c &= S;
    c += "P: "; c &= P;
    c += "Q: "; c &= Q;
    
    //Round S up to nearest Q.
    if(S % Q == 0)
    {
      c += "1A";
      SOnQ = S;
    }
    else
    {
      c += "1B";
      SOnQ = S + Q - (S % Q);
    }
    c += "SOnQ: "; c &= SOnQ;

    //Find offset of filter, when its zero offset is removed.
    if(S != 0)
    {
      c += "2A";
      Offset = SOnQ % S;
    }
    else
    {
      c += "2B";
      Offset = 0;
      InputShift = 0;
    }
    c += "InputShift: "; c &= InputShift;
    c += "Offset: "; c &= Offset;
    
    /*We know: (Offset + n * P) % Q == 0
    Solve for n, begin with n = 1 and increment. There will be a solution within
    S searches.*/
    if(Offset == 0)
      InputShift = 0;
    for(int64 n = 0; n < Q && Offset != 0; n++)
    {
      c += "n: "; c &= n;
      if((Offset + n * P) % Q == 0)
      {
        c += "3A";
        InputShift = n;
        break;
      };
    }
    c += "InputShift: "; c &= InputShift;
    
    //Make sure that we actually found an input shift.
    if(InputShift < 0)
    {
      //Stop -- this should not happen!
      c += "There was a problem determining the input delay.";
      return;
    }

    //Calculate input and output shifts.
    OutputShift = SOnQ - (Offset + P * InputShift);
    c += "OutputShift: "; c &= OutputShift;
    
    if(OutputShift % Q != 0)
    {
      //Stop -- this should not happen!
      c += "There was a problem determining the output delay.";
      return;
    }
    OutputShift /= Q;
    c += "OutputShift: "; c &= OutputShift;
    
    //The pass delay problem is solved!
        
    //Initialize disk read and write heads.
    sf_seek(s_scratch, OutputShift, SEEK_SET | SFM_READ);
    sf_seek(s_scratch, OutputShift, SEEK_SET | SFM_WRITE);
    sf_seek(s_in, 0, SEEK_SET);
    
    //Loop through horizontal blocks of size L in the P-space input.
    int64 PSpaceInitial = 0;
    int64 PSpaceSamples = p->L;
    int64 NSpaceStart;
    int64 PSpaceStart = PSpaceInitial;
    do
    {
      //Make calculations for translating from N space to P space.
      int64 PSpaceEnd = PSpaceStart + PSpaceSamples - 1;
      if(PSpaceStart % p->P == 0)
        NSpaceStart = PSpaceStart / p->P;
      else
        NSpaceStart = (PSpaceStart - (PSpaceStart % p->P) + p->P) / p->P;
      int64 NSpaceEnd = (PSpaceEnd - (PSpaceEnd % p->P)) / p->P;
      int64 NSpaceSamples = NSpaceEnd - NSpaceStart + 1;
        
      //Make calculations for translating from P space to PQ space.
      int64 PQSpaceStart;
      if(PSpaceStart % p->Q == 0)
        PQSpaceStart = PSpaceStart / p->Q;
      else
        PQSpaceStart = (PSpaceStart - (PSpaceStart % p->Q) + p->Q) / p->Q;        
      int64 PQSpaceEnd = (PSpaceEnd - (PSpaceEnd % p->Q)) / p->Q;
      int64 PQSpaceSamples = PQSpaceEnd - PQSpaceStart + 1;
      
      //Make calculations for adding the pass delay.
      int64 PSpacePassStart = PSpaceStart;
      int64 PSpacePassEnd = PSpaceEnd;
      int64 PQSpacePassStart = PQSpaceStart;
      int64 PQSpacePassEnd = PQSpaceEnd;
      int64 PQSpacePassSamples = PQSpaceSamples;
      
      //Read in a block taking into account the pass delay input shift zero pad.
      int64 FramesRead;
      if(InputShift == 0)
      {
        //Read in a block from the N-space input file.
        FramesRead = (int64)sf_readf_double(
          s_in, NChunk, (sf_count_t)NSpaceSamples);
      }
      else if(InputShift < NSpaceSamples)
      {
        //Zero pad the beginning of the chunk array to reflect input shift.
        Memory::ClearArray(NChunk, InputShift * p->Channels);
        
        //Read in a block from the N-space input file.
        FramesRead = (int64)sf_readf_double(
          s_in, &NChunk[InputShift * p->Channels],
          (sf_count_t)(NSpaceSamples - InputShift));
          
        //Report the zero padded frames as actual frames.
        FramesRead += InputShift;
          
        //Input shift is no longer necessary.
        InputShift = 0;
      }
      else
      {
        /*For some reason the input shift was really large, therefore we will
        not even read from the file at all yet.*/
        Memory::ClearArray(NChunk, NSpaceSamples * p->Channels);
        
        //Report the zero padded frames as actual frames.
        FramesRead = InputShift;
        
        //Decrement the initial zero pad amount as it is used up.
        InputShift -= NSpaceSamples;
      }
      
      //Calculate the number of samples read.
      int64 SamplesRead = FramesRead * p->Channels;
      
      //Zero out any portion that was not read.
      Memory::ClearArray(&NChunk[SamplesRead],
        NSpaceSamples * p->Channels - SamplesRead);
      
      /*Zero out p-chunk array. (Note we only need to do this once per
      multi-channel chunk since the placement of zeroes is the same in all
      channels.*/
      Memory::ClearArray(PChunk, p->L);
      
      //Read in a block from scratch disk.
      int64 PQFramesRead = (int64)sf_readf_double(
        s_scratch, PQChunk, (sf_count_t)PQSpaceSamples);
      int64 PQSamplesRead = PQFramesRead * p->Channels;
      
      //Zero out any portion that was not read.
      Memory::ClearArray(&PQChunk[PQSamplesRead],
        PQSpaceSamples * p->Channels - PQSamplesRead);
      
      //Roll back write cursor and read cursor to same point.
      int64 OriginalPosition = 
        sf_seek(s_scratch, -PQFramesRead, SEEK_CUR | SFM_READ);        
      sf_seek(s_scratch, OriginalPosition, SEEK_SET | SFM_WRITE);
      
      //Now work on each channel in the chunk.
      for(int64 Channel = 0; Channel < p->Channels; Channel++)
      {
        /*Initialize the PChunk with all the values from NChunk. In p-space we
        are interleaving P zeroes in between each actual sample point. We
        created the zeroes above by clearing the array.*/
        int64 PIndexStart = NSpaceStart * p->P - PSpaceStart;
        int64 PIndexEnd = NSpaceEnd * p->P - PSpaceStart;
        int64 P_hop = p->P;
        float64* ptr_ChannelNChunk = &NChunk[Channel];
        int64 ChannelHop = p->Channels;
        
        for(int64 PIndex = PIndexStart; PIndex <= PIndexEnd; PIndex += P_hop)
        {
          PChunk[PIndex] = *ptr_ChannelNChunk;
          ptr_ChannelNChunk += ChannelHop;
        }
        
        //Copy P chunk into FFTer, and zero out everything else starting at L.
        float64* fft_time = FFTer.GetTimeDomain();
        Memory::CopyArray(fft_time, PChunk, p->L);
        Memory::ClearArray(&fft_time[p->L], FFTer.N_Freq() * 2 - p->L);
        
        //Convert the P chunk into the frequency domain.
        FFTer.TimeToFreq();
        
        //Transform is in place so freq domain is same memory as time domain.
        float64* fft_freq = fft_time;
        int64 P_Freq = FFTer.N_Freq() * 2;
        
        //Apply filter in frequency domain through complex multiplication.
        for(int64 FreqSample = 0; FreqSample < P_Freq; FreqSample += 2)
        {
          int64 FreqSample_imag = FreqSample + 1;
          float64* fft_cur_freq_real = &fft_freq[FreqSample];
          float64* fft_cur_freq_imag = &fft_freq[FreqSample_imag];
          
          float64 r1 = *fft_cur_freq_real;
          float64 r2 = FilterFFT[FreqSample];
          
          float64 i1 = *fft_cur_freq_imag;
          float64 i2 = FilterFFT[FreqSample_imag];
          
          *fft_cur_freq_real = r1 * r2 - i1 * i2;
          *fft_cur_freq_imag = r1 * i2 + r2 * i1;
        }
        
        //Transform back to the time domain.
        FFTer.FreqToTime();
        
        //Get current channel of overlap data.
        float64* ChannelOverlapChunk = &OverlapChunk[Channel];
        float64* ptr_ChannelOverlapChunk = ChannelOverlapChunk;
        
        /*Compute the necessary overlap, straight onto the FFT time domain data.
        At the same time fill in the new overlap data.*/
        int64 OverlapEnd = p->M_1;
        float64* ptr_fft_time_overlap = &fft_time[p->L];
        for(int64 OverlapIndex = 0; OverlapIndex < OverlapEnd; OverlapIndex++)
        {
          float64 NewOverlapValue = *ptr_fft_time_overlap;
          fft_time[OverlapIndex] += *ptr_ChannelOverlapChunk;
          *ptr_ChannelOverlapChunk = NewOverlapValue;
          ptr_ChannelOverlapChunk += ChannelHop;
          ptr_fft_time_overlap++;
        }
        
        //Mix Q-decimated FFT time domain into current PQ chunk.
        float64* ptr_PQChunkChannel = &PQChunk[Channel];
        int64 Q_hop = p->Q;
        int64 PQIndexStart = PQSpacePassStart * Q_hop - PSpacePassStart;
        int64 PQIndexEnd = PQSpacePassEnd * Q_hop - PSpacePassStart;
        float64 NormalizeFactor = p->FFTSize * p->P;
        for(int64 PQIndex = PQIndexStart;
          PQIndex <= PQIndexEnd; PQIndex += Q_hop)
        {
          *ptr_PQChunkChannel += fft_time[PQIndex] * NormalizeFactor;            
          ptr_PQChunkChannel += ChannelHop;
        }
      }

      //Determine how much data to write.
      int64 CurrentPosition = sf_seek(s_scratch, 0, SEEK_CUR | SFM_WRITE);
      int64 FramesUntilEnd = p->OutPQFrames - CurrentPosition;
      if(FramesUntilEnd < PQSpacePassSamples)
        PQSpacePassSamples = FramesUntilEnd;

      //Write PQ chunk block back to scratch disk.
      sf_writef_double(s_scratch, PQChunk, (sf_count_t)PQSpacePassSamples);
      if(FramesUntilEnd <= PQSpacePassSamples)
        break;

      //Push the read cursor up to where the write cursor is.
      int64 NextPosition = sf_seek(s_scratch, 0, SEEK_CUR | SFM_WRITE);
      sf_seek(s_scratch, NextPosition, SEEK_SET | SFM_READ);
      
      //Increment the P-space index for the next loop iteration.
      PSpaceStart += p->L;
      
      //Get the current read place.
      int64 CurrentReadPlace = sf_seek(s_in, 0, SEEK_CUR);
      float64 pc = (float64)CurrentReadPlace/(float64)p->Frames*100.;
      c &= (number)pc;
      c &= "%...";
      GlobalWorkInfo::setPercentComplete(pc);
      std::cout.flush();
    } while(true);
  }
  
  //Cleanup arrays.
  delete [] OverlapChunk;
  delete [] PQChunk;
  delete [] PChunk;
  delete [] NChunk;
  delete [] FilterFFT;
  FFTer.Initialize(16, FFTW_PATIENT, 0, true);
  
  /*Dump the plot contents to file and write a Mathematica script that can
  generate some nice plots for us.*/
  if(DoFilterPlot)
  {
    c += "Writing filter plot to '"; c &= p->ExportFilterFilename; c &= "'...";
    PlotFFTer.Initialize(PlotFFTSize, FFTW_PATIENT, 0, true);
    Memory::CopyArray(PlotFFTer.GetTimeDomain(), PlotFFTData, PlotFFTSize);
    delete [] PlotFFTData;
    PlotFFTer.TimeToFreqUnnormalized();
    count BinsToExport = PlotFFTer.N_Freq();
    File::Write(p->ExportFilterFilename, (byte*)PlotFFTer.GetFreqDomain(),
      sizeof(float64) * 2 * BinsToExport);
    
    String ExportFilterStem = p->ExportFilterFilename;
    ExportFilterStem.Replace(".fft", "");
    String ActiveRegion = ExportFilterStem; ActiveRegion &= "_Active.png";
    String TransRegion = ExportFilterStem; TransRegion &= "_Transition.png";
    String PassRipple = ExportFilterStem; PassRipple &= "_PassbandRipple.png";
    String PhaseDeviation = ExportFilterStem;
      PhaseDeviation &= "_PhaseDeviation.png";
    
    c++;
    c++;
    c += "(*---------------------Mathematica Test Code---------------------*)";
    c++;
    c += "(*Import the data from file.*)";
    c += "FFTData=Import[\""; c &= p->ExportFilterFilename;
      c &= "\", \"Complex128\"];";
    c++;
    
    c += "(*Calculate some useful constants.*)";
    c += "FFTLength=(Length[FFTData]-1)*2;";
    c += "P="; c &= (integer)p->P; c &= ";";
    c += "Q="; c &= (integer)p->Q; c &= ";";
    c += "MinPQ=Min[P,Q];";
    c += "OldSRate = "; c &= p->OldSampleRate; c &= ";";
    c += "NewSRate = "; c &= p->NewSampleRate; c &= ";";
    c += "PlotPointsLimit = 1000;";
    c += "HighNyq = Max[OldSRate, NewSRate]/2;";
    c += "LowNyq = Min[OldSRate, NewSRate]/2;";
    c += "FreqToBin[f_] := Round[(f/HighNyq)*(Length[FFTData] - 1)/MinPQ];";
    c += "LimitSize[s_, t_] := (s1 = Ceiling[Length[t]/s]; "
      "Take[t, {1, -1, s1}])";
    c++;
    
    //The following plot takes insane memory (> 4GB) for high P or Q.
    //c += "ListLinePlot[";
    //c += " 20*Log10[Abs[FFTData]], {DataRange -> {0, HighNyq*MinPQ}, ";
    //c += "  PlotLabel -> \"Filter Spectrum\", AxesLabel -> {f, \"dB\"}}]";
    
    c += "(*Active Region Plot*)";
    c += "ar=LimitSize[PlotPointsLimit,";
    c += "  Chop[20*Log10[Abs[FFTData[[1 ;; FreqToBin[HighNyq]]]]]]];";
    c += "Export[\""; c &= ActiveRegion; c &= "\", ListLinePlot[ar,";
    c += "{DataRange -> {0, HighNyq}, PlotRange -> {0, -320}, "
      "PlotLabel -> \"Active Region: DC to Upper Nyquist\", "
      "AxesLabel -> {\"\\[ScriptF] (Hz)\",\"dB\"}, "
      "LabelStyle -> (FontFamily -> \"Georgia\"), ImageSize -> Large}]]";
    c++;
    
    c += "(*Transition Region Plot*)";
    c += "tr=LimitSize[PlotPointsLimit,Chop[20*Log10[Abs[";
    c += "    FFTData[[FreqToBin[LowNyq - 100] ;; ";
    c += "       FreqToBin[LowNyq + 100]]]]]]];";
    c += "Export[\""; c &= TransRegion; c &= "\", ListLinePlot[tr,";
    c += "{DataRange -> {LowNyq - 100, LowNyq + 100},";
    c += " PlotRange -> {0, -320}, "
      "PlotLabel -> \"Transition: Lower Nyquist Cutoff +/- 100 Hz\", ";
    c += "  AxesLabel -> {\"\\[ScriptF] (Hz)\",\"dB\"}, "
      "LabelStyle -> (FontFamily -> \"Georgia\"), ImageSize -> Large}]]";
    c++;
    
    c += "(*Passband Ripple Plot*)";
    c += "pr=LimitSize[PlotPointsLimit,20*Log10[Abs[Abs[";
    c += "    FFTData[[1 ;; FreqToBin[LowNyq - 50]]]]-1]]];";
    c += "Export[\""; c &= PassRipple; c &= "\", ListLinePlot[";
    c += "pr, {DataRange -> {0, ";
    c += "    LowNyq - 50}, PlotLabel -> \"Passband Ripple from Unity\", "
      "PlotRange -> {0,-320}, AxesLabel -> {\"\\[ScriptF] (Hz)\",\"dB\"}, "
      "LabelStyle -> (FontFamily -> \"Georgia\"), ImageSize -> Large}]]";
    c++;
    
    c += "(*Unwrap function for phase. Works similarly to MATLAB's unwrap.*)";
    c += "Unwrap[args_] := ";
    c += "  Module[{pairs, diffs, j, len = Length[args], corr = 0}, ";
    c += "  pairs = Partition[args, 2, 1];";
    c += "  diffs = Map[#[[1]] - #[[2]] &, pairs];";
    c += "  PrependTo[diffs, 0];";
    c += "  diffs = 2*Pi*Sign[Chop[diffs, Pi]];";
    c += "  Table[corr += diffs[[j]];";
    c += "  corr + args[[j]], {j, 1, len}]]";
    c++;
    
    c += "(*Fits a line to beginning of unwrapped phase, then we calculate the";
    c += "  phase of the group delay using that line. The plot will subtract ";
    c += "  the group delay from the actual phase so that we can see even the ";
    c += "  slightest deviation from the group delay in the passband.*)";
    c += "FittedPhase = Fit[Unwrap[Arg[FFTData[[1 ;; 10]]]], {1, x}, x];";
    c += "GroupDelay = Table[FittedPhase, {x, 1, FreqToBin[LowNyq]-1}];";
    c++;
    
    c += "(*Phase Deviation Plot*)";
    c += "pd=LimitSize[PlotPointsLimit, ";
    c += "  Mod[Unwrap[Arg[FFTData[[1 ;; FreqToBin[LowNyq]-1]]]] - "
      "GroupDelay + 2 \\[Pi], 4 \\[Pi]] - 2 \\[Pi]];";
    c += "Export[\""; c &= PhaseDeviation; c &= "\", ListLinePlot[pd,";
    c += "{PlotLabel -> \"Phase Deviation from Group\","; 
    c += "  AxesLabel -> {\"\\[ScriptF] (Hz)\",\"\\[Theta] (radians)\"}, "
      "DataRange -> {0, LowNyq}, ";
    c += "  PlotRange -> {-10^-6, 10^-6}, PlotStyle -> Thick, "
      "LabelStyle -> (FontFamily -> \"Georgia\"), ImageSize -> Large}]]";
    c++;
    c += "(*-------------------End Mathematica Test Code-------------------*)";
    c++;
    c++;
  }
}
