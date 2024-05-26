/*
  ==============================================================================

   This file is part of the Belle, Bonne, Sage library
   Copyright 2007-2010 by Andi

  ------------------------------------------------------------------------------

   Belle, Bonne, Sage can be redistributed and/or modified under the terms of
   the GNU Lesser General Public License, as published by the
   Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Belle, Bonne, Sage is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with Belle, Bonne, Sage; if not, visit www.gnu.org/licenses or write:
   
   Free Software Foundation, Inc.
   59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ==============================================================================
*/

#include "prim.h"

//C++ standard library includes -- alphabetical order
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

//C standard library includes -- alphabetical order
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

/*The source code all has to do with wrapping methods from the C++ library, so
that the std namespace and the global ANSI C namespace do not leak into a file
which includes prim or bbs.*/

namespace prim
{
  //-------------------//
  //Externs for primXML//
  //-------------------//
  const unicode::UCS4 XML::Parser::Delimiters::None[1] = {0};
  const unicode::UCS4 XML::Parser::Delimiters::WhiteSpace[5] = {32,9,10,13,0};
  const unicode::UCS4 XML::Parser::Delimiters::TagEntry[2] = {'<',0};
  const unicode::UCS4 XML::Parser::Delimiters::TagExit[2] = {'>',0};
  const unicode::UCS4 XML::Parser::Delimiters::TagName[5] = 
    {'!', '?', '/', '>', 0};
  const unicode::UCS4 XML::Parser::Delimiters::TagAttributeName[4] = 
    {'=','>','/',0};
  const unicode::UCS4 XML::Parser::Delimiters::TagAttributeValue[3] = 
    {'"','>',0};
  const unicode::UCS4 XML::Parser::Delimiters::Generic[6] = 
    {'=','<','>','"','/',0};

  //------------------------//
  //Externs for primProfiler//
  //------------------------//
  Profiler primProfiles;
  bool Profiler::Running = true;
  
  //-----------------------------//
  //Source methods for primRandom//
  //-----------------------------//
  uint32 Random::GetSystemTicks(void)
  {
    return (uint32)clock();
  }
  
  //-------------------------//
  //Source methods for primOS//
  //-------------------------//
  bool OS::Windows(void)
  {
  #ifdef PRIM_WINDOWS
    return true;
  #else
    return false;
  #endif
  }
  
  bool OS::MacOSX(void)
  {
  #ifdef PRIM_MACOSX
    return true;
  #else
    return false;
  #endif
  }
  
  bool OS::Linux(void)
  {
  #ifdef PRIM_LINUX
    return true;
  #else
    return false;
  #endif
  }
  
  bool OS::BSD(void)
  {
  #ifdef PRIM_LINUX
    return true;
  #else
    return false;
  #endif
  }
  
  //----------------------------//
  //Source methods for primTypes//
  //----------------------------//
  bool DataModel::Validate(void)
  {
    int sizeofint8 = sizeof(int8);
    int sizeofint16 = sizeof(int16);
    int sizeofint32 = sizeof(int32);
    int sizeofint64 = sizeof(int64);
    int sizeofvoidptr = sizeof(void*);
    int sizeofuintptr = sizeof(uintptr);
    int sizeofint = sizeof(int);
    int sizeoflong = sizeof(long);

    if(sizeofint8 != 1 || sizeofint16 != 2 || sizeofint32 != 4 ||
       sizeofint64 != 8 || sizeofvoidptr != sizeofuintptr)
    {
      std::cout << "Selected Data Model: " << primDataModel << std::endl;
      std::cout << "int8: " << sizeof(int8) << std::endl;
      std::cout << "int16: " << sizeof(int16) << std::endl;
      std::cout << "int32: " << sizeof(int32) << std::endl;
      std::cout << "int64: " << sizeof(int64) << std::endl;
      std::cout << "intptr: " << sizeof(uintptr) << std::endl;
      std::cout << "voidptr: " << sizeof(void*) << std::endl;
      std::cout << "-----------------------------" << std:: endl;
      std::cout << "FATAL ERROR: Erroneous data type model detected at run-time"
        << ". You must pass the following define for this system to the "
        << "compiler:" << std::endl;
      if(sizeofint == 8)
        std::cout << "PRIM_USING_ILP64";
      else if(sizeoflong == 8)
        std::cout << "PRIM_USING_LP64";
      else if(sizeofvoidptr == 4)
        std::cout << "PRIM_USING_ILP32";
      else
        std::cout << "PRIM_USING_LLP64";
      std::cout << std::endl << std::endl;
      std::cout << "Halting execution..." << std::endl;
      int* a = 0;
      int b = 0;
      *a = b;
    }
    
    return true;
  }
  
