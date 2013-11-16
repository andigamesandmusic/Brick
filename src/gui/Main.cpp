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

//--------//
//Includes//
//--------//

#include "Main.h"
#include "BrickGUI.h"

void BrickMain::initialise(const juce::String& commandLine)
{
  ///Initialize the application with a single window.
  new Window;
}

Window::Window() : juce::DocumentWindow("Brick 1.3", juce::Colours::lightgrey,
    juce::DocumentWindow::allButtons, true)
{
  //A tooltip window.
  new juce::TooltipWindow;
  
  //Set the content component for this window.
  setContentComponent(new BrickGUI, true, false);
  
  //Set size of the window.
  centreWithSize(592, 448);
  
  //Set window properties.
  setResizable(false, false);
  setUsingNativeTitleBar(true);
  setDropShadowEnabled(prim::OS::MacOSX());
  setVisible(true);
}

//Start Brick using the JUCE macro.
START_JUCE_APPLICATION(BrickMain)
