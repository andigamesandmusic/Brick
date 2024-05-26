/*
 ==============================================================================
 
 This file is part of Brick
 Copyright 2010 Andi
 
 ------------------------------------------------------------------------------
 
 Brick can be redistributed and/or modified under the terms of the
 GNU General Public License, as published by the Free Software Foundation;
 either version 3 of the License, or (at your option) any later version.
 
 Brick is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 for more details.
 
 You should have received a copy of the GNU General Public License
 along with Brick; if not, visit www.gnu.org/licenses or write to
 the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 Boston, MA 02111-1307 USA
 
 ==============================================================================
*/

#include "BrickGUI.h"
#include "FileIO.h"
#include "Work.h"

//Helper for JUCE string constants
#define T(x) juce::String(x)

void BrickGUI::AddComponentPointers(void)
{
  List<void*>& c = ComponentPointers;
  
  //Buttons
  c.Add() = &B_Go;
  c.Add() = &B_OctaveDown;
  c.Add() = &B_OctaveUp;

  //Combo boxes
  c.Add() = &C_Bandwidth;
  c.Add() = &C_Depth;
  c.Add() = &C_DitherBits;
  c.Add() = &C_DitherShape;
  c.Add() = &C_OutputAudioFormat;
  c.Add() = &C_OutputSampleFormat;
  c.Add() = &C_OutputSampleRate;
  
  //Group panes
  c.Add() = &G_InputAudioFile;
  c.Add() = &G_OutputSRC;
  
  //Labels
  c.Add() = &L_Bandwidth;
  c.Add() = &L_Cents;
  c.Add() = &L_Depth;
  c.Add() = &L_DitherShape;
  c.Add() = &L_DragDrop;
  c.Add() = &L_InputFilename;
  c.Add() = &L_Octaves;
  c.Add() = &L_OutputFilename;
  c.Add() = &L_OutputSampleRate;
  c.Add() = &L_Ratio;
  c.Add() = &L_SampleFormat;
  c.Add() = &L_SampleOutFormat;
  c.Add() = &L_SampleRate;
  c.Add() = &L_Semitones;
  c.Add() = &L_Tolerance;
  
  //Sliders
  c.Add() = &S_Cents;
  c.Add() = &S_Octaves;
  c.Add() = &S_Semitones;
  c.Add() = &S_Tolerance;
  
  //Text editors
  c.Add() = &T_OutputFilename;
  c.Add() = &T_RatioDen;
  c.Add() = &T_RatioNum;
}

void BrickGUI::ZeroComponentPointers(void)
{
  for(count i = 0; i < ComponentPointers.n(); i++)
    *((Component**)ComponentPointers[i]) = 0;
}

void BrickGUI::DeleteAndZeroComponentPointers(void)
{
  for(count i = 0; i < ComponentPointers.n(); i++)
    deleteAndZero(*((Component**)ComponentPointers[i]));
}

void BrickGUI::SetAllComponentsEnabled(bool Enabled)
{
  for(count i = 0; i < ComponentPointers.n(); i++)
    (*((Component**)ComponentPointers[i]))->setEnabled(Enabled);
}

BrickGUI::~BrickGUI()
{
  DeleteAndZeroComponentPointers();
}

void BrickGUI::resized()
{
  //The group panes on the left-hand side
  G_InputAudioFile->setBounds (16, 8, 360, 128);
  G_OutputSRC->setBounds (16, 144, 360, 288);

  //Drag and drop instruction label
  L_DragDrop->setBounds (144, 32, 216, 88);

  //Left-hand column
  L_InputFilename->setBounds   (32, 32, 328, 24);
  L_SampleRate->setBounds      (32, 64, 328, 24);
  L_SampleFormat->setBounds    (32, 96, 328, 24);
  //---
  L_OutputSampleRate->setBounds(32, 168, 120, 24);
  L_SampleOutFormat->setBounds (32, 200, 120, 24);
  L_DitherShape->setBounds     (32, 232, 120, 24);
  L_OutputFilename->setBounds  (32, 264, 120, 24);
  //---
  L_Depth->setBounds           (32, 312, 120, 24);
  L_Bandwidth->setBounds       (32, 344, 120, 24);
  //---
  B_Go->setBounds              (32, 384, 328, 32);
  
  //Editable part of the left-hand column
  C_OutputSampleRate->setBounds  (152, 168, 208, 24);
  C_OutputSampleFormat->setBounds(152, 200, 208, 24);
  C_DitherShape->setBounds       (152, 232, 136, 24); //Linked
  C_DitherBits->setBounds        (296, 232, 64, 24);  //
  T_OutputFilename->setBounds    (152, 264, 136, 24); //Linked
  C_OutputAudioFormat->setBounds (296, 264, 64, 24);  //
  //---
  C_Depth->setBounds             (152, 312, 208, 24);
  C_Bandwidth->setBounds         (152, 344, 208, 24);
  
  
  //Ratio portion of pitch shift
  L_Ratio->setBounds (392, 36, 80, 24);
  T_RatioNum->setBounds (464, 24, 80, 20);
  T_RatioDen->setBounds (464, 48, 80, 20);
  
  
  //Pitch shift knobs
  B_OctaveUp->setBounds  (400, 104, 80, 32); //Linked
  B_OctaveDown->setBounds(480, 104, 80, 32); //
  L_Octaves->setBounds   (400, 144, 80, 24);
  S_Octaves->setBounds   (400, 168, 160, 32);
  L_Semitones->setBounds (397, 216, 96, 24);
  S_Semitones->setBounds (400, 240, 160, 32);
  L_Cents->setBounds     (400, 288, 80, 24);
  S_Cents->setBounds     (400, 304, 160, 48);
  L_Tolerance->setBounds (397, 360, 88, 24);
  S_Tolerance->setBounds (400, 376, 160, 48);
}

//------------------------------------Old Code--------------------------------//

