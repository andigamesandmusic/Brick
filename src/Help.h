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

void DoHelp(void)
{
  Console c;
  c += "Usage: brick inputfile.aiff outputfile.wav [settings]";
  c += "       brick inputfile.aiff outputspectrogram.png [settings]";
  c += "";
  c += "  Brick can read and write multi-channel .aiff (or .aif), .wav, .au, .raw";
  c += "  ";
  c += "  Brick can probably read: .caf, .flac, .htk, .iff, .mat4, .mat5, .paf, .pvf,";
  c += "                           .sd2, .sf, .svx, .voc, .w64, .xi";
  c += "                       ";
  c += "  Brick can write spectrograms (instead of audio) to: .png, .jpg";
  c += "                       ";
  c += "  Brick is a non-destructive resampler. It can not be used to resample a file";
  c += "  in place.";
  c += "  ";
  c += "  The [settings] can be combinations of the following:";
  c += "  ";
  c += "                                   *****";
  c += "";
  c += "  OUTPUT FORMAT";
  c += "  --samplerate=keep [keep 44100Hz 48000Hz nnnnnnHz] (integers only)";
  c += "  The sample rate of the output in whole Hertz. You must specify the units 'Hz'.";
  c += "  If there is no change to the sample rate, you can explictly write this with";
  c += "  'keep'.";
  c += "  ";
  c += "  --sampleformat=keep [keep int16 int24 int32 float32 float64]";
  c += "  The bit format representing each sample. Using 'keep' will match the output";
  c += "  format to the input format.";
  c += "  ";
  c += "                                   *****";
  c += "";
  c += "  RAW DATA";
  c += "  If the input file extension is .raw, then you must provide the following";
  c += "  information, with the exception that if the output is a spectrogram, then";
  c += "  --inputsamplerate is not applicable and does not need to be specified.";
  c += "  ";
  c += "  In order to read a file as raw data, the extension must be .raw. Note that it";
  c += "  is assumed that samples are grouped into frames not channels; that is,";
  c += "  channels are not presented separately, but their samples are interleaved";
  c += "  (i.e., chan0samp0, chan1samp0, chan0samp1, chan1samp1, etc.)";
  c += "  ";
  c += "  --inputchannels=[1 2 3 4 nn] (integers only)";
  c += "  The number of channels of raw data input. Channel samples are interleaved.";
  c += "  ";
  c += "  --inputsampleformat=[int16 int24 int32 float32 float64]";
  c += "  The format of the raw data input. Integer data is interpreted as signed. The";
  c += "  endianness is determined by the system.";
  c += "  ";
  c += "  --inputsamplerate=[44100Hz 48000Hz nnnnnnHz] (integers only)";
  c += "  The sample rate of the raw data input.";
  c += "  ";
  c += "                                   *****";
  c += "";
  c += "  RESAMPLE QUALITY";
  c += "  --allowablebandwidthloss=0.1%";
  c += "  The maximum percentage of bandwidth loss near the Nyquist frequency of the";
  c += "  output sample rate. You must specify this number as a percentage. This";
  c += "  reflects the width of the low-pass filter transition region between the pass";
  c += "  and stop bands. It does not account for frequency content in the input above";
  c += "  the output Nyquist frequency that is inherently rejected due to the resampling";
  c += "  process. The default value is 0.1%.";
  c += "  ";
  c += "  --depth=200dB [nnn.ndB]";
  c += "  The depth of attenuation in decibels in the resampling filter's stopband. The";
  c += "  maximum value of depth is 300dB (a conservative limit to the dynamic range of";
  c += "  the 64-bit floating point operations that are computing the filter.) Note the";
  c += "  dynamic range of the integer bit-rates are: 16-bit=96.3dB, 24-bit=144.5dB, and";
  c += "  32-bit=192.7dB.";
  c += "  ";
  c += "  --nofilter";
  c += "  Disables the resampling filter thereby doing a simple rate change with no";
  c += "  sample-wise conversion. The output file will simply have a different sample";
  c += "  rate. This option is also used internally when the overall sample rate ratio";
  c += "  stays the same (i.e., 44.1kHz to 88.2kHz with a +1 pitch-shift).";
  c += "  ";
  c += "                                   *****";
  c += "";
  c += "  PITCH SHIFT";
  c += "  --pitchshift=[3/2 +1m3.35 |+-|n|dmPMA|n.nn|]";
  c += "  A ratio or octave and interval by which to pitch shift (by resampling) the";
  c += "  audio file. Valid intervals are P1, m2, M2, m3, M3, P4, A4, D5, TT, P5, m6,";
  c += "  M6, m7, M7. For example up an octave: +1. Down a minor third: -m3.";
  c += "  ";
  c += "  In the case that an interval is specified, the irrational ratio is";
  c += "  rationalized to within an error of +/- the value of --centstolerance. The";
  c += "  ratio of the pitch shift is cascaded with the ratio of the sample ratio";
  c += "  conversion.";
  c += "";
  c += "  --centstolerance=0.1 (0.0 to 100.0)";
  c += "  The minimum number of cents (+/-) that an interval-based (irrational) pitch";
  c += "  shift will be accurate to.";
  c += "  ";
  c += "                                   *****";
  c += "";
  c += "  DITHERING";
  c += "  --dither=triangle [triangle, rectangle, off]";
  c += "  Sets the type of dithering to be used when downconverting integer formats, or";
  c += "  when one of the resample, low-pass, or high-pass filters are applied.";
  c += "  Dithering is never used unless necessary (i.e., unless the signal has been";
  c += "  modified by a filter)";
  c += "  ";
  c += "  --ditherbits=1.0 (0.0 - 1.0)";
  c += "  Sets the amount of bits (0.0 up to 1.0) to dither. As dither increases the";
  c += "  white noise bed gets louder and the correlated quantization noise gets softer.";
  c += "  Usually, the default value of 1.0 is the best.";
  c += "  ";
  c += "                                   *****";
  c += "";
  c += "  SPECTROGRAM (mono and stereo only)";
  c += "  A spectrogram is made when the output file is of type '.png' (PNG) or";
  c += "  '.jpg' (JPEG). For JPEG the highest quality compression will be used. The";
  c += "  following parameters control the spectrogram:";
  c += "  ";
  c += "  --spectrogramsize=4096 (128 to 65536, integers only)";
  c += "  The window size of the FFT. This does not have to be a power-of-two, but will";
  c += "  be faster if it is. The height of the image is this value divided by 2 plus 1.";
  c += "  Increasing this value will increase the resolution of the spectrogram, but";
  c += "  does not generally affect the dynamic range. See spectrogram beta for changing";
  c += "  the dynamic range of the analysis.";
  c += "  ";
  c += "  --spectrogramstep=1000 (1 to spectrogram size, integers only)";
  c += "  The sample step size corresponding to the time-domain width of a single column";
  c += "  in the image. The width of the image will be approximately the total number of";
  c += "  sample frames divided by the step size.";
  c += "  ";
  c += "  --spectrogrambeta=35.0 (5.0 to 40.0)";
  c += "  The beta value of the Kaiser window. Higher beta values increase the sidelobe";
  c += "  attenuation (dynamic range), but decrease the main-lobe width (resolution) if";
  c += "  the spectrogram size is held constant.";
  c += "  ";
  c += "  --gradient=gray [gray/grey color/colour]";
  c += "  The type of color gradient used for the spectrogram. The gray gradient has the";
  c += "  property that the value of the 8-bit pixel is the dB value at the given";
  c += "  frequency. Thus black is 0dB and white is -255 dB.";
  c += "  ";
  c += "  If the input is stereo, then the gradient will automatically be red=left";
  c += "  channel, blue=right channel, green=difference between channels. Thus you will";
  c += "  see yellow as purely left, purple as similar frequency energy in both the";
  c += "  channels, and green-blue as purely right.";
  c += "  ";
  c += "  --gradientrange=default [default, 6dB to 300dB]";
  c += "  The dynamic range of the gradient. If default is specified then the dynamic";
  c += "  range will be 255dB for gray, 180dB for color, and ~128dB for stereo.";
  c += "  ";
  c += "  ";
  c += "                                   *****";
  c += "";
  c += "  CONVOLUTION (mono only)";
  c += "  --convolve=[impulseresponse.aiff/au/wav]";
  c += "  Performs convolution using the given impulse-response. Since the convolution";
  c += "  is calculated using the same algorithm used for sample-rate conversion, it";
  c += "  will be very efficient.";
  c += "  ";
  c += "  ";
  c += "                                   *****";
  c += "";
  c += "  WISDOM";
  c += "  Wisdom is an accumulation of machine-dependent optimizations that take place";
  c += "  during the plan-phase of the FFTs used in Brick (via FFTW). Wisdom is stored";
  c += "  in an application support text file and takes up very little space. The";
  c += "  purpose of wisdom is hint the application as to which FFT algorithms might";
  c += "  run the fastest on that particular machine. In order to make use of wisdom,";
  c += "  you should run Brick with the parameter --acquirewisdom (by itself) to";
  c += "  generate wisdom for your machine. (See below).";
  c += "  ";
  c += "  --acquirewisdom";
  c += "  Precalculate wisdom for all possible FFTs that can fit into physical memory.";
  c += "  This parameter may not be used with any other parameters. There are three";
  c += "  stages with time-limits on how long to spend optimizing each FFT size (up to";
  c += "  about 26 possible): ten-seconds, one-minute, two-minutes.";
  c += "  ";
  c += "  This may take several minutes, but it is a one-time cost hat can subsequently";
  c += "  increase the speed at which Brick does conversions, around 10%-20% depending";
  c += "  on the FFT.";
  c += "  ";
  c += "  If you stop acquiring wisdom (Control + C to exit), then you will have";
  c += "  accumulated all the wisdom up to that point. If you then begin acquiring";
  c += "  wisdom again, the system will pick up where you left off.";
  c += "  ";
  c += "  --forgetwisdom";
  c += "  Deletes the cache of wisdom on your system. You should do this when you";
  c += "  upgrade to a new version of Brick and re-run --acquirewisdom.";
  c += "  ";
  c += "  --donotloadwisdom";
  c += "  Forces the program to not load wisdom during this run.";
  c += "  ";
  c += "  ";
  c += "                                   *****";
  c += "";
  c += "  FILTER ANALYSIS (requires Mathematica for graphs)";
  c += "  --exportfilter=filtername.fft";
  c += "  Exports a high-resolution FFT of the filter being applied during the";
  c += "  block-convolution algorithm of a sample-rate conversion. The console will";
  c += "  then display code that can be copy-pasted into a Mathematica document to";
  c += "  create graphs showing the active region, transition region, passband ripple,";
  c += "  and group (phase) deviation.";
  c += "  ";
  c += "    ";
  c += "                                   *****";
  c += "";
  c += "  HELP";
  c += "  --help";
  c += "  Lists and describes the correct parameter usage to Brick. If Brick is run with";
  c += "  no parameters, then the help will be shown.";
}