  //-------------------------------//
  //Source methods for primProfiler//
  //-------------------------------//
  void Profiler::GetSummaryAsString(String& out, bool SortByFrequency)
  {
    bool wasAlreadyNotRunning=false;
    if(!Running)
      wasAlreadyNotRunning=true;
    else
      Running = false;

    //Do unsorted by default.
    count SortIndex[Profiler::MaximumProfiles];
    for(count i=0;i<ProfileCount;i++)
      SortIndex[i] = i;

    //Do simple sort.
    if(SortByFrequency)
      for(count i=0;i<ProfileCount-1;i++)
        for(count j=i+1;j<ProfileCount;j++)
          if(Durations[SortIndex[i]] < Durations[SortIndex[j]])
          {
            math::Swap(SortIndex[i],SortIndex[j]);
          }

    out = "";
    String s;
    for(int i=0;i<ProfileCount;i++)
    {
      s = "";
      s.AppendInteger(Frequencies[SortIndex[i]], 9,true);
      out += s;

      out &= " at line ";
      
      s = "";
      s.AppendInteger(LineNumbers[SortIndex[i]], 5,true);
      out &= s;
      
      out &= " in file ";

      s = Filenames[SortIndex[i]];
      out &= s;

      count MinColumns = 15;
      if(s.n() < MinColumns)
        for(int i=(int)s.n();i<MinColumns;i++)
          out--;

      number ticks = (number)Durations[SortIndex[i]];

      if(ticks)
      {
        out--;
        out.AppendNumber(ticks / 1000000.0f, 3);
        out &= " megacycles";
      }
    }

    if(!wasAlreadyNotRunning)
      Running = true;
  }

  void Profiler::SendSummaryToConsole(bool SortByFrequency)
  {
    bool wasAlreadyNotRunning=false;
    if(!Running)
      wasAlreadyNotRunning=true;
    else
      Running = false;
    
    Console con;
    String s;
    GetSummaryAsString(s, SortByFrequency);
    con += s;

    if(!wasAlreadyNotRunning)
      Running = true;
  }

  //-----------------------------//
  //Source methods for primMemory//
  //-----------------------------//

  void Memory::Set(void* Destination, uint8 ValueToSet, count BytesToSet)
  {
    _profile(primProfiles);

    if(Destination != 0 && BytesToSet > 0)
      memset(Destination,ValueToSet,BytesToSet);

    _endprofile(primProfiles);
  }

  void Memory::Copy(void* Destination, void* Source, count BytesToCopy)
  {
    _profile(primProfiles);

    if(Destination != 0 && Source != 0 && BytesToCopy > 0)
      memcpy(Destination,Source,BytesToCopy);

    _endprofile(primProfiles);
  }

  //------------------------------//
  //Source methods for primConsole//
  //------------------------------//

  /*The implementation resides in this source file so that <iostream> will not
  pollute the user's namespace.*/
  void Console::Out(const ascii* s)
  {
    std::cout << s;
    BeginningOfStream = false;
  }

  void Console::WaitForKeyStroke(void)
  {
    char c;
    std::cin >> c;
  }

  bool Stream::BeginningOfStream = true;

  //---------------------------//
  //Source methods for primFile//
  //---------------------------//
  count File::Read(const ascii* Filename, byte*& ByteArray)
  {
    //Create an input file stream.
    std::ifstream FileStream;

    //Open the file, but do not truncate it.
    FileStream.open(Filename,
      std::ios::in | std::ios::binary);

    //Get out of here if for some reason the file could not be opened.
    if(!FileStream.is_open())
    {
      ByteArray=0;
      return 0;
    }

    //Seek to the end of the file to get the length.
    FileStream.seekg(0,std::ios_base::end);
    count Length = (count)FileStream.tellg();

    //Allocate an array of bytes.
    ByteArray = new byte[Length+1];

    //Seek to the beginning of the file and read into the byte array.
    FileStream.seekg(0,std::ios_base::beg);
    FileStream.read((ascii*)ByteArray, Length);

    //Add a null-terminator in case this is converted to a string.
    ByteArray[Length]=0;

    //Return the length of the file.
    return Length;
  }

