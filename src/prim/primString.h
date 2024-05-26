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

#ifndef primString
#define primString

#include "primList.h"
#include "primMath.h"
#include "primMemory.h"
#include "primProfiler.h"
//#include "primRational.h"
#include "primTypes.h"
#include "primUnicode.h"
#include "primVector.h"

namespace prim
{
  class String
  {
  public:
    //EMBEDDED CLASS//

    ///A String is made of a linked list of Sections
    struct Section
    {
      ///Pointer to a string of characters containing a UTF8 substring
      ascii* Substring;

      ///Length of the substring
      count Length;

      ///Number of characters in the substring (equals Length if ASCII)
      count Characters;

      ///Creates a section with no allocated substring
      Section() : Substring(0), Length(0), Characters(0) {}

      ///Destroys the substring.
      ~Section()
      {
        _profile(primProfiles);

        delete [] Substring;

        _endprofile(primProfiles);
      }

      /**\brief Returns the length in bytes of a const ascii* without the
      null-terminator.*/
      static count SubstringLength(const ascii* someSubstring)
      {
        _profile(primProfiles);

        const ascii* beginningOfSubstring = someSubstring;
        while(*someSubstring!=0)
        {
          _profile(primProfiles);
          someSubstring++;
          _endprofile(primProfiles);
        }

        _endprofile(primProfiles);
        return (count)(someSubstring - beginningOfSubstring);
      }

      ///Initializes this Section with a substring
      void Initialize(const ascii* newSubstring)
      {
        _profile(primProfiles);

        //Delete any previous substring.
        delete [] Substring;

        //Calculate the number of bytes in the substring.
        Length = SubstringLength(newSubstring);

        //Calculate the number of characters in the substring.
        Characters = unicode::UTF8::Characters(newSubstring);

        //Add a null-terminator when creating the memory.
        Substring = new ascii[Length+1];

        //Copy the string in.
        Memory::CopyArray(Substring,newSubstring,Length+1);

        _endprofile(primProfiles);
      }

      /**\brief Initializes this Section by copying a pointer to a
      preallocated substring.*/
      void Initialize(ascii* newSubstring, count newLength)
      {
        _profile(primProfiles);

        //Delete any previous substring.
        delete [] Substring;

        //Copy in the length and pointer to the substring
        Length=newLength;
        Substring=newSubstring;

        //Calculate the number of characters in the substring.
        Characters = unicode::UTF8::Characters(newSubstring);

        _endprofile(primProfiles);
      }
    };

  private:
    //-------------//
    //Internal Data//
    //-------------//

    /**\brief Stores the number of bytes the String holds as the sum total
    of the number of bytes in each section.*/
    count Length;

    /**\brief Stores the number of characters the String contains as the sum
    total of the number of characters in each section. \details If this
    number is the same as ByteLength, then the String only uses the ASCII
    character set. Otherwise, it is encoded with variable-byte characters
    per UTF-8.*/
    count Characters;

    ///Stores sections of substrings that together compose the String.
    mutable List<Section> Sections;

  public:

    //----------//
    //Properties//
    //----------//

    ///Returns the length of the string in characters.
    inline count CharacterLength(void) const {return Characters;}

    ///Shorthand to access the length of the string in characters
    inline count n(void) const {return Characters;}

    ///Returns the length of the string in bytes.
    inline count ByteLength(void) const {return Length;}

    //----------//
    //Characters//
    //----------//
    /**Gets a single UTF-8 character as a UCS4 codepoint. Note that in UTF-8
    random access is not really efficient because of the variable-length 
    encoding. However, since String uses sections internally to represent the
    string, traversing will be faster as irrelevant sections do not need to be
    traversed.*/
    inline unicode::UCS4 Character(count i) const
    {
      if(i < 0 || i >= Characters)
        return 0;
      count CurrentIndex = 0;
      for(count j = 0; j < Sections.n(); j++)
      {
        count SectionCharacters = Sections[j].Characters;
        
        if(CurrentIndex + SectionCharacters > i)
        {
          //In the right section, now decode UTF-8 until the index is found.
          const ascii* SectionString = Sections[j].Substring;
          unicode::UCS4 CurrentCharacter = 0;
          for(count k = 0; k <= i - CurrentIndex; k++)
            CurrentCharacter = unicode::UTF8::Decode(SectionString);

          return CurrentCharacter;
        }
        else
          CurrentIndex += SectionCharacters;
      }
      return 0;
    }

