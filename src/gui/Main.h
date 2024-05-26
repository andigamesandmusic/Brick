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

#ifndef Main_h
#define Main_h

#include "Libraries.h"

///The class deriving from JUCEApplication which starts Brick.
class BrickMain : public juce::JUCEApplication
{
public:
  //-----------------------//
  //Initialization/Shutdown//
  //-----------------------//
  BrickMain() {}
  ~BrickMain(){}
  void initialise (const juce::String& commandLine);
  void shutdown() {}

  //------------------//
  //Application Events//
  //------------------//
  void systemRequestedQuit(void) {juce::JUCEApplication::quit();}
  void anotherInstanceStarted(const juce::String& commandLine) {}
  
  //---------------------//
  //Application Constants//
  //---------------------//
  const juce::String getApplicationName(void) {return "Brick";}
  const juce::String getApplicationVersion(void) {return "1.3";}

  //--------------------------//
  //Multiple Instance Checking//
  //--------------------------//
  bool moreThanOneInstanceAllowed(void)
  {
    /*Disable instance checking on Mac for now due to JUCE bug, but on Windows
    and Linux, prevent multiple instances from running and instead open a new
    window. Note that Mac OS X tends to not allow this anyway, so JUCE doesn't 
    really need to take care of it.*/
    return prim::OS::MacOSX();
  }
};

///The main window class for the Brick application.
struct Window : public juce::DocumentWindow
{
  //----------------------//
  //Constructor/Destructor//
  //----------------------//
  Window();

  //------//
  //Events//
  //------//
  void closeButtonPressed(void) {juce::JUCEApplication::quit();}
};
#endif