  bool File::Write(const ascii* Filename, const byte* ByteArray, 
    count BytesToWrite)
  {
    //Create an input file stream.
    std::ofstream FileStream;

    //Open the file and truncate it.
    FileStream.open(Filename,
      std::ios::out | std::ios::trunc | std::ios::binary);

    //Get out of here if for some reason the file could not be opened.
    if(!FileStream.is_open())
      return false;

    //Write the byte array to the file.
    FileStream.write((ascii*)ByteArray, BytesToWrite);

    //Close the file.
    FileStream.close();
    
    //Return success.
    return true;
  }
  
  
  bool File::Append(const ascii* Filename, const byte* ByteArray, 
    count BytesToWrite)
  {
    //Create an input file stream.
    std::ofstream FileStream;

    //Open the file and truncate it.
    FileStream.open(Filename,
      std::ios::out | std::ios::app | std::ios::binary);

    //Get out of here if for some reason the file could not be opened.
    if(!FileStream.is_open())
      return false;

    //Write the byte array to the file.
    FileStream.write((ascii*)ByteArray, BytesToWrite);
    
    //Close the file.
    FileStream.close();

    //Return success.
    return true;
  }

  bool File::Replace(const ascii* Filename, const ascii* someString)
  {
    //Create an output file stream.
    std::ofstream FileStream;

    //Open the file, but do not truncate it.
    FileStream.open(Filename,
      std::ios::out | std::ios::trunc | std::ios::binary);

    //Get out of here if for some reason the file could not be opened.
    if(!FileStream.is_open())
      return false;

    //Calculate the length of the character array.
    count Length = String::Section::SubstringLength(someString);

    //Replace the file with the string.
    FileStream.write(someString,Length);

    //Close the file.
    FileStream.close();

    return true;
  }

  bool File::Append(const ascii* Filename, const ascii* someString)
  {
    //Create an output file stream.
    std::ofstream FileStream;

    //Open the file, but do not truncate it.
    FileStream.open(Filename, std::ios::out | std::ios::app | std::ios::binary);

    //Get out of here if for some reason the file could not be opened.
    if(!FileStream.is_open())
      return false;

    //Calculate the length of the character array.
    count Length = String::Section::SubstringLength(someString);

    //Append the string to the file.
    FileStream.write(someString,Length);

    //Close the file.
    FileStream.close();

    return true;
  }

  //--------------------------//
  //Source methods for primXML//
  //--------------------------//
  void XML::Element::AppendToString(String& XMLOutput)
  {
    static count TabLevel = 0;

    /*First give the virtual method a chance to update the XML data 
    structure.*/
    Translate();

    String TabString;
    for(count i = 0; i < TabLevel; i++)
      TabString &= "  ";
    XMLOutput += TabString;
    XMLOutput &= "<";
    XMLOutput &= Name;

    //Write the attributes within the tag.
    for(count i = 0; i < Attributes.n(); i++)
    {
      XMLOutput -= Attributes[i].Name;
      XMLOutput &= "=\"";
      XMLOutput &= Attributes[i].Value;
      XMLOutput &= "\"";
    }

    //Write the objects to the element.
    if(Objects.n() > 0)
    {
      XMLOutput &= ">";
      bool ContainsTags = false;
      for(count i = 0; i < Objects.n(); i++)
      {
        Object* e = Objects[i];
        Text* t = e->IsText();
        Element* el = e->IsElement();
        if(t)
          XMLOutput &= *(String*)t;
        else
        {
          ContainsTags = true;
          TabLevel++;
          el->AppendToString(XMLOutput);
          TabLevel--;
        }
      }
      if(ContainsTags)
        XMLOutput += TabString;
      XMLOutput &= "</";
      XMLOutput &= Name;
      XMLOutput &= ">";
    }
    else
    {
      //Use self-closing syntax: <br/>
      XMLOutput &= "/>";
    } 
  }

  //---------------------------//
  //Source methods for primMath//
  //---------------------------//

  namespace math
  {
    const number HalfPi = (number)1.5707963267948966192313216916398;
    const number Pi = (number)3.1415926535897932384626433832795;
    const number ThreeHalvesPi = (number)4.7123889803846898576939650749193;
    const number TwoPi = (number)6.283185307179586476925286766559;
    const number DegToRad = Pi / (number)180.0;
    const number RadToDeg = (number)180.0 / Pi;
    