    /**Gets a single UTF-8 character as a UCS4 codepoint. Note that in UTF-8
    random access is not really efficient because of the variable-length 
    encoding. However, since String uses sections internally to represent the
    string, traversing will be faster as irrelevant sections do not need to be
    traversed.*/
    inline unicode::UCS4 operator [] (count i) const
    {
      return Character(i);
    }

    //--------//
    //Clearing//
    //--------//

    ///Clears the string.
    void Clear(void)
    {
      _profile(primProfiles);

      Sections.RemoveAll();
      Characters = Length = 0;

      _endprofile(primProfiles);
    }

    //------------------------//
    //Collapsing and Expanding//
    //------------------------//

    /**\brief Appends all the substrings together as a single string so that
    the characters can be accessed as a contiguous array.*/
    const ascii* Merge(void) const
    {
      _profile(primProfiles);

      //Precalculate the number of sections.
      count NumberOfSections = Sections.Size();

      //If we do not have any sections, create at least a null string.
      if(NumberOfSections==0)
      {
        Sections.Add();
        Sections[0].Initialize("");
      }

      //We don't need to do this if there is only one section in the list.
      if(NumberOfSections==1)
      {
        _endprofile(primProfiles);
        return Sections[0].Substring;
      }

      /*Created a new substring that is as large as needed to store the
      whole string as a contiguous ascii array.*/
      count MergedLength = ByteLength();
      ascii* MergedSubstring = new ascii[MergedLength+1];

      //Copy all the substrings into the merged substring.
      ascii* ptrDestination = MergedSubstring;
      for(count i=0;i<NumberOfSections;i++)
      {
        Section& CurrentSection=Sections[i];
        count SourceLength = CurrentSection.Length;
        ascii* ptrSource = CurrentSection.Substring;
        Memory::CopyArray(ptrDestination,ptrSource,SourceLength);
        ptrDestination+=SourceLength;
      }

      //Add a null terminator.
      *ptrDestination=0;

      //Destroy the fragmented string and replace it with the merged copy.
      Sections.RemoveAll();
      Sections.Add();
      Sections[0].Initialize(MergedSubstring,MergedLength);

      //Return a pointer to the substring.
      _endprofile(primProfiles);
      return MergedSubstring;
    }

    /**\brief Splits the string internally at some index point by breaking
    the relevant section into two sections. \details Mostly this is used
    internally by Insert(). It returns the index of the left partition.*/
    count Split(count beforeIndex) const
    {
      _profile(primProfiles);

      //Determine if splitting is necessary.
      if(beforeIndex <= 0)
      {
        _endprofile(primProfiles);
        return -1;
      }
      else if(beforeIndex >= Characters)
      {
        _endprofile(primProfiles);
        return Sections.n() - 1;
      }

      count LeftCharacterIndex=0;
      count RightCharacterIndex=0;
      count LeftByteIndex=0;
      count RightByteIndex=0;

      //Look for the section that contains the index that was passed in.
      for(count i=0;i<Sections.n();i++)
      {
        Section& thisSection = Sections[i];

        LeftCharacterIndex = RightCharacterIndex;
        RightCharacterIndex += thisSection.Characters;
        LeftByteIndex = RightByteIndex;
        RightByteIndex += thisSection.Length;

        if(beforeIndex == LeftCharacterIndex ||
          beforeIndex == RightCharacterIndex)
        {
          _endprofile(primProfiles);
          return i; //No need to split
        }
        else if(beforeIndex > LeftCharacterIndex
          && beforeIndex < RightCharacterIndex)
        {
          //Split this section into two.
          count LeftPartCharacters =
            beforeIndex - LeftCharacterIndex;

          count LeftPartBytes = 0;
          count RightPartBytes = 0;
          for(count j=0;j<LeftPartCharacters;j++)
          {
            using namespace unicode;
            LeftPartBytes += UTF8::Octets(
              (UTF8::octet)thisSection.Substring[LeftPartBytes]);
          }
          RightPartBytes = thisSection.Length - LeftPartBytes;

          //Allocate memory for the new substrings.
          ascii* LeftPart = new ascii[LeftPartBytes+1];
          ascii* RightPart = new ascii[RightPartBytes+1];

          //Copy the section's string into the two new substrings.
          Memory::CopyArray(LeftPart,
            thisSection.Substring,LeftPartBytes);
          Memory::CopyArray(RightPart,
            &thisSection.Substring[LeftPartBytes],RightPartBytes);

          //Add null-terminators.
          LeftPart[LeftPartBytes]=0;
          RightPart[RightPartBytes]=0;

          //Create two blank sections.
          Section BlankSection;
          Sections.InsertAfter(BlankSection,i);
          Sections.InsertAfter(BlankSection,i+1);

          //Remove the old section.
          Sections.Remove(i);

          //Retrieve the new sections.
          Section& LeftSection = Sections[i];
          Section& RightSection = Sections[i+1];

          //Initialize the two sections with their new substrings.
          LeftSection.Initialize(LeftPart,LeftPartBytes);
          RightSection.Initialize(RightPart,RightPartBytes);

          //Return the left partion index.
          _endprofile(primProfiles);
          return i;
        }
      }

      //This line should never be executed.
      _endprofile(primProfiles);
      return -1;
    }

