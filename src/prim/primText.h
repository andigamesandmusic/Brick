/*
  ==============================================================================

   This file is part of the Belle, Bonne, Sage library
   Copyright 2007-2010 by William Andrew Burnson

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

#ifndef primText
#define primText

namespace prim
{
  typedef prim::count Justification;
  struct Justifications
  {
    static const Justification Left = 0;
    static const Justification Center = 1;
    static const Justification Right = 2;
    static const Justification Full = 3;
  };

  struct Text
  {
    static void MakeASCIIPrintableString(prim::String& someString)
    {
      using namespace prim;
      const ascii* Merged = someString.Merge();
      ascii* AsciiCharacters = new ascii[someString.n()+1];

      for(count i=0;i<someString.n();i++)
      {
        unicode::UCS4 Character = unicode::UTF8::Decode(Merged);
        if(Character < 32 || Character > 126)
          Character = '?';
        AsciiCharacters[i] = (ascii)Character;
      }

      AsciiCharacters[someString.n()] = 0; //Null-terminator
      someString = AsciiCharacters;
      delete AsciiCharacters;
    }

    static String EncodeDataAsHexString(byte Data)
    {

      String HexValues;

      ascii ch1 = (ascii)((Data >> 4) & 15);
      ch1 < 10 ? ch1 += 48 : ch1 += 55;
      ascii ch2 = (ascii)(Data & 15);
      ch2 < 10 ? ch2 += 48 : ch2 += 55;

      HexValues.Append(ch1,ch2);

      return HexValues;
    }

    static void EncodeDataAsHexString(
      const byte* Data, count DataByteLength, String& HexString)
    {
      //Create temporary ASCII character string.
      ascii* HexCharacters = new ascii[DataByteLength*2+1];

      //Convert the bytes into hex.
      count c=0;
      for(count b=0;b<DataByteLength;b++)
      {
        ascii ch1 = (ascii)((Data[b] >> 4) & 15);
        ch1 < 10 ? ch1 += 48 : ch1 += 55;
        ascii ch2 = (ascii)(Data[b] & 15);
        ch2 < 10 ? ch2 += 48 : ch2 += 55;
        HexCharacters[c]     = ch1;
        HexCharacters[c + 1] = ch2;
        c+=2;
      }

      //Add null-terminator.
      HexCharacters[DataByteLength*2]=0;

      //Copy to string.
      HexString = HexCharacters;

      //Free temporary memory.
      delete HexCharacters;
    }
  };
}

#endif