BrickGUI::BrickGUI () : Component (T("BrickGUI")), ptrBrickThread(0)
{
  AddComponentPointers();
  ZeroComponentPointers();
  
  using namespace juce;
  addAndMakeVisible (G_InputAudioFile = new GroupComponent (T("new group"),
                                                           T("Input Audio File")));
  G_InputAudioFile->setTextLabelPosition (juce::Justification::centredLeft);
  G_InputAudioFile->setColour (GroupComponent::outlineColourId, Colour (0x66ffffff));
  G_InputAudioFile->setColour (GroupComponent::textColourId, Colours::white);

  addAndMakeVisible (G_OutputSRC = new GroupComponent (T("new group"),
                                                           T("Output Sample Rate Conversion")));
  G_OutputSRC->setTextLabelPosition (juce::Justification::centredLeft);
  G_OutputSRC->setColour (GroupComponent::outlineColourId, Colour (0x66ffffff));
  G_OutputSRC->setColour (GroupComponent::textColourId, Colours::white);

  addAndMakeVisible (S_Octaves = new Slider (T("S_Octaves")));
  S_Octaves->setExplicitFocusOrder (13);
  S_Octaves->setRange (0, 10, 1);
  S_Octaves->setSliderStyle (Slider::IncDecButtons);
  S_Octaves->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 30);
  S_Octaves->addListener (this);

  addAndMakeVisible (L_Octaves = new Label (T("new label"),
                                         T("OCTAVES")));
  L_Octaves->setFont (Font (T("Lucida Grande"), 17.0000f, Font::plain));
  L_Octaves->setJustificationType (juce::Justification::centred);
  L_Octaves->setEditable (false, false, false);
  L_Octaves->setColour (Label::textColourId, Colours::white);
  L_Octaves->setColour (TextEditor::textColourId, Colours::black);
  L_Octaves->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (S_Semitones = new Slider (T("S_Semitones")));
  S_Semitones->setExplicitFocusOrder (14);
  S_Semitones->setRange (0, 11, 1);
  S_Semitones->setSliderStyle (Slider::IncDecButtons);
  S_Semitones->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 30);
  S_Semitones->addListener (this);

  addAndMakeVisible (L_Semitones = new Label (T("new label"),
                                         T("SEMITONES")));
  L_Semitones->setFont (Font (T("Lucida Grande"), 17.0000f, Font::plain));
  L_Semitones->setJustificationType (juce::Justification::centredLeft);
  L_Semitones->setEditable (false, false, false);
  L_Semitones->setColour (Label::backgroundColourId, Colour (0x0));
  L_Semitones->setColour (Label::textColourId, Colours::white);
  L_Semitones->setColour (TextEditor::textColourId, Colours::black);
  L_Semitones->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (S_Cents = new Slider (T("S_Cents")));
  S_Cents->setExplicitFocusOrder (15);
  S_Cents->setRange (0, 99.5, 0.5);
  S_Cents->setSliderStyle (Slider::RotaryVerticalDrag);
  S_Cents->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 30);
  S_Cents->setColour (Slider::rotarySliderFillColourId, Colour (0x7f252560));
  S_Cents->addListener (this);

  addAndMakeVisible (L_Cents = new Label (T("new label"),
                                         T("CENTS")));
  L_Cents->setFont (Font (T("Lucida Grande"), 17.0000f, Font::plain));
  L_Cents->setJustificationType (juce::Justification::centred);
  L_Cents->setEditable (false, false, false);
  L_Cents->setColour (Label::backgroundColourId, Colour (0x0));
  L_Cents->setColour (Label::textColourId, Colours::white);
  L_Cents->setColour (TextEditor::textColourId, Colours::black);
  L_Cents->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (S_Tolerance = new Slider (T("S_Tolerance")));
  S_Tolerance->setExplicitFocusOrder (16);
  S_Tolerance->setRange (2.5, 100, 0.1);
  S_Tolerance->setSliderStyle (Slider::RotaryVerticalDrag);
  S_Tolerance->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 30);
  S_Tolerance->setColour (Slider::rotarySliderFillColourId, Colour (0x7f252560));
  S_Tolerance->addListener (this);

  addAndMakeVisible (L_Tolerance = new Label (T("new label"),
                                         T("TOLERANCE")));
  L_Tolerance->setFont (Font (T("Lucida Grande"), 17.0000f, Font::plain));
  L_Tolerance->setJustificationType (juce::Justification::centredLeft);
  L_Tolerance->setEditable (false, false, false);
  L_Tolerance->setColour (Label::backgroundColourId, Colour (0x0));
  L_Tolerance->setColour (Label::textColourId, Colours::white);
  L_Tolerance->setColour (TextEditor::textColourId, Colours::black);
  L_Tolerance->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (L_InputFilename = new Label (T("LabelFilename"),
                                        T("Filename: ")));
  L_InputFilename->setFont (Font (15.0000f, Font::plain));
  L_InputFilename->setJustificationType (juce::Justification::centredLeft);
  L_InputFilename->setEditable (false, false, false);
  L_InputFilename->setColour (Label::textColourId, Colours::white);
  L_InputFilename->setColour (TextEditor::textColourId, Colours::black);
  L_InputFilename->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (L_SampleRate = new Label (T("new label"),
                                         T("Sample Rate:")));
  L_SampleRate->setFont (Font (15.0000f, Font::plain));
  L_SampleRate->setJustificationType (juce::Justification::centredLeft);
  L_SampleRate->setEditable (false, false, false);
  L_SampleRate->setColour (Label::textColourId, Colours::white);
  L_SampleRate->setColour (TextEditor::textColourId, Colours::black);
  L_SampleRate->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (L_SampleFormat = new Label (T("new label"),
                                         T("Sample Format:")));
  L_SampleFormat->setFont (Font (15.0000f, Font::plain));
  L_SampleFormat->setJustificationType (juce::Justification::centredLeft);
  L_SampleFormat->setEditable (false, false, false);
  L_SampleFormat->setColour (Label::textColourId, Colours::white);
  L_SampleFormat->setColour (TextEditor::textColourId, Colours::black);
  L_SampleFormat->setColour (TextEditor::backgroundColourId, Colour (0x0));
  
  addAndMakeVisible (L_SampleOutFormat = new Label (T("new label"),
                                         T("Sample Format:")));
  L_SampleOutFormat->setFont (Font (15.0000f, Font::plain));
  L_SampleOutFormat->setJustificationType (juce::Justification::centredLeft);
  L_SampleOutFormat->setEditable (false, false, false);
  L_SampleOutFormat->setColour (Label::textColourId, Colours::white);
  L_SampleOutFormat->setColour (TextEditor::textColourId, Colours::black);
  L_SampleOutFormat->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (C_OutputSampleFormat = new ComboBox (T("C_OutputSampleFormat")));
  C_OutputSampleFormat->setTooltip (T("The sample format and bit-depth. Each of these has an associated dynamic range. For integer formats the dynamic range is fixed; for floating-point formats the value given is in terms of data type\'s mantissa precision."));
  C_OutputSampleFormat->setExplicitFocusOrder (1);
  C_OutputSampleFormat->setEditableText (false);
  C_OutputSampleFormat->setJustificationType (juce::Justification::centredLeft);
  C_OutputSampleFormat->setTextWhenNothingSelected (juce::String::empty);
  C_OutputSampleFormat->setTextWhenNoChoicesAvailable (T("(no choices)"));
  C_OutputSampleFormat->addItem (T("64-bit float (319.1 dB min.)"), 1);
  C_OutputSampleFormat->addItem (T("32-bit integer (192.7 dB)"), 2);
  C_OutputSampleFormat->addItem (T("32-bit float (138.5 dB min.)"), 3);
  C_OutputSampleFormat->addItem (T("24-bit integer (144.5 dB)"), 4);
  C_OutputSampleFormat->addItem (T("16-bit integer (96.3 dB)"), 5);
  C_OutputSampleFormat->addItem (T("8-bit (48.2 dB)"), 6);
  C_OutputSampleFormat->addListener (this);

  addAndMakeVisible (C_OutputSampleRate = new ComboBox (T("C_OutputSampleRate")));
  C_OutputSampleRate->setTooltip (T("The sample rate of the output. The value may be specified either in Hz or kHz."));
  C_OutputSampleRate->setExplicitFocusOrder (1);
  C_OutputSampleRate->setEditableText (true);
  C_OutputSampleRate->setJustificationType (juce::Justification::centredLeft);
  C_OutputSampleRate->setTextWhenNothingSelected (juce::String::empty);
  C_OutputSampleRate->setTextWhenNoChoicesAvailable (T("(no choices)"));
  
  C_OutputSampleRate->addItem (T("192 kHz"), 1);
  C_OutputSampleRate->addItem (T("96 kHz"), 2);
  C_OutputSampleRate->addItem (T("48 kHz"), 3);
  C_OutputSampleRate->addItem (T("176.4 kHz"), 4);
  C_OutputSampleRate->addItem (T("88.2 kHz"), 5);
  C_OutputSampleRate->addItem (T("44.1 kHz"), 6);
  
  C_OutputSampleRate->addListener (this);

  addAndMakeVisible (L_OutputSampleRate = new Label (T("new label"),
                                          T("Sample Rate:")));
  L_OutputSampleRate->setFont (Font (15.0000f, Font::plain));
  L_OutputSampleRate->setJustificationType (juce::Justification::centredLeft);
  L_OutputSampleRate->setEditable (false, false, false);
  L_OutputSampleRate->setColour (Label::textColourId, Colours::white);
  L_OutputSampleRate->setColour (TextEditor::textColourId, Colours::black);
  L_OutputSampleRate->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (L_OutputFilename = new Label (T("new label"),
                                          T("Output Filename:  ")));
  L_OutputFilename->setFont (Font (15.0000f, Font::plain));
  L_OutputFilename->setJustificationType (juce::Justification::centredLeft);
  L_OutputFilename->setEditable (false, false, false);
  L_OutputFilename->setColour (Label::textColourId, Colours::white);
  L_OutputFilename->setColour (TextEditor::textColourId, Colours::black);
  L_OutputFilename->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (B_OctaveDown = new TextButton (T("B_OctaveDown")));
  B_OctaveDown->setExplicitFocusOrder (12);
  B_OctaveDown->setButtonText (T("DOWN"));
  B_OctaveDown->setConnectedEdges (Button::ConnectedOnLeft);
  B_OctaveDown->addButtonListener (this);
  B_OctaveDown->setColour (TextButton::buttonColourId, Colour (0xff6f6b69));
  B_OctaveDown->setColour (TextButton::buttonOnColourId, Colour (0xffaf7b79));
  B_OctaveDown->setColour (TextButton::textColourOnId, Colours::white);
  B_OctaveDown->setColour (TextButton::textColourOffId, Colours::white);
  B_OctaveDown->setClickingTogglesState(true);
  B_OctaveDown->setRadioGroupId(222);

  addAndMakeVisible (B_OctaveUp = new TextButton (T("B_OctaveUp")));
  B_OctaveUp->setExplicitFocusOrder (11);
  B_OctaveUp->setButtonText (T("UP"));
  B_OctaveUp->setConnectedEdges (Button::ConnectedOnRight);
  B_OctaveUp->addButtonListener (this);
  B_OctaveUp->setColour (TextButton::buttonColourId, Colour (0xff6f6b69));
  B_OctaveUp->setColour (TextButton::buttonOnColourId, Colour (0xffaf7b79));
  B_OctaveUp->setColour (TextButton::textColourOnId, Colours::white);
  B_OctaveUp->setColour (TextButton::textColourOffId, Colours::white);
  B_OctaveUp->setClickingTogglesState(true);
  B_OctaveUp->setRadioGroupId(222);

  addAndMakeVisible (T_OutputFilename = new TextEditor (T("T_OutputFilename")));
  T_OutputFilename->setExplicitFocusOrder (4);
  T_OutputFilename->setMultiLine (false);
  T_OutputFilename->setReturnKeyStartsNewLine (false);
  T_OutputFilename->setReadOnly (false);
  T_OutputFilename->setScrollbarsShown (true);
  T_OutputFilename->setCaretVisible (true);
  T_OutputFilename->setPopupMenuEnabled (true);
  T_OutputFilename->setText (juce::String::empty);

  addAndMakeVisible (C_OutputAudioFormat = new ComboBox (T("C_OutputAudioFormat")));
  C_OutputAudioFormat->setExplicitFocusOrder (5);
  C_OutputAudioFormat->setEditableText (false);
  C_OutputAudioFormat->setJustificationType (juce::Justification::centredLeft);
  C_OutputAudioFormat->setTextWhenNothingSelected (juce::String::empty);
  C_OutputAudioFormat->setTextWhenNoChoicesAvailable (T("(no choices)"));
  C_OutputAudioFormat->addItem (T(".aiff"), 1);
  C_OutputAudioFormat->addItem (T(".wav"), 2);
  C_OutputAudioFormat->addItem (T(".au"), 3);
  C_OutputAudioFormat->addItem (T(".raw"), 4);
  C_OutputAudioFormat->addListener (this);

  addAndMakeVisible (L_DitherShape = new Label (T("new label"),
                                         T("Dither Shape:")));
  L_DitherShape->setFont (Font (15.0000f, Font::plain));
  L_DitherShape->setJustificationType (juce::Justification::centredLeft);
  L_DitherShape->setEditable (false, false, false);
  L_DitherShape->setColour (Label::textColourId, Colours::white);
  L_DitherShape->setColour (TextEditor::textColourId, Colours::black);
  L_DitherShape->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (C_DitherShape = new ComboBox (T("C_DitherShape")));
  C_DitherShape->setTooltip (T("Dither is statistical noise that is introduced in order to decorrelate quantization error. The types of dither distribution are Triangular, Rectangular, and Round (no dither). Regardless of the setting, dither is only applied when it is necessary (i.e. when resampling or downconverting)."));
  C_DitherShape->setExplicitFocusOrder (2);
  C_DitherShape->setEditableText (false);
  C_DitherShape->setJustificationType (juce::Justification::centredLeft);
  C_DitherShape->setTextWhenNothingSelected (juce::String::empty);
  C_DitherShape->setTextWhenNoChoicesAvailable (T("(no choices)"));
  //C_DitherShape->addItem (T("Fletcher-Munson"), 1); //Not for now...
  C_DitherShape->addItem (T("Triangular"), 1);
  C_DitherShape->addItem (T("Rectangular"), 2);
  C_DitherShape->addItem (T("Round (off)"), 3);
  C_DitherShape->addListener (this);

  addAndMakeVisible (C_DitherBits = new ComboBox (T("C_DitherBits")));
  C_DitherBits->setTooltip (T("The depth of the dither in bits. Higher values use more dither, but slightly decrease the signal-to-noise ratio."));
  C_DitherBits->setExplicitFocusOrder (3);
  C_DitherBits->setEditableText (false);
  C_DitherBits->setJustificationType (juce::Justification::centredLeft);
  C_DitherBits->setTextWhenNothingSelected (juce::String::empty);
  C_DitherBits->setTextWhenNoChoicesAvailable (T("(no choices)"));
  C_DitherBits->addItem (T("1.0"), 1);
  C_DitherBits->addItem (T("0.9"), 2);
  C_DitherBits->addItem (T("0.8"), 3);
  C_DitherBits->addItem (T("0.7"), 4);
  C_DitherBits->addItem (T("0.6"), 5);
  C_DitherBits->addItem (T("0.5"), 6);
  C_DitherBits->addItem (T("0.4"), 7);
  C_DitherBits->addItem (T("0.3"), 8);
  C_DitherBits->addItem (T("0.2"), 9);
  C_DitherBits->addItem (T("0.1"), 10);
  C_DitherBits->addListener (this);

  addAndMakeVisible (C_Depth = new ComboBox (T("C_Depth")));
  C_Depth->setTooltip (T("The depth of the stopband in the resampling filter in dB. Generally 200 dB suffices for most applications, but you may decrease the value if you are working with lower bit-depths. For correct results, you must make sure that this value is greater than the dynamic range of your sample format."));
  C_Depth->setExplicitFocusOrder (6);
  C_Depth->setEditableText (true);
  C_Depth->setJustificationType (juce::Justification::centredLeft);
  C_Depth->setTextWhenNothingSelected (juce::String::empty);
  C_Depth->setTextWhenNoChoicesAvailable (T("(no choices)"));
  C_Depth->addItem (T("300 dB"), 1);
  C_Depth->addItem (T("250 dB"), 2);
  C_Depth->addItem (T("200 dB"), 3);
  C_Depth->addItem (T("150 dB"), 4);
  C_Depth->addItem (T("100 dB"), 5);
  C_Depth->addItem (T("50 dB"), 6);
  C_Depth->addListener (this);

  addAndMakeVisible (L_Depth = new Label (T("new label"),
                                          T("Stopband Depth:")));
  L_Depth->setFont (Font (15.0000f, Font::plain));
  L_Depth->setJustificationType (juce::Justification::centredLeft);
  L_Depth->setEditable (false, false, false);
  L_Depth->setColour (Label::textColourId, Colours::white);
  L_Depth->setColour (TextEditor::textColourId, Colours::black);
  L_Depth->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (C_Bandwidth = new ComboBox (T("C_Bandwidth")));
  C_Bandwidth->setTooltip (T("The amount of bandwidth loss just below the Nyquist rate. You can decrease this value to a point without adding significant time to the conversion, but once memory runs out on the system, Brick will break up the conversion into multiple passes which causes the conversion time to start increasing inversely proportional to the bandwidth loss."));
  C_Bandwidth->setExplicitFocusOrder (7);
  C_Bandwidth->setEditableText (true);
  C_Bandwidth->setJustificationType (juce::Justification::centredLeft);
  C_Bandwidth->setTextWhenNothingSelected (juce::String::empty);
  C_Bandwidth->setTextWhenNoChoicesAvailable (T("(no choices)"));
  C_Bandwidth->addItem (T("0.01%"), 1);
  C_Bandwidth->addItem (T("0.05%"), 2);
  C_Bandwidth->addItem (T("0.1%"), 3);
  C_Bandwidth->addItem (T("0.5%"), 4);
  C_Bandwidth->addItem (T("1%"), 5);
  C_Bandwidth->addListener (this);

  addAndMakeVisible (L_Bandwidth = new Label (T("new label"),
                                          T("Bandwidth Loss:")));
  L_Bandwidth->setFont (Font (15.0000f, Font::plain));
  L_Bandwidth->setJustificationType (juce::Justification::centredLeft);
  L_Bandwidth->setEditable (false, false, false);
  L_Bandwidth->setColour (Label::textColourId, Colours::white);
  L_Bandwidth->setColour (TextEditor::textColourId, Colours::black);
  L_Bandwidth->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (L_Ratio = new Label (T("new label"),
                                          T("RATIO")));
  L_Ratio->setFont (Font (T("Lucida Grande"), 17.0000f, Font::plain));
  L_Ratio->setJustificationType (juce::Justification::centred);
  L_Ratio->setEditable (false, false, false);
  L_Ratio->setColour (Label::backgroundColourId, Colour (0x0));
  L_Ratio->setColour (Label::textColourId, Colours::white);
  L_Ratio->setColour (TextEditor::textColourId, Colours::black);
  L_Ratio->setColour (TextEditor::backgroundColourId, Colour (0x0));

  addAndMakeVisible (T_RatioNum = new TextEditor (T("T_RatioNum")));
  T_RatioNum->setExplicitFocusOrder (9);
  T_RatioNum->setMultiLine (false);
  T_RatioNum->setReturnKeyStartsNewLine (false);
  T_RatioNum->setReadOnly (false);
  T_RatioNum->setScrollbarsShown (false);
  T_RatioNum->setCaretVisible (true);
  T_RatioNum->setPopupMenuEnabled (true);
  T_RatioNum->setText (T("1"));
  T_RatioNum->addListener(this);

  addAndMakeVisible (T_RatioDen = new TextEditor (T("T_RatioDen")));
  T_RatioDen->setExplicitFocusOrder (10);
  T_RatioDen->setMultiLine (false);
  T_RatioDen->setReturnKeyStartsNewLine (false);
  T_RatioDen->setReadOnly (false);
  T_RatioDen->setScrollbarsShown (false);
  T_RatioDen->setCaretVisible (true);
  T_RatioDen->setPopupMenuEnabled (true);
  T_RatioDen->setText (T("1"));
  T_RatioDen->addListener(this);

  addAndMakeVisible (B_Go = new TextButton (T("B_Go")));
  B_Go->setExplicitFocusOrder (8);
  B_Go->setButtonText (T("GO"));
  B_Go->addButtonListener (this);
  B_Go->setColour (TextButton::buttonColourId, Colour (0xff6f6b69));
  B_Go->setColour (TextButton::textColourOnId, Colours::white);
  B_Go->setColour (TextButton::textColourOffId, Colours::white);

  addAndMakeVisible (L_DragDrop = new Label (T("L_DragDrop"),
                                                T("Drag and Drop File Here")));
  L_DragDrop->setFont (Font (30.7000f, Font::plain));
  L_DragDrop->setJustificationType (juce::Justification::centred);
  L_DragDrop->setEditable (false, false, false);
  L_DragDrop->setColour (Label::textColourId, Colours::white);
  L_DragDrop->setColour (Label::outlineColourId, Colour (0x7dffffff));
  L_DragDrop->setColour (TextEditor::textColourId, Colours::black);
  L_DragDrop->setColour (TextEditor::backgroundColourId, Colour (0x0));

  setSize (592, 448);

  ResetInterface();
}