    /**Forces the internal representation to split into sections. Since UTF-8
    is a variable length encoding, it is impossible to seek randomly without
    some kind of traversal. Fortunately, since String is already represented as
    a list of substrings, you can substantially reduce traversal time by 
    deliberately having the string break into sections of known byte length.
    This may also speed up Insert method.*/
    void SplitIntoParts(count SectionCharacterLength) const
    {
      const ascii* EntireString = Merge();
      if(SectionCharacterLength <= 0 || SectionCharacterLength >= Characters)
        return;
      count NumberOfSections = Characters / SectionCharacterLength;
      count FinalCharactersToCopy = SectionCharacterLength;
      if(math::Mod(Characters, SectionCharacterLength) > 0)
      {
        NumberOfSections++;
        FinalCharactersToCopy = (count)math::Mod(Characters, 
          SectionCharacterLength);
      }

      const ascii* BeginningOfSection = EntireString;
      for(count i = 0; i < NumberOfSections; i++)
      {
        const ascii* CurrentCharacter = BeginningOfSection;
        count CharactersToCopy = SectionCharacterLength;
        if(i == NumberOfSections - 1)
          CharactersToCopy = FinalCharactersToCopy;
        for(count j = 0; j < CharactersToCopy; j++)
          unicode::UTF8::Decode(CurrentCharacter);
        count Bytes = (count)(CurrentCharacter - BeginningOfSection);
        Sections.Add();
        ascii* NewSection = new ascii[Bytes + 1];
        Memory::CopyArray(NewSection, BeginningOfSection, Bytes);
        NewSection[Bytes] = 0;
        Sections.last().Initialize(NewSection);
        BeginningOfSection = CurrentCharacter;
      }

      Sections.Remove(0);
    }

    /**Internally splits the strings to optimize random access. It is useful to
    call this just before looping through the character codes using the []
    operator for example. Note that Merge() will unoptimize the string 
    completely. The Substring and Subsection methods gradually fragment the 
    string but on average the string will be fragmented at Sqrt(n) * 2 + 1 
    sections.*/
    void SplitOptimally(void) const
    {
      if(Sections.n() > (integer)math::Sqrt((number)Characters) * 2
        || Sections.n() < (integer)math::Sqrt((number)Characters) / 2)
      SplitIntoParts((count)math::Sqrt((number)Characters));
    }

    //--------//
    //Trimming//
    //--------//

    /**Removes outer portions of a string while retaining an inner portion. Trim
    is like Substring; however, this method actually alters the string, whereas
    Substring returns a copy.*/
    void Trim(count startIndex, count endIndex)
    {
      //Fringe cases...
      if(Characters == 0)
        return;
      if(startIndex > endIndex || 
        startIndex >= Characters ||
        endIndex < 0)
      {
        Clear();
        return;
      }
      if(startIndex < 0)
        startIndex = 0;
      if(endIndex >= Characters)
        endIndex = Characters - 1;

      /*First split the string at those points so that we don't necessarily
      have to recopy everything.*/
      Split(startIndex);
      Split(endIndex + 1);

      count CharactersFromEndToRemove = Characters - endIndex - 1;
      count CharactersFromStartToRemove = startIndex;
      count CharactersFromEndRemoved = 0;
      count CharactersFromStartRemoved = 0;
      count BytesRemoved = 0;

      //Remove the sections at the end of the string.
      while(CharactersFromEndToRemove != CharactersFromEndRemoved)
      {
        CharactersFromEndRemoved += Sections.last().Characters;
        BytesRemoved += Sections.last().Length;
        Sections.Remove(Sections.n() - 1);
      }

      //Remove the sections from the beginning of the string.
      while(CharactersFromStartToRemove != CharactersFromStartRemoved)
      {
        CharactersFromStartRemoved += Sections.first().Characters;
        BytesRemoved += Sections.first().Length;
        Sections.Remove(0);
      }

      //Update the lengths.
      Characters -= (CharactersFromEndRemoved + CharactersFromStartRemoved);
      Length -= BytesRemoved;
    }

