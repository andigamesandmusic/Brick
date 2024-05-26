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

#include "Globals.h"

List<String> Files;
List<String> Parameters;
List<String> ParameterKeys;
List<String> ParameterValues;
List<String> ParameterNames;
List<String> ParameterSettings;

bool GlobalInfo::ImportFilesAndParameters(List<String>& Arguments)
{
  Console c;
  
  bool ParameterSpaceStarted = false;
  
  for(count i = 1; i < Arguments.n(); i++)
  {
    String a = Arguments[i];
    if(a.n() >= 2 && a[0] == '-')
    {
      if(a[1] != '-')
      {
        c += "Parameter '"; c &= a; c &= "' is not well-formed. Did you "
          "forget the second hyphen?";
        return false;
      }
      else
      {
        a.TrimBeginning(2);
        Parameters.Add() = a;
        ParameterSpaceStarted = true;
      }
    }
    else
    {
      if(!ParameterSpaceStarted)
        Files.Add() = a;
      else
      {
        c += "Parameter '"; c &= a; c &= "' is not being used in the correct "
          "syntactical context. You must specify files before the parameter "
          "settings. (Also ensure there are no spaces in your parameter "
          "settings.)";
        return false;
      }
    }
  }
  
  if(Files.n() > 2)
  {
    c += "You may not have more than two file arguments (one input file, one "
      " output file).";
    return false;
  }
  else if(Files.n() == 1)
  {
    c += "You must have an output file if an input file is specified.";
    return false;
  }
  
  return true;
}

void GlobalInfo::CreateParameterKeyValues(void)
{
  Console c;
  for(count i = 0; i < Parameters.n(); i++)
  {
    String p = Parameters[i];
    count s = 0;
    if(p.Find("=", s))
    {
      ParameterKeys.Add() = p.Substring(0, s);
      ParameterValues.Add() = p.Substring(s + 1, p.n() - s - 1);
    }
    else
    {
      ParameterKeys.Add() = p;
      ParameterValues.Add() = "";
    }
  }
}

void GlobalInfo::ListFilesAndParameters(void)
{
  Console c;
  c += "Files:";
  c += "------";
  for(count i = 0; i < Files.n(); i++)
    c += Files[i];
  c++;
  
  c += "Parameters:";
  c += "-----------";
  for(count i = 0; i < Parameters.n(); i++)
  {
    c += ParameterKeys[i];
    if(ParameterValues[i])
    {
      c &= "  :  ";
      c &= ParameterValues[i];
    }
  }
  c++;
}

void GlobalInfo::AddParameter(String Name, String Value)
{
  ParameterNames.Add() = Name;
  ParameterSettings.Add() = Value;
}

bool GlobalInfo::SetParameter(String Name, String Value)
{
  Console c;
  if(!Value)
    Value = "yes";
    
  for(count i = 0; i < ParameterNames.n(); i++)
  {
    if(ParameterNames[i] == Name)
    {
      ParameterSettings[i] = Value;
      return true;
    }
  }
  c += "The parameter name '"; c &= Name; c &= "' is not valid. Use "
    "brick --help to list valid parameters.";
  return false;
}

void GlobalInfo::CreateListOfParameters(void)
{
  AddParameter("samplerate", "");
  AddParameter("sampleformat", "");
  AddParameter("inputchannels", "");
  AddParameter("inputsampleformat", "");
  AddParameter("inputsamplerate", "");
  AddParameter("allowablebandwidthloss", "");
  AddParameter("depth", "");
  AddParameter("nofilter", "");
  AddParameter("pitchshift", "");
  AddParameter("centstolerance", "");
  /*AddParameter("lpfcutoff", "");
  AddParameter("lpftransition", "");
  AddParameter("lpfdepth", "");
  AddParameter("hpfcutoff", "");
  AddParameter("hpftransition", "");
  AddParameter("hpfdepth", "");*/
  AddParameter("dither", "");
  AddParameter("ditherbits", "");
  
  /*AddParameter("keeporiginallength", "");
  AddParameter("forceoriginallength", "");
  AddParameter("trim", "");*/
  AddParameter("test", "");
  AddParameter("testlength", "");
  //AddParameter("wisdomtime", "");
  AddParameter("donotloadwisdom", "");
  AddParameter("acquirewisdom", "");
  AddParameter("forgetwisdom", "");
  AddParameter("spectrogramsize", "");
  AddParameter("spectrogramstep", "");
  AddParameter("spectrogrambeta", "");
  AddParameter("gradient", "");
  AddParameter("gradientrange", "");
  AddParameter("convolve", "");
  AddParameter("exportfilter", "");
}