bool BrickGUI::isInterestedInFileDrag(const juce::StringArray &files)
{
  if(ptrBrickThread) return false;
  return true;
}

void BrickGUI::ResetInterface(void)
{
  if(ptrBrickThread) return;
  C_Bandwidth->setSelectedId(3);
  OutBandwidth = "0.1%";
  
  C_Depth->setSelectedId(3);
  OutDepth = "200dB";
  
  if(prim::OS::Windows())
    C_OutputAudioFormat->setSelectedId(2);
  else
    C_OutputAudioFormat->setSelectedId(1);
  
  C_DitherBits->setSelectedId(1);
  OutDitherBits = 1.0;
  
  C_DitherShape->setSelectedId(1);
  OutDitherShape = "triangle";
  
  T_OutputFilename->setText(juce::String::empty);
  
  C_OutputSampleRate->setSelectedId(1);
  OutSampleRate = 192000;
  
  C_OutputSampleFormat->setSelectedId(4);
  OutSampleFormat = "int24";
  
  L_SampleRate->setText("Sample Rate:", false);
  L_SampleFormat->setText("Sample Format:", false);
  B_OctaveUp->setToggleState(true, false);
  PS_UpDown = "+";
  PS_Octaves = "0";
  PS_Semitones = "P1";
  PS_Cents = ".0";
  PS_Tolerance = 0.1;
  PS = "+0P1.0";
  T_RatioNum->setText("1");
  T_RatioDen->setText("1");
  S_Cents->setValue(0);
  S_Tolerance->setValue(2.5);
  S_Octaves->setValue(0);
  S_Semitones->setValue(0);
  
  L_OutputFilename->setText("Output Filename:  ", false);
  L_OutputFilename->setJustificationType (juce::Justification::centredLeft);
}