    ///Removes a certain number of characters from the beginning.
    void TrimBeginning(count charactersToTrim)
    {
      Trim(charactersToTrim, Characters - 1);
    }

    ///Removes a certain number of characters from the end.
    void TrimEnd(count charactersToTrim)
    {
      Trim(0, Characters - 1 - charactersToTrim);
    }

    //----------//
    //Substrings//
    //----------//

    ///Returns a subsection from a start and end index (inclusive).
    void Subsection(String& Destination, count startIndex, count endIndex) const
    {
      //Clear the destination before copying the substring in.
      Destination.Clear();

      //Fringe cases...
      if(Characters == 0 ||
        startIndex > endIndex || 
        startIndex >= Characters ||
        endIndex < 0)
        return;
      if(startIndex < 0)
        startIndex = 0;
      if(endIndex >= Characters)
        endIndex = Characters - 1;

      //Make section boundaries too simplify the copying algorithm.
      Split(startIndex);
      Split(endIndex + 1);

      //Find the sections that are relevant for copying.
      count BeginningSection = 0;
      count EndingSection = Sections.n() - 1;

      //Find the beginning section.
      count CurrentCharacter = 0;
      for(count i = 0; i < Sections.n(); i++)
      {
        if(CurrentCharacter == startIndex)
        {
          BeginningSection = i;
          break;
        }
        CurrentCharacter += Sections[i].Characters;
      }

      //Get the total length and find the last section.
      count TotalLength = 0;
      CurrentCharacter = 0;
      for(count i = BeginningSection; i < Sections.n(); i++)
      {
        Section& CurrentSection = Sections[i];
        TotalLength += CurrentSection.Length;
        CurrentCharacter += CurrentSection.Characters;
        if(CurrentCharacter == endIndex - startIndex + 1)
        {
          EndingSection = i;
          break;
        }
      }

      ascii* WholeSubstring = new ascii[TotalLength + 1];
      
      ascii* CurrentPart = WholeSubstring;
      for(count i = BeginningSection; i <= EndingSection; i++)
      {
        Section& CurrentSection = Sections[i];
        Memory::CopyArray(CurrentPart, CurrentSection.Substring, 
          CurrentSection.Length);
        CurrentPart += CurrentSection.Length;
      }

      WholeSubstring[TotalLength] = 0;
      Destination.Sections.Add().Initialize(WholeSubstring, TotalLength);
      Destination.Length = TotalLength;
      Destination.Characters = endIndex - startIndex + 1;
    }

    /**Returns a subsection from a start and end index (inclusive). In general, 
    methods that return String are not as fast because they end up copying the 
    entire String over. Use the other form when performance is paramount.*/
    String Subsection(count startIndex, count endIndex) const
    {
      String ReturnValue;
      Subsection(ReturnValue, startIndex, endIndex);
      return ReturnValue;
    }

    ///Returns a certain number of characters at a particular index.
    void Substring(String& Destination, count startIndex, 
      count CharactersToTake) const
    {
      Substring(Destination, startIndex, startIndex + CharactersToTake - 1);
    }

    /**Returns a certain number of characters at a particular index. In general, 
    methods that return String are not as fast because they end up copying the 
    entire String over. Use the other form when performance is paramount.*/
    String Substring(count startIndex, count CharactersToTake) const
    {
      String ReturnValue;
      Subsection(ReturnValue, startIndex, startIndex + CharactersToTake - 1);
      return ReturnValue;
    }

    ///Returns a number of characters before a particular index (exclusive).
    void SubstringBefore(String& Destination, count startIndex, 
      count CharactersToTake) const
    {
      Subsection(Destination, startIndex - CharactersToTake, startIndex - 1);
    }

    /**Returns a number of characters before a particular index (exclusive). In 
    general, methods that return String are not as fast because they end up 
    copying the entire String over. Use the other form when performance is 
    paramount.*/
    String SubstringBefore(count startIndex, count CharactersToTake) const
    {
      String ReturnValue;
      Subsection(ReturnValue, startIndex - CharactersToTake, startIndex - 1);
      return ReturnValue;
    }

    ///Returns a number of characters from the start of the string.
    void Prefix(String& Destination, count CharactersToTake) const
    {
      Subsection(Destination, 0, CharactersToTake - 1);
    }

    /**Returns a number of characters from the start of the string. In general, 
    methods that return String are not as fast because they end up copying the 
    entire String over. Use the other form when performance is paramount.*/
    String Prefix(count CharactersToTake) const
    {
      String ReturnValue;
      Subsection(ReturnValue, 0, CharactersToTake - 1);
      return ReturnValue;
    }

