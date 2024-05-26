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

#include "Libraries.h"

struct GlobalInfo
{
  List<String> Files;
  List<String> Parameters;
  List<String> ParameterKeys;
  List<String> ParameterValues;
  List<String> ParameterNames;
  List<String> ParameterSettings;
  
  bool ImportFilesAndParameters(List<String>& Arguments);
  
  void CreateParameterKeyValues(void);
  
  void ListFilesAndParameters(void);
  
  String GetValue(String Name);
  
  void AddParameter(String Name, String Value);
  
  bool SetParameter(String Name, String Value);
  
  void CreateListOfParameters(void);
  
  bool IsSpecified(String Name);
  
  bool SetAllParameters(void);
  
  void PrintAllParameters(void);
  
  bool CheckParameterExclusions(void);
};