void BrickGUI::textEditorTextChanged(juce::TextEditor& editor)
{
  if(ptrBrickThread) return;
  prim::String n(T_RatioNum->getText().toUTF8());
  prim::String d(T_RatioDen->getText().toUTF8());
  
  prim::math::Ratio r(n.ToInteger(), d.ToInteger());
  if(r.Num() <= 0)
    r = 1;
  PS = (integer)r.Num();
  PS &= "/";
  PS &= (integer)r.Den();
  updateOutFilename();
}

void BrickGUI::textEditorReturnKeyPressed(juce::TextEditor& editor)
{
}

void BrickGUI::textEditorEscapeKeyPressed(juce::TextEditor& editor)
{
}

void BrickGUI::textEditorFocusLost(juce::TextEditor& editor)
{
  if(ptrBrickThread) return;
  for(count i = 0; i < PS.n(); i++)
  {
    if(PS[i] == '/')
    {
      T_RatioNum->setText(prim::String((integer)math::Ratio(PS).Num()).Merge());
      T_RatioDen->setText(prim::String((integer)math::Ratio(PS).Den()).Merge());
    }
  }
}


void BrickGUI::filesDropped(const juce::StringArray &files, int x, int y)
{
  if(ptrBrickThread) return;
  FullFilename = files[0];
  
  FileIO helper;
  
  //Set up file info structures.
  SF_INFO s_info;
  prim::Memory::ClearObject(s_info);
  
  ResetInterface();

  SNDFILE* s = sf_open(FullFilename.toUTF8(), SFM_READ, &s_info);
  prim::String s_error = helper.CheckFileError(s);
  if(s_error || (s_info.frames == 0 && s_info.samplerate == 0))
  {
    String ErrorString = "Filename: (the file could not be opened)";
    if(!s_error)
    {
      s_error = "File does not appear to be written in a recognizable audio format.";
      sf_close(s);
    }
    L_InputFilename->setTooltip(juce::String(s_error.Merge()));
    FullFilename = "";
    L_InputFilename->setText(ErrorString.Merge(), false);
    L_DragDrop->setVisible(false);
    //ResetInterface();
    return;
  }
  /*
  
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
  */
  SampleRate = s_info.samplerate;
  SampleType = helper.GetSampleType(s_info, &SampleTypeDescription);
  SampleTypeDescription.Replace(" PCM", "");
  SampleTypeDescription.Replace(" signed", " integer");

  juce::String RateLabelText = "Sample Rate: ";
  if(SampleRate < 1000)
  {
    RateLabelText << (int)SampleRate << " Hz";
  }
  else
  {
    prim::String sr = (number)((int64)SampleRate / 1000.);
    RateLabelText << sr.Merge() << " kHz";
  }
  L_SampleRate->setText(RateLabelText, false);
  
  juce::String FormatLabelText = "Sample Format: ";
  FormatLabelText << SampleTypeDescription.Merge();
  L_SampleFormat->setText(FormatLabelText, false);
  
  InFrames = s_info.frames;
  float64 Minutes = InFrames / (float64)SampleRate / 60.;
  int64 MinutesInteger = (int64)Minutes;
  int64 SecondsInteger = (int64)((Minutes - (float64)MinutesInteger) * 60.);
    
  sf_close(s);
  
  juce::File f(FullFilename);
  juce::String FilenameLabelText = "Filename: ";
  JustFileName = f.getFileName().toUTF8();
  JustFilePath = f.getParentDirectory().getFullPathName().toUTF8();
  if(JustFilePath[JustFilePath.n() - 1] != '/' ||
    JustFilePath[JustFilePath.n() - 1] != '\\')
  {
    JustFilePath.Append((ascii)juce::File::separator);
  }
    
  
  FilenameLabelText << f.getFileName() << " (";
  if(MinutesInteger)
    FilenameLabelText << (int)MinutesInteger << " min. ";
  FilenameLabelText << (int)SecondsInteger << " sec., " << s_info.channels << 
    " ch.)";
  L_InputFilename->setText(FilenameLabelText, false);
  L_InputFilename->setTooltip(FullFilename);
  L_DragDrop->setVisible(false);

  C_OutputSampleRate->setText(juce::String((int)SampleRate), true);
  comboBoxChanged(C_OutputSampleRate);
  
  int FormatId;
  if(SampleType == "float64")
    FormatId = 1;
  else if(SampleType == "int32")
    FormatId = 2;
  else if(SampleType == "float32")
    FormatId = 3;
  else if(SampleType == "int24")
    FormatId = 4;
  else if(SampleType == "int16")
    FormatId = 5;
  else if(SampleType == "int8")
    FormatId = 6;
  C_OutputSampleFormat->setSelectedId(FormatId);
  OutSampleFormat = SampleType;
  
  OutPrefix = JustFileName;
  for(count i = OutPrefix.n() - 1; i > 0; i--)
  {
    if(OutPrefix[i] == '.')
    {
      OutPrefix = OutPrefix.Prefix(i);
      break;
    }
  }

  for(count i = 0; i < OutPrefix.n(); i++)
  {
    if(OutPrefix[i] == '_')
    {
      OutPrefix = OutPrefix.Prefix(i);
      break;
    }
  }
  
  L_OutputFilename->setText(OutPrefix.Merge(), false);
  L_OutputFilename->setJustificationType(juce::Justification::centredRight);
  updateOutFilename();
}

