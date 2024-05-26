/*
  ==============================================================================

   This file is part of Brick
   Copyright 2010 by Andi

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

/* Description:
This is a starter file which the other source files in this directory compile
with. It forwards int main() to a higher level CommandLine() function which is
defined below.*/

#if 0
#include "prim.h"
using namespace prim;

void CommandLine(List<String>& Arguments);

int main(int ArgumentCount, const ascii* ArgumentValues[])
{
  Console c;
  List<String> Arguments;
  for(count i = 0; i < (count)ArgumentCount; i++)
  {
    String Arg;
    Arg &= ArgumentValues[i];
    Arguments.Append(Arg);
  }

  CommandLine(Arguments);

  //Add a blank line to the end of the program.
  c++;

  /*On Windows, console application windows disappear when the execution is
  finished, so wait for the user to close the window manually so that they can
  see the results of the execution.*/
  if(OS::Windows())
  {
    c += "Press any key and hit return: ";
    c.WaitForKeyStroke();
  }
  
  return 0;
}

#else

#include "Libraries.h"

void CommandLine(List<String>& Arguments);

///The class deriving from JUCEApplication which starts Brick.
class Brick : public juce::JUCEApplication
{
public:
  //-----------------------//
  //Initialization/Shutdown//
  //-----------------------//
  Brick(){}
  ~Brick(){}
  void initialise (const juce::String& commandLine)
  {
    prim::String s = commandLine.toUTF8();
    prim::String s1;
    List<String> ls;
    ls.Add();
    
    for(count i = 0; i < s.n(); i++)
    {
      char c = s[i];
      if(c == ' ')
      {
        ls.Add() = s1;
        s1 = "";
      }
      else
        s1.Append(c);
    }
    if(s1)
    {
      ls.Add() = s1;
    }
    Console cn;
    cn += "----------------------------------------------------------------------";
    cn += "Brick 1.3, the Brickwall Filter Resampler";
    cn += "  by Andi (c) 2011, licensed GPL";
    cn += "----------------------------------------------------------------------";
    cn ++;
    CommandLine(ls);
    cn++;
    juce::JUCEApplication::quit();
  }
  
  void shutdown() {}

  //------------------//
  //Application Events//
  //------------------//
  void systemRequestedQuit(void) {}
  void anotherInstanceStarted(const juce::String& commandLine) {}
  
  //---------------------//
  //Application Constants//
  //---------------------//
  const juce::String getApplicationName(void) { return "Brick"; }

  const juce::String getApplicationVersion(void) { return "1.3"; }

  //--------------------------//
  //Multiple Instance Checking//
  //--------------------------//
  bool moreThanOneInstanceAllowed(void) { return true; }
};

//Start Brick using the JUCE macro.
START_JUCE_APPLICATION(Brick)

#endif