    ///Returns a number of characters from the end of the string.
    void Suffix(String& Destination, count CharactersToTake) const
    {
      Subsection(Destination, Characters - CharactersToTake,
        Characters - 1);
    }

    /**Returns a number of characters from the end of the string. In general, 
    methods that return String are not as fast because they end up copying the 
    entire String over. Use the other form when performance is paramount.*/
    String Suffix(count CharactersToTake) const
    {
      String ReturnValue;
      Subsection(ReturnValue, Characters - CharactersToTake,
        Characters - 1);
      return ReturnValue;
    }

    //---------//
    //Insertion//
    //---------//

    /**\brief Inserts a substring by spliting a section in two and linking
    the new substring in between them.*/
    void Insert(const ascii* Substring, count beforeIndex)
    {
      _profile(primProfiles);

      count LeftIndex = Split(beforeIndex);

      Section BlankSection;
      Section* NewSection;

      if(LeftIndex==-1)
      {
        //Do a prepend.
        Sections.InsertBefore(BlankSection,0);
        NewSection = &Sections[0];
      }
      else
      {
        //Do an append or insert between two sections.
        Sections.InsertAfter(BlankSection,LeftIndex);
        NewSection = &Sections[LeftIndex+1];
      }

      NewSection->Initialize(Substring);
      Length += NewSection->Length;
      Characters += NewSection->Characters;

      _endprofile(primProfiles);
    }

    //----------------//
    //Find and Replace//
    //----------------//

    /**Looks for a substring at a given start index. If it finds the string, it
    returns true and indicates the index of the substring in the search string
    in IndexOfFoundInstance. If the substring is not found or there is an error
    in the input, then the value in IndexOfFoundInstance is left unchanged and
    the return value is false.*/
    bool Find(const ascii* FindString, count& IndexOfFoundInstance, 
      count StartIndex = 0)
    {
      //Make sure the pointer is valid.
      if(!FindString)
        return false;
      
      /*Determine the length of the find string in characters without the null-
      terminator.*/
      count FindStringLength = unicode::UTF8::Characters(FindString);
      
      //Exit if the find string is bigger than the search string.
      if(!FindStringLength || FindStringLength > Characters)
        return false;

      //Exit if the start index makes it impossible to find the string.
      if(StartIndex < 0 || StartIndex + FindStringLength > Characters)
        return false;

      //Look for the string.
      for(count i = StartIndex; i <= Characters - FindStringLength; i++)
      {
        const prim::ascii* Iterator = FindString;
        bool StringMatches = true;
        for(count j = 0; j < FindStringLength; j++)
        {
          unicode::UCS4 SearchCharacter = Character(i + j);
          unicode::UCS4 FindCharacter = unicode::UTF8::Decode(Iterator);
          if(SearchCharacter != FindCharacter)
          {
            StringMatches = false;
            break;
          }
        }
        if(StringMatches)
        {
          IndexOfFoundInstance = i;
          return true;
        }
      }

      return false;
    }

    ///Replaces all instances of the find string with the replace string.
    bool Replace(const prim::ascii* FindString, 
      const prim::ascii* ReplaceString)
    {
      if(!FindString || !ReplaceString)
        return false;
      
      count StartIndex = 0;
      count FindIndex = 0;
      count FindStringByteLength = Section::SubstringLength(FindString);
      count FindStringLength = unicode::UTF8::Characters(FindString);
      count ReplaceStringLength = unicode::UTF8::Characters(ReplaceString);

      bool ReplacedAtLeastOneInstance = false;

      while(Find(FindString, FindIndex, StartIndex))
      {
        //Excise the find string.
        count StartSection = Split(FindIndex) + 1;
        count EndSection = Split(FindIndex + FindStringLength) + 1;
        for(count i = EndSection - 1; i >= StartSection; i--)
          Sections.Remove(i);

        //Update the character count to reflect the removal.
        Characters -= FindStringLength;
        Length -= FindStringByteLength;

        //Insert the replace string.
        if(ReplaceStringLength)
        {
          Insert(ReplaceString, FindIndex);
          StartIndex = FindIndex + ReplaceStringLength;
          ReplacedAtLeastOneInstance = true;
        }
      }

      return ReplacedAtLeastOneInstance;
    }

    //---------//
    //Appending//
    //---------//