void BrickGUI::updateOutFilename(void)
{
  if(ptrBrickThread) return;
  if(FullFilename != "")
  {
    OutSuffix = "_";
    OutSuffix &= (number)((float64)OutSampleRate / 1000.);
    OutSuffix &= "kHz_";
    OutSuffix &= OutSampleFormat;
    if(PS != "+0P1.0" && PS != "1/1")
    {
      prim::String PS2 = PS;
      OutSuffix = "";
      PS2.Replace("+0", "");
      PS2.Replace("-0", "");
      PS2.Replace("/", "over");
      if(PS2.Suffix(2) == ".0")
        PS2.Replace(".0", "");
      OutSuffix &= "_";
      OutSuffix &= PS2;
    }
  }
  else 
    OutSuffix = "";
  T_OutputFilename->setText(OutSuffix.Merge(), false);
}

void BrickGUI::updateRatioBoxes(void)
{
  if(ptrBrickThread) return;
  PS = "";
  PS &= PS_UpDown;
  PS &= PS_Octaves;
  PS &= PS_Semitones;
  PS &= PS_Cents;
  Console c;
  c += PS;
  FileIO f;
  math::Ratio r = f.GetPitchShiftRatio(PS, PS_Tolerance);
  prim::String n = (integer)r.Num();
  prim::String d = (integer)r.Den();
  T_RatioNum->setText(n.Merge(), false);
  T_RatioDen->setText(d.Merge(), false);
  updateOutFilename();
}