bool GlobalInfo::IsSpecified(String Name)
{
  for(count i = 0; i < ParameterKeys.n(); i++)
  {
    if(ParameterKeys[i] == Name)
      return true;
  }
  return false;    
}

String GlobalInfo::GetValue(String Name)
{
  for(count i = 0; i < ParameterKeys.n(); i++)
  {
    if(ParameterKeys[i] == Name)
      return ParameterValues[i];
  }
  return "";    
}

bool GlobalInfo::SetAllParameters(void)
{
  for(count i = 0; i < ParameterKeys.n(); i++)
    if(!SetParameter(ParameterKeys[i], ParameterValues[i]))
      return false;
  return true;
}

void GlobalInfo::PrintAllParameters(void)
{
  Console c;
  c++;
  c += "Parameter List";
  c += "----------------------------------------------------------------------";
    
  for(count i = 0; i < ParameterNames.n(); i++)
  {
    if(ParameterSettings[i])
    {
      c += ParameterNames[i];
      c &= " : ";
      c &= ParameterSettings[i];
    }
  }    
}

bool GlobalInfo::CheckParameterExclusions(void)
{
  Console c;
  if(IsSpecified("nofilter"))
  {
    if(IsSpecified("depth"))
    {
      c += "--depth parameter incompatible with --nofilter";
      return false;
    }
    if(IsSpecified("allowablebandwidthloss"))
    {
      c += "--allowablebandwidthloss parameter incompatible with --nofilter";
      return false;
    }
  }
  /*
  if(
     (IsSpecified("lpfcutoff") || IsSpecified("lpftransition") ||
      IsSpecified("lpfdepth")) &&
     !(IsSpecified("lpfcutoff") && IsSpecified("lpftransition") &&
      IsSpecified("lpfdepth")) )
  {
    c += "--lpfcutoff --lpftransition and --lpfdepth must all be specified "
      "if one is specified"; return false;
  }
  
  if(
     (IsSpecified("hpfcutoff") || IsSpecified("hpftransition") ||
      IsSpecified("hpfdepth")) &&
     !(IsSpecified("hpfcutoff") && IsSpecified("hpftransition") &&
      IsSpecified("hpfdepth")) )
  {
    c += "--hpfcutoff --hpftransition and --hpfdepth must all be specified "
      "if one is specified"; return false;
  }
  */
  
  if(IsSpecified("centstolerance") && !IsSpecified("pitchshift"))
  {
    c += "--centstolerance can not be used without specifiying --pitchshift";
    return false;
  }
  
  if(IsSpecified("testlength") && !IsSpecified("test"))
  {
    c += "--testlength can not be used without specifiying --test";
    return false;
  }
  
  if(IsSpecified("ditherbits") && !IsSpecified("dither"))
  {
    c += "--ditherbits can not be used without specifiying --dither";
    return false;
  }
  
  /*
  {
    bool p = IsSpecified("keeporiginallength");
    bool q = IsSpecified("forceoriginallength");
    bool r = IsSpecified("trim");
    if((p && (q || r)) || (q && (p || r)) || (r && (q || p)))
    {
      c += "Only one of --trim, --keeporiginallength, --forceoriginallength "
        "may be specified.";
      return false;
    }
  }*/
  
  /*
  if(IsSpecified("wisdomoff"))
  {
    if(IsSpecified("wisdomtime") || IsSpecified("wisdomonly") || 
      IsSpecified("donotsavewisdom") || IsSpecified("acquirewisdom") ||
      IsSpecified("forgetwisdom"))
    {
      c += "--wisdomoff may not be used with --wisdomtime, --wisdomonly, "
        "--donotsavewisdom, --acquirewisdom, --forgetwisdom";
      return false;
    }
  }*/
  
  if(IsSpecified("convolve") && (IsSpecified("samplerate") || 
    IsSpecified("pitchshift")))
  {
    c += "--convolve may not be used with --samplerate or --pitchshift";
    return false;
  }
  
  if(IsSpecified("acquirewisdom") || IsSpecified("forgetwisdom"))
  {
    if(ParameterKeys.n() > 1)
    {
      c += "--acquirewisdom, --forgetwisdom may not be used with any other "
        "parameters";
      return false;
    }
  }
  return true;
}