    /**\brief Appends a substring by creating a new section and linking it
    to the end of the string.*/
    void Append(const ascii* Substring)
    {
      _profile(primProfiles);

      //Prevent adding a null pointer or a null string.
      if(!Substring || *Substring == 0)
      {
        _endprofile(primProfiles);
        return;
      }

      Sections.Add();
      Section& NewSection = Sections.last();
      NewSection.Initialize(Substring);
      Length += NewSection.Length;
      Characters += NewSection.Characters;

      _endprofile(primProfiles);
    }

    /**\brief Appends a substring of a known byte-length, and creates a new
    section to link to the end of the string.*/
    void Append(const ascii* Substring, count Bytes)
    {
      _profile(primProfiles);

      //Prevent adding a null string.
      if(!Substring || *Substring == 0 || Bytes == 0)
      {
        _endprofile(primProfiles);
        return;
      }

      Sections.Add();
      Section& NewSection = Sections.last();

      ascii* NewSubstring = new ascii[Bytes + 1];
      Memory::CopyArray(NewSubstring,Substring,Bytes);
      NewSubstring[Bytes] = 0;

      NewSection.Initialize(NewSubstring,Bytes);
      Length += NewSection.Length;
      Characters += NewSection.Characters;

      _endprofile(primProfiles);
    }

    ///Appends an array of UCS4 codepoints.
    void Append(const unicode::UCS4* UCS4String)
    {
      _profile(primProfiles);

      //Prevent adding a null string.
      if(*UCS4String==0)
      {
        _endprofile(primProfiles);
        return;
      }

      //Allocate memory for the UTF-8 octets.
      count Octets = unicode::UTF8::Octets(UCS4String);
      ascii* OctetStream = new ascii[Octets+1];

      //Encode the UCS-4 characters into the new UTF-8 stream.
      unicode::UTF8::EncodeArray(OctetStream,UCS4String);

      //Add the UTF-8 data to the string.
      Sections.Add();
      count SectionIndex = Sections.Size() - 1;
      Section& NewSection = Sections[SectionIndex];
      NewSection.Initialize(OctetStream,Octets);
      Length += NewSection.Length;
      Characters += NewSection.Characters;

      _endprofile(primProfiles);
    }

    /**Overload for Append to add single UCS-4 characters. You can specify
    up to ten characters in one call.*/
    void Append(
      unicode::UCS4 Code1,
      unicode::UCS4 Code2=0,
      unicode::UCS4 Code3=0,
      unicode::UCS4 Code4=0,
      unicode::UCS4 Code5=0,
      unicode::UCS4 Code6=0,
      unicode::UCS4 Code7=0,
      unicode::UCS4 Code8=0,
      unicode::UCS4 Code9=0,
      unicode::UCS4 Code10=0)
    {
      /*Allocate a temporary array to store the UCS-4 characters as a
      contiguous string.*/
      unicode::UCS4* Codes = new unicode::UCS4[11];

      //Initialize the array with the values passed in the parameters.
      Codes[0]=Code1;
      Codes[1]=Code2;
      Codes[2]=Code3;
      Codes[3]=Code4;
      Codes[4]=Code5;
      Codes[5]=Code6;
      Codes[6]=Code7;
      Codes[7]=Code8;
      Codes[8]=Code9;
      Codes[9]=Code10;
      Codes[10]=0;

      //Now call the regular Append, with the array we just created.
      Append(Codes);

      //Delete the temporary array.
      delete [] Codes;
    }

    ///Appends a prim::integer to the string
    void AppendInteger(integer x, count LeadingZeroes=0, 
      bool UseSpacesInsteadOfZeroes=false)
    {
      _profile(primProfiles);

      //Bring the math namespace into scope.
      using namespace math;

      //Check for the simple zero case.
      if(!x)
      {
        Append("0");
        _endprofile(primProfiles);
        return;
      }

      //A static array for containing the digits
      const count MaximumDigits = 64;
      ascii Digits[MaximumDigits];

      //Clear the array with leading zeroes.
      if(UseSpacesInsteadOfZeroes)
        Memory::ClearArray(&Digits[0],MaximumDigits,' ');
      else
        Memory::ClearArray(&Digits[0],MaximumDigits,'0');

      count DigitCount=0;

      //Build the string by computing each digit sequentially.
      String Integer;

      //Make negative sign if necessary.
      if(x<0)
        Integer &= "-";

      //Precompute all of the digits.
      while(x!=0)
      {
        Digits[DigitCount]=(ascii)ModAbs(x,(integer)10) + (ascii)48;
        DigitCount++;
        x/=10;
      }

      //Account for leading zeroes if necessary.
      DigitCount = Min(Max(DigitCount, LeadingZeroes),MaximumDigits);

      //Append the digits in the opposite order (high to low).
      for(count i=DigitCount-1;i>=0;i--)
        Integer.Append(Digits[i]);

      //Append the integer string.
      Append(Integer.Merge());

      _endprofile(primProfiles);
    }