void BrickGUI::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
  if(ptrBrickThread) return;

    if (sliderThatWasMoved == S_Octaves)
    {
        integer v = S_Octaves->getValue();
        PS_Octaves = v;
        updateRatioBoxes();
    }
    else if (sliderThatWasMoved == S_Semitones)
    {
        int v = S_Semitones->getValue();
        switch(v)
        {
          case 0: PS_Semitones = "P1"; break;
          case 1: PS_Semitones = "m2"; break;
          case 2: PS_Semitones = "M2"; break;
          case 3: PS_Semitones = "m3"; break;
          case 4: PS_Semitones = "M3"; break;
          case 5: PS_Semitones = "P4"; break;
          case 6: PS_Semitones = "A4"; break;
          case 7: PS_Semitones = "P5"; break;
          case 8: PS_Semitones = "m6"; break;
          case 9: PS_Semitones = "M6"; break;
          case 10: PS_Semitones = "m7"; break;
          case 11: PS_Semitones = "M7"; break;
        }
        updateRatioBoxes();
    }
    else if (sliderThatWasMoved == S_Cents)
    {
      if(S_Cents->getValue() == 0.5)
      {
        S_Cents->setValue(1.0, true);
        return;
      }
        prim::String v = (prim::number)(S_Cents->getValue() / 100.);
        if(v == "0")
          v = "0.0";
        v.Replace("0.", ".");
        PS_Cents = v;
        updateRatioBoxes();
    }
    else if (sliderThatWasMoved == S_Tolerance)
    {
        PS_Tolerance = prim::String((number)S_Tolerance->getValue());
        updateRatioBoxes();
    }
}