    bool IsApproximatelyEqual(number Number1, number Number2,
      count DigitsOfSensitivity)
    {
      number Delta = Exp(10.f, (number)-DigitsOfSensitivity) * 0.5f;
      number Difference = Number1 - Number2;
      return Difference >= 0.f && Difference < Delta || 
        Difference < 0 && -Difference < Delta;
    }

    number Exp(number x)
    {
      return exp(x);
    }

    number Exp(number base, number x)
    {
      return pow(base, x);
    }

    number Log(number x)
    {
      return log(x);
    }

    number Log(number base, number x)
    {
      return log(x) / log(base);
    }

    number Sin(number Radians)
    {
      return sin(Radians);
    }

    number Cos(number Radians)
    {
      return cos(Radians);
    }

    number Tan(number Radians)
    {
      return tan(Radians);
    }

    number ArcSin(number x)
    {
      return asin(x);
    }

    number ArcCos(number x)
    {
      return acos(x);
    }

    number ArcTan(number x)
    {
      return atan(x);
    }

    number Deg(number Radians)
    {
      return Radians * (number)57.295779513082320876798154814105;
    }

    number Rad(number Degrees)
    {
      return Degrees * (number)0.017453292519943295769236907684886;
    }

    integer Sgn(integer x)
    {
      return (x == 0 ? 0 : (x < 0 ? (integer)-1 : (integer)1));
    }

    number Sgn(number x)
    {
      return (x == 0 ? 0 : (x < 0 ? (number)-1 : (number)1));
    }

    integer Abs(integer x)
    {
      return (x < 0 ? -x : x);
    }

    number Abs(number x)
    {
      return (x < 0 ? -x : x);
    }

    number Sqrt(number x)
    {
      return sqrt(x);
    }

    number Dist(number x, number y)
    {
      return Sqrt(x * x + y * y);
    }

    number Dist(number x1, number y1, number x2, number y2)
    {
      number DeltaX = x1 - x2;
      number DeltaY = y1 - y2;
      return sqrt(DeltaX * DeltaX + DeltaY * DeltaY);
    }

    number Dist(Vector& v1, Vector& v2)
    {
      number DeltaX = v2.x - v1.x;
      number DeltaY = v2.y - v1.y;
      return sqrt(DeltaX * DeltaX + DeltaY * DeltaY);
    }

    integer Mod(integer x, integer y)
    {
      if(x>=0 || Abs((integer)(-1L % 3L)) == 2)
        return Abs((integer)(x % y)); //This platform usesthe cyclic form.
      else
        return (integer)((y-Abs((integer)(x % y))) % y);
    }

    integer ModAbs(integer x, integer y)
    {
      if(x>=0 || Abs((integer)(-1L % 3L)) == 1)
        return Abs((integer)(x % y)); //This platform uses absolute value form.
      else
        return (integer)(y-Abs((integer)(x % y)));
    }

    count Roots(number a, number b, number c,
      number& Root1, number& Root2)
    {
      _profile(primProfiles);
      //Implemented according to the algorithm given at:
      //http://en.wikipedia.org/wiki/Quadratic_equation

      number Determinant = Sqrt(b * b - 4 * a *c);

      if(Determinant>0)
      {
        number t = (number)-0.5 * (b + Sgn(b) * Determinant);
        if(t!=0)
        {
          Root1 = c/t;
          if(a!=0)
            Root2 = t/a;
          else
          {
            _endprofile(primProfiles);
            return 1;
          }
        }
        else
        {
          Root1 = (-b + Determinant) * (number)0.5 * a;
          Root2 = (-b - Determinant) * (number)0.5 * a;
        }
        Ascending(Root1,Root2);
        _endprofile(primProfiles);
        return 2;
      }
      else if(Determinant==0)
      {
        Root1 = (number)0.5 * b / a;
        Root2 = 0;
        _endprofile(primProfiles);
        return 1;
      }
      else
      {
        Root1 = 0;
        Root2 = 0;
        _endprofile(primProfiles);
        return 0;
      }
    }

    /**Returns a random integer in a particular range. The order of the
    arguments does not make a difference. Note that the upper integer is 
    excluded. In other words the interval is [Low, High).*/
    integer Rand(integer Floor, integer Ceiling)
    {
      static Random r;
      return r.NextIntegerInRange(Floor, Ceiling);
    }

    /**Returns a random number in a particular range. The order of the arguments
    does not make a difference. Note that the upper number is excluded. In other
    words the interval is [Low, High).*/
    number RandNumber(number Floor, number Ceiling)
    {
      static Random r;
      return r.NextNumberInRange(Floor, Ceiling);
    }

