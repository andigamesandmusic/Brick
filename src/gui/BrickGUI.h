/*
 ==============================================================================
 
 This file is part of Brick
 Copyright 2010 William Andrew Burnson
 
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

#ifndef BrickGUI_h
#define BrickGUI_h

#include "Libraries.h"

//The Brick command line
void CommandLine(prim::List<prim::String>& Arguments);

struct BrickThread : public juce::Thread
{
  prim::List<prim::String> params;
  
  BrickThread() : juce::Thread("BrickResampleThread") {}
  void run(void) {CommandLine(params);}
};

class BrickGUI : public juce::ButtonListener, public juce::ComboBoxListener,
                 public juce::Component,     public juce::FileDragAndDropTarget,
                 public juce::SliderListener, public juce::TextEditorListener, 
                 public juce::Timer
{
public:
  BrickGUI ();
  ~BrickGUI();

  void ResetInterface(void);

  void paint (juce::Graphics& g);
  void resized();
  void sliderValueChanged (juce::Slider* sliderThatWasMoved);
  void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged);
  void buttonClicked (juce::Button* buttonThatWasClicked);

  void updateRatioBoxes(void);
  void updateOutFilename(void);

  void textEditorTextChanged(juce::TextEditor& editor);
  void textEditorReturnKeyPressed(juce::TextEditor& editor);
  void textEditorEscapeKeyPressed(juce::TextEditor& editor);
  void textEditorFocusLost(juce::TextEditor& editor);

  bool isInterestedInFileDrag(const juce::StringArray &files);
  void filesDropped(const juce::StringArray &files, int x, int y);

  BrickThread* ptrBrickThread;
  void timerCallback(void);
  
  BrickGUI (const BrickGUI&);
  const BrickGUI& operator= (const BrickGUI&);

protected:
  //List of all the component pointers
  List<void*> ComponentPointers;
  
  //Adds all component pointers to list.
  void AddComponentPointers(void);
  
  //Zeroes component pointers.
  void ZeroComponentPointers(void);
  
  //Deletes and zeroes component pointers.
  void DeleteAndZeroComponentPointers(void);
  
  //Sets the enable status of the components.
  void SetAllComponentsEnabled(bool Enabled);

  juce::String FullFilename;
  prim::String JustFilePath;
  prim::String JustFileName;
  prim::int64 SampleRate;
  prim::String SampleType;
  prim::String SampleTypeDescription;
  prim::int64 InFrames;
  prim::String OutPrefix;
  prim::String OutSampleFormat;
  prim::String OutDitherShape;
  prim::float64 OutDitherBits;
  prim::String OutDepth;
  prim::String OutBandwidth;
  prim::int64 OutSampleRate;
  prim::String PS_UpDown;
  prim::String PS_Octaves;
  prim::String PS_Semitones;
  prim::String PS_Cents;
  prim::float64 PS_Tolerance;
  prim::String PS;
  prim::String OutSuffix;

  //Buttons
  juce::TextButton* B_Go;
  juce::TextButton* B_OctaveDown;
  juce::TextButton* B_OctaveUp;
  
  //Combo boxes
  juce::ComboBox* C_Bandwidth;
  juce::ComboBox* C_Depth;
  juce::ComboBox* C_DitherBits;
  juce::ComboBox* C_DitherShape;
  juce::ComboBox* C_OutputAudioFormat;
  juce::ComboBox* C_OutputSampleFormat;
  juce::ComboBox* C_OutputSampleRate;
  
  //Group panes
  juce::GroupComponent* G_InputAudioFile;
  juce::GroupComponent* G_OutputSRC;
  
  //Labels
  juce::Label* L_Bandwidth;
  juce::Label* L_Cents;
  juce::Label* L_Depth;
  juce::Label* L_DitherShape;
  juce::Label* L_DragDrop;
  juce::Label* L_InputFilename;
  juce::Label* L_Octaves;
  juce::Label* L_OutputFilename;
  juce::Label* L_OutputSampleRate;
  juce::Label* L_Ratio;
  juce::Label* L_SampleFormat;
  juce::Label* L_SampleOutFormat;
  juce::Label* L_SampleRate;
  juce::Label* L_Semitones;
  juce::Label* L_Tolerance;
  
  //Sliders
  juce::Slider* S_Cents;
  juce::Slider* S_Octaves;
  juce::Slider* S_Semitones;
  juce::Slider* S_Tolerance;
  
  //Text editors
  juce::TextEditor* T_OutputFilename;
  juce::TextEditor* T_RatioDen;
  juce::TextEditor* T_RatioNum;
};
#endif