void BrickGUI::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
  if(ptrBrickThread) return;

  if (comboBoxThatHasChanged == C_OutputSampleFormat)
  {
    switch(C_OutputSampleFormat->getSelectedId())
    {
      case 1: OutSampleFormat = "float64"; break;
      case 2: OutSampleFormat = "int32"; break;
      case 3: OutSampleFormat = "float32"; break;
      case 4: OutSampleFormat = "int24"; break;
      case 5: OutSampleFormat = "int16"; break;
      case 6: OutSampleFormat = "int8"; break;
    }
    updateOutFilename();
  }
  else if (comboBoxThatHasChanged == C_OutputSampleRate)
  {
    String s = C_OutputSampleRate->getText().toUTF8();
    String k;
    bool UsedDot = false;
    int64 Multiplier = 1;
    for(count i = 0; i < s.n(); i++)
    {
      ascii c = s[i];
      
      if((c >= '0' && c <= '9') || c == '.')
      {
        if(c == '.' && !UsedDot)
        {
          UsedDot = true;
          Multiplier = 1000;
        }
        else if(c == '.')
          break;
        k.Append(c);
      }
      else if(c == 'k' || c == 'K')
      {
        Multiplier = 1000;
      }
    }
    integer m = k.ToNumber() * Multiplier;
    if(m < 10)
      m = 10;
    else if(m > 80000000)
      m = 80000000;
    OutSampleRate = m;
    String Designation;
    number r;
    if(m < 1000)
    {
      r = m;
      Designation = " Hz";
    }
    else
    {
      r = m / 1000.;
      Designation = " kHz";
    }
    k = r; k &= Designation;
    C_OutputSampleRate->setText(k.Merge(), false);
    updateOutFilename();
  }
  else if (comboBoxThatHasChanged == C_OutputAudioFormat)
  {
  }
  else if (comboBoxThatHasChanged == C_DitherShape)
  {
    switch(C_DitherShape->getSelectedId())
    {
      case 1: OutDitherShape = "triangle"; break;
      case 2: OutDitherShape = "rectangle"; break;
      case 3: OutDitherShape = "off"; break;
    }
  }
  else if (comboBoxThatHasChanged == C_DitherBits)
  {
    switch(C_DitherBits->getSelectedId())
    {
      case 1: OutDitherBits = 1.0; break;
      case 2: OutDitherBits = 0.9; break;
      case 3: OutDitherBits = 0.8; break;
      case 4: OutDitherBits = 0.7; break;
      case 5: OutDitherBits = 0.6; break;
      case 6: OutDitherBits = 0.5; break;
      case 7: OutDitherBits = 0.4; break;
      case 8: OutDitherBits = 0.3; break;
      case 9: OutDitherBits = 0.2; break;
      case 10: OutDitherBits = 0.1; break;
    }
  }
  else if (comboBoxThatHasChanged == C_Depth)
  {
    String s = C_Depth->getText().toUTF8();
    String k;
    for(count i = 0; i < s.n(); i++)
    {
      ascii c = s[i];
      if(c == '.')
        break;
      if(c >= '0' && c <= '9')
      {
        k.Append(c);
      }
    }
    integer m = k.ToInteger();
    if(m < 50)
      k = "50";
    else if(m > 300)
      k = "300";
    k &= " dB";
    C_Depth->setText(k.Merge(), false);
    OutDepth = k;
  }
  else if (comboBoxThatHasChanged == C_Bandwidth)
  {
    String s = C_Bandwidth->getText().toUTF8();
    String k;
    bool UsedDot = false;
    for(count i = 0; i < s.n(); i++)
    {
      ascii c = s[i];
      
      if((c >= '0' && c <= '9') || c == '.')
      {
        if(c == '.' && !UsedDot)
          UsedDot = true;
        else if(c == '.')
          break;
        k.Append(c);
      }
    }
    number m = k.ToNumber();
    if(m < 0.000001)
      k = "0.000001";
    else if(m > 10)
      k = "10";
    k &= "%";
    C_Bandwidth->setText(k.Merge(), false);
    OutBandwidth = k;
  }
}