    integer Rand(integer Ceiling)
    {
      static Random r;
      return r.NextIntegerInRange(0, Ceiling);
    }

    number RandNumber(number Ceiling)
    {
      static Random r;
      return r.NextNumberInRange(0, Ceiling);
    }

    String NumberToString(number Number, count SignificantFigures)
    {
      if(Abs(Number) > 99.9f && Abs(Number) < 100.1f)
        Number = Number;
      _profile(primProfiles);
      //Set boundaries on the significant figure range.
      if(SignificantFigures < 1)
        SignificantFigures = 1;
      if(SignificantFigures > 17)
        SignificantFigures = 17;

      /*If the number is very large or very small, then just print a large
      number or zero.*/
      if(Number > (number)1e10)
      {
        _endprofile(primProfiles);
        return String("10000000000");
      }
      if(Number < (number)-1e10)
      {
        _endprofile(primProfiles);
        return String("-10000000000");
      }
      if(Number > (number)-1e-10 && Number < (number)1e-10)
      {
        _endprofile(primProfiles);
        return String("0");
      }

      //Ask for a particular number of digits
      const count Digits = 30;

      //Create a stringstream converter.
      std::stringstream Converter;

      //Use a 'fixed' decimal point
      Converter.setf(std::ios::fixed);

      //Disable scientific notation.
      Converter.unsetf(std::ios::scientific);

      //Set precision to so many digits.
      Converter.precision(Digits);
      Converter << Number;

      //Send the results to a string for further formatting.
      String Unformatted = String(Converter.str().c_str());

      //Copy the unformatted string to a buffer.
      count DataLength = Unformatted.n();
      ascii* Data = new ascii[DataLength + 1];
      Memory::CopyArray(Data, Unformatted.Merge(),
        Unformatted.ByteLength()+1);


      //Trim any significant figures that weren't requested.
      count SigFigCount = 0;
      bool PastDecimal = false;
      bool InSigFigs = false;
      for(count i = 0; i < DataLength; i++)
      {
        ascii Digit = Data[i];

        if(Digit == '.')
        {
          PastDecimal = true;
          if(SigFigCount >= SignificantFigures)
          {
            Data[i] = 0;
            DataLength = i + 1;
            break;
          }
        }
        else if(!InSigFigs && (Digit == '0' || Digit == '-'))
          ;
        else
        {
          InSigFigs = true;
          if(SigFigCount == SignificantFigures)
          {
            /*Do rounding so that we don't incur error when the same number is
            converted to and from string many times.*/
            if(Data[i] >= '5')
            {
              //Perform rounding up.
              bool NeedsToCarryOne = false;
              for(count j = i - 1; j >= 0; j--)
              {
                if(Data[j] == '.')
                  continue;
                else if(Data[j] == '9')
                {
                  Data[j] = '0';
                  NeedsToCarryOne = true;
                  continue;
                }
                else if(Data[j] != '-')
                {
                  Data[j] = Data[j] + 1;
                  NeedsToCarryOne = false;
                  break;
                }
              }

              /*Had 99.9999 or something like that and need to prepend '1' so
              that the rounding carries correctly.*/
              if(NeedsToCarryOne)
              {
                for(count j = DataLength - 1; j >= 1; j--)
                  Data[j] = Data[j - 1];
                if(Data[1] == '-')
                {
                  Data[0] = '-';
                  Data[1] = '1';
                }
                else
                  Data[0] = '1';
              }
            }
            if(PastDecimal)
            {
              Data[i] = 0;
              DataLength = i;
              break;
            }
            else
              Data[i] = '0';
          }
          else if(SigFigCount > SignificantFigures)
            Data[i] = '0';
          else
            SigFigCount++;
        }
      }

      //Get rid of trailing zeroes.
      for(count i = DataLength - 1; i >= 1; i--)
      {
        ascii Digit = Data[i];

        if(Digit == '0')
          Data[i] = 0;
        else if(Digit == '.')
        {
          Data[i] = 0;
          break;
        }
        else
          break;
      }

      //Return the formatted number.
      String Formatted = String(Data);
      delete [] Data;
      _endprofile(primProfiles);
      return Formatted;
    }

    number StringToNumber(const String& NumericString)
    {
      return (number)atof(NumericString.Merge());
    }

    integer StringToInteger(const String& NumericString)
    {
      return (integer)atol(NumericString.Merge());
    }
  }
}