    ///Appends a prim::number to the string
    void AppendNumber(number x, count SignificantFigures = 5)
    {
      _profile(primProfiles);

      Append(math::NumberToString(x, SignificantFigures).Merge());

      _endprofile(primProfiles);
    }

    ///Appends a math::vector to the string.
    void AppendVector(math::Vector v)
    {
      _profile(primProfiles);

      String VectorString;
      VectorString &= "(";
      VectorString.AppendNumber(v.x);
      VectorString &= ", ";
      VectorString.AppendNumber(v.y);
      VectorString &= ")";
      Append(VectorString.Merge());

      _endprofile(primProfiles);
    }

    /*
    ///Appends a math::Ratio to the string.
    void AppendRatio(math::Ratio r)
    {
      String RatioString;
      RatioString = r.toPrettyString();
      Append(RatioString.Merge());
    }
    */

    //--------------//
    //Trimming Space//
    //--------------//
    void RemoveLeadingAndTrailingCharacters(unicode::UCS4 CharacterCode)
    {
      count Start = 0;
      for(count i = 0; i < Length; i++)
      {
        if(Character(i) == CharacterCode)
          Start++;
        else break;
      }

      count End = Length - 1;
      for(count i = Length - 1; i > 0; i--)
      {
        if(Character(i) == CharacterCode)
          End--;
        else break;
      }
      Trim(Start, End);
    }

    inline void TrimSpaces(void)
    {
      RemoveLeadingAndTrailingCharacters(32);
    }

    //------------//
    //Constructors//
    //------------//

    ///Creates an empty string.
    String() : Length(0), Characters(0) {}

    ///Initializes a string with a character array.
    String(const ascii* otherString) : Length(0), Characters(0)
    {
      Append(otherString);
    }

    ///Initializes a string with a character array.
    String(const ascii* otherString, count Bytes) : Length(0), Characters(0)
    {
      Append(otherString, Bytes);
    }

    ///Copy constructor copies another string into this one.
    String(const String& otherString) : Length(0), Characters(0)
    {
      _profile(primProfiles);

      /*Unfortunately, we can't collapse otherString using merge since
      this would violate the const indicator which is obligatory for a
      proper copy constructor.*/
      Length = otherString.Length;
      Characters = otherString.Characters;

      //Loop through each segment and copy it.
      ascii* WholeString = new ascii[Length + 1];
      ascii* WritePosition = WholeString;
      for(count i=0;i<otherString.Sections.n();i++)
      {
        Section& Segment = otherString.Sections[i];
        count BytesToCopy = Segment.Length;
        Memory::CopyArray(WritePosition,
          Segment.Substring,
          BytesToCopy);

        WritePosition += BytesToCopy;
      }

      //Add null-terminator.
      WholeString[Length]=0;

      //Add the full string to the section.
      Sections.Add();
      Section& NewSection = Sections.last();
      NewSection.Initialize(WholeString,Length);

      _endprofile(primProfiles);
    }

    ///Constructor turns an integer into a string.
    String(integer x) : Length(0), Characters(0)
    {
      AppendInteger(x);
    }

    ///Constructor turns a number into a string.
    String(number x) : Length(0), Characters(0)
    {
      AppendNumber(x);
    }

    ///Constructor turns a vector into a string.
    String(math::Vector v) : Length(0), Characters(0)
    {
      AppendVector(v);
    }

    //APPENDING OPERATORS//

    ///Appends a constant string.
    void operator &= (const ascii* otherString)
    {
      Append(otherString);
    }

    ///Appends a string.
    void operator &= (const String& otherString)
    {
      Append(otherString.Merge());
    }

    ///Appends an integer.
    void operator &= (integer x)
    {
      AppendInteger(x);
    }

    ///Appends an number.
    void operator &= (number x)
    {
      AppendNumber(x);
    }

    ///Appends a vector.
    void operator &= (math::Vector v)
    {
      AppendVector(v);
    }

    /*
    ///Appends a ratio.
    void operator &= (math::Ratio r)
    {
      AppendRatio(r);
    }
    */

    ///Appends a space to the string (character 32).
    void operator -- (int)
    {
      Append(32);
    }

    ///Appends a string after a space (character 32).
    void operator -= (const ascii* otherString)
    {
      if(Length)
        Append(32);
      Append(otherString);
    }

    ///Appends an integer after a space (character 32).
    void operator -= (integer x)
    {
      if(Length)
        Append(32);
      AppendInteger(x);
    }