void BrickGUI::buttonClicked (juce::Button* buttonThatWasClicked)
{
  if(ptrBrickThread) return;

  if (buttonThatWasClicked == B_OctaveDown)
  {
    PS_UpDown = "-";
    updateRatioBoxes();
  }
  else if (buttonThatWasClicked == B_OctaveUp)
  {
    PS_UpDown = "+";
    updateRatioBoxes();
  }
  else if (buttonThatWasClicked == B_Go)
  {
    Console c;
    if(FullFilename == "" || T_OutputFilename->getText() == "")
      return;
    prim::List<prim::String> params;
    params.Add() = "brick";
    params.Add() = FullFilename.toUTF8();
    prim::String outfile = JustFilePath;
    outfile &= OutPrefix;
    outfile &= T_OutputFilename->getText().toUTF8();
    outfile &= C_OutputAudioFormat->getText().toUTF8();
    params.Add() = outfile;
    
    prim::String srate = "--samplerate=";
    srate &= (integer)OutSampleRate;
    srate &= "Hz";
    params.Add() = srate;
    
    prim::String sformat = "--sampleformat=";
    sformat &= OutSampleFormat;
    params.Add() = sformat;
    
    prim::String sdepth = "--depth=";
    sdepth &= OutDepth;
    sdepth.Replace(" ", "");
    params.Add() = sdepth;
    
    prim::String sband = "--allowablebandwidthloss=";
    sband &= OutBandwidth;
    sband.Replace(" ", "");
    params.Add() = sband;
    
    prim::String spitch = "--pitchshift=";
    spitch &= PS;
    params.Add() = spitch;
    
    prim::String stolerance = "--centstolerance=";
    stolerance &= (number)S_Tolerance->getValue();
    params.Add() = stolerance;
    
    prim::String sdither = "--dither=";
    sdither &= OutDitherShape;
    params.Add() = sdither;
    
    prim::String sditherbits = "--ditherbits=";
    sditherbits &= (number)OutDitherBits;
    params.Add() = sditherbits;
    
    ptrBrickThread = new BrickThread;
    c++;
    for(count i = 0; i < params.n(); i++)
    {
      c -= params[i];
      ptrBrickThread->params.Add() = params[i];
    }
    
    ptrBrickThread->startThread();
    startTimer(200);
    B_Go->setButtonText("Preparing...");
    SetAllComponentsEnabled(false);
  }
}

void BrickGUI::timerCallback(void)
{
  if(!ptrBrickThread)
    stopTimer();
    
  if(ptrBrickThread->isThreadRunning())
  {
    if(GlobalWorkInfo::getTotalPasses() == 0 &&
      B_Go->getButtonText() != "Preparing...")
        B_Go->setButtonText("Preparing...");
    else if(GlobalWorkInfo::getTotalPasses() > 0)
    {
      juce::String rs = "Resampling: ";
      prim::String pc = (integer)GlobalWorkInfo::getPercentComplete();
      pc &= "% (pass ";
      pc &= (integer)GlobalWorkInfo::getPassNumber();
      pc &= " of ";
      pc &= (integer)GlobalWorkInfo::getTotalPasses();
      pc &= ")";
      rs << pc.Merge();
      B_Go->setButtonText(rs);
    }
  }
  else
  {
    B_Go->setButtonText("Go");
    delete ptrBrickThread;
    ptrBrickThread = 0;
    stopTimer();
    SetAllComponentsEnabled(true);
  }
}

void BrickGUI::paint (juce::Graphics& g)
{
  using namespace juce;

  g.fillAll (Colour (0xff575757));

  g.setGradientFill (ColourGradient (Colour (0xff2a0d0d),
                                     48.0f, 296.0f,
                                     Colour (0xffc37963),
                                     592.0f, 0.0f,
                                     false));
  g.fillRect (0, 0, 592, 448);

  g.setGradientFill (ColourGradient (Colour (0x808c8c8c),
                                     384.0f, 72.0f,
                                     Colour (0x80f2f2f2),
                                     576.0f, 416.0f,
                                     false));
  g.fillRoundedRectangle (389.0f, 93.0f, 187.0f, 339.0f, 13.0000f);

  g.setColour (Colour (0xffb5b5b5));
  g.drawRoundedRectangle (389.0f, 93.0f, 187.0f, 339.0f, 13.0000f, 1.3000f);

  g.setGradientFill (ColourGradient (Colour (0x808c8c8c),
                                     384.0f, 72.0f,
                                     Colour (0x80f2f2f2),
                                     576.0f, 416.0f,
                                     false));
  g.fillRoundedRectangle (389.0f, 13.0f, 186.0f, 66.0f, 13.0000f);

  g.setColour (Colour (0xffb5b5b5));
  g.drawRoundedRectangle (389.0f, 13.0f, 186.0f, 66.0f, 13.0000f, 1.3000f);
}