    ///Appends a number after a space (character 32).
    void operator -= (number x)
    {
      if(Length)
        Append(32);
      AppendNumber(x);
    }

    ///Appends a vector after a space (character 32).
    void operator -= (math::Vector v)
    {
      if(Length)
        Append(32);
      AppendVector(v);
    }
    
    ///Appends a string after a space (character 32).
    void operator -= (const prim::String& s)
    {
      if(Length)
        Append(32);
      Append(s.Merge());
    }    

    ///Appends a new line to the string (character 10, Linux/Mac style).
    void operator ++ (int)
    {
      Append(10);
    }

    ///Appends a string after a new line (character 10, Linux/Mac style).
    void operator += (const ascii* otherString)
    {
      if(Length)
        Append(10);
      Append(otherString);
    }

    ///Appends an integer after a new line (character 10, Linux/Mac style).
    void operator += (integer x)
    {
      if(Length)
        Append(10);
      AppendInteger(x);
    }

    ///Appends a number after a new line (character 10, Linux/Mac style).
    void operator += (number x)
    {
      if(Length)
        Append(10);
      AppendNumber(x);
    }

    ///Appends a vector after a new line (character 10, Linux/Mac style).
    void operator += (math::Vector v)
    {
      if(Length)
        Append(10);
      AppendVector(v);
    }
    
    ///Appends a string after a new line (character 10, Linux/Mac style).
    void operator += (const prim::String& s)
    {
      if(Length)
        Append(10);
      Append(s.Merge());
    }      

    ///Gets a string containing the character at the 'ith' index position.
    String ith(count i) const
    {
      String ReturnValue;
      ReturnValue.Append(Character(i));
      return ReturnValue;
    }

    //----------//
    //Assignment//
    //----------//

    ///Assignment operator copies a character array to replace the string.
    void operator= (const ascii* otherString)
    {
      Clear();
      Append(otherString);
    }

    ///Assignment operator copies another string to replace this one.
    void operator= (const String& otherString)
    {
      Clear();
      Append(otherString.Merge());
    }

    ///Assignment operator copies an integer to replace the string.
    void operator= (integer x)
    {
      Clear();
      AppendInteger(x);
    }

    ///Assignment operator copies a number to replace the string.
    void operator= (number x)
    {
      Clear();
      AppendNumber(x);
    }

    ///Assignment operator copies a vector to replace the string.
    void operator= (math::Vector v)
    {
      Clear();
      AppendVector(v);
    }

    //-----------------//
    //Automatic Casting//
    //-----------------//

    ///Casts the String into an array of characters.
    operator const ascii* (void) const
    {
      return Merge();
    }

    /**Returns whether or not the string contains text. This operator can be 
    used in conditional statements such as if(ExampleString){...}.*/
    operator bool (void) const
    {
      return (n() > 0);
    }

    //----------//
    //Comparison//
    //----------//

    ///Compares this String an ascii string for binary equivalence.
    bool operator == (const ascii* otherString) const
    {
      _profile(primProfiles);

      const ascii* thisString = Merge();

      /*Check for case where this string has no characters, but other string
      does have characters.*/
      if(Length == 0 && *otherString != 0)
      {
        _endprofile(primProfiles);
        return false;
      }
      for(count i = 0; i < Length; i++)
      {
        if(*otherString==0 || *thisString != *otherString)
        {
          _endprofile(primProfiles);
          return false;
        }

        thisString++;
        otherString++;
      }
      _endprofile(primProfiles);
      return *otherString == 0;
    }

    ///Compares this String with another for binary equivalence.
    bool operator == (const String& otherString) const
    {
      //First do a simple length test, then actually look at the strings.
      if(otherString.Characters != Characters)
        return false;
      return (*this == (const ascii*)otherString.Merge());
    }

    ///Compares this String an ascii string for binary unequivalence.
    bool operator != (const ascii* otherString) const
    {
      return ! (*this == otherString);
    }

    ///Compares this String with another for binary unequivalence.
    bool operator != (const String& otherString) const
    {
      //First do a simple length test, then actually look at the strings.
      if(otherString.Characters != Characters)
        return true;
      return ! (*this == (const ascii*)otherString.Merge());
    }

    //------------------//
    //Numeric Conversion//
    //------------------//

    integer ToInteger(void) const
    {
      return math::StringToInteger(*this);
    }

    number ToNumber(void) const
    {
      return math::StringToNumber(*this);
    }

    operator number (void) const
    {
      return ToNumber();
    }

    operator integer (void) const
    {
      return ToInteger();
    }
  };
}

#endif
