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

#ifndef primUnicode
#define primUnicode

#include "primTypes.h"

namespace prim
{
  /**The unicode namespace defines encodings and Unicode codepoints. It is
  designed as namespace subsets instead of static classes to permit the use of
  the 'using namespace' ... keyword to quickly access a character set. For
  example, if you had a String called str, then you could do the following:

  { //Scoping block using namespace unicode::cyrillic; using namespace
  unicode::latin::punctuation;
  str.Append(LeftDoubleAngleQuote,P,r,a,v,d,a,RightDoubleAngleQuote);
  str.Append(' ',o,n,' '); str.Append(s,k,a,z,a,l,'.'); }

  The letters that are passed to String::Append are actually constants that
  have been scoped locally from the cyrillic namespace. Also notice that it is
  possible to use single quotes to access the standard ASCII characters. This
  allows one to work in whichever script is convenient without having to
  provide scope resolution operators for each constant.

  Also the unicode namespace defines the UTF-8 class which is used for
  encoding and decoding UTF-8 from Unicode UCS-4 codepoints.*/
  namespace unicode
  {
    /**\brief A four-byte character fixed width encoding suitable for
    carrying any Unicode character in the BMP or SMPs.*/
    typedef uint32 UCS4;

    /**\brief A class that defines the encoding and decoding algorithms
    necessary to convert UCS4 (4 byte Unicode codepoints) into the more
    compact UTF-8 encoding.*/
    struct UTF8
    {
      ///An 8-bit unit of which UTF-8 characters are comprised.
      typedef uint8 octet;

      /**\brief Computes the number of octets in a character given the
      first octet.*/
      static count Octets(octet FirstOctet)
      {
        if(FirstOctet<0x80)
          return 1;
        else if((FirstOctet & 0xFB) == 0xFB)
          return 6;
        else if((FirstOctet & 0xF8) == 0xF8)
          return 5;
        else if((FirstOctet & 0xF0) == 0xF0)
          return 4;
        else if((FirstOctet & 0xE0) == 0xE0)
          return 3;
        else if((FirstOctet & 0xC0) == 0xC0)
          return 2;
        else
          return 1; //If information is corrupt at least move ahead.
      }

      /**\brief Computes the number of octets (bytes) required to store a
      string of Unicode characters in UTF-8.*/
      static count Octets(const UCS4* UCS4String)
      {
        count OctetCount=0;
        UCS4 NextCharacter=*UCS4String;
        while(NextCharacter!=0)
        {
          if(NextCharacter < 0x80 || NextCharacter > 0x10FFFF)
            OctetCount+=1; //1 octet
          else if(NextCharacter < 0x800)
            OctetCount+=2; //2 octets
          else if(NextCharacter < 0x10000)
            OctetCount+=3; //3 octets
          else
            OctetCount+=4; //4 octets

          UCS4String++;
          NextCharacter=*UCS4String;
        }
        return OctetCount;
      }

      ///Computes the number of characters in a UTF-8 string.
      static count Characters(const ascii* UTF8String)
      {
        count CharacterCount=0;
        ascii NextCharacter=*UTF8String;

        while(NextCharacter!=0)
        {
          UTF8String+=Octets((octet)NextCharacter);
          CharacterCount++;
          NextCharacter=*UTF8String;
        }

        return CharacterCount;
      }

      /**\brief Encodes a UCS-4 codepoint as a UTF-8 character in a string
      of ascii characters. \details The pointer to the string is
      incremented automatically, and the number of bytes it increments is
      returned by the method.*/
      static count Encode(UCS4 Codepoint, ascii*& UTF8String)
      {
        //See: http://tools.ietf.org/html/rfc3629#section-3
        octet*& e = (octet*&)UTF8String;
        UCS4 d=Codepoint;

        //Protect against bad codepoints.
        if(d > 0x10FFFF)
          d=63; //'?' character

        //Encode the codepoint into some number of octets
        if(d < 0x80) //1 octet
        {
          *e=(UTF8::octet)d;
          e++;
          return 1;
        }
        else if(d < 0x800) //2 octets
        {
          *e = ( (d >> 6) & 0x1F) + 0xC0; //110xxxxx
          e++;
          *e = (d & 0x3F) + 0x80; //10xxxxxx
          e++;
          return 2;
        }
        else if(d < 0x10000) //3 octets
        {
          *e = ( (d >> 12) & 0x0F) + 0xE0; //1110xxxx
          e++;
          *e = ( (d >> 6) & 0x3F) + 0x80; //10xxxxxx
          e++;
          *e = (d & 0x3F) + 0x80; //10xxxxxx
          e++;
          return 3;
        }
        else //4 octets
        {
          *e = ( (d >> 18) & 0x07) + 0xF0; //11110xxx
          e++;
          *e = ( (d >> 12) & 0x3F) + 0x80; //10xxxxxx
          e++;
          *e = ( (d >> 6) & 0x3F) + 0x80; //10xxxxxx
          e++;
          *e = (d & 0x3F) + 0x80; //10xxxxxx
          e++;
          return 4;
        }
      }

      /**\brief Encodes an array of UCS-4 codepoints into a pre-allocated
      UTF-8 buffer. \details You can use the Octets method to determine
      how large the buffer will need to be.*/
      static void EncodeArray(ascii* Destination, const UCS4* Source)
      {
        while(*Source!=0)
        {
          Encode(*Source,Destination);
          Source++;
        }
        *Destination=0;
      }

      /**Decodes the next character pointed to in a pointer to a string.
      The pointer passed in is automatically incremented so long as the
      null terminator is not reached.*/
      static UCS4 Decode(const ascii*& UTF8String)
      {
        //See: http://tools.ietf.org/html/rfc3629#section-3
        octet*& e = (octet*&)UTF8String;

        //Compute the number of octets in this character.
        count NumberOfOctets=Octets(*e);

        //Protect against bad octet values.
        for(count i=0;i<NumberOfOctets;i++)
        {
          octet v = e[i];
          if(v == 0xC0 || v == 0xC1 || v >= 0xF5)
            return 63; //'?' character
        }

        //Decode the character.
        UCS4 d=0;

        if(NumberOfOctets==1)
          d=e[0];
        else if(NumberOfOctets==2)
          d = (e[1] & 0x3F) + ((e[0] & 0x1F) << 6);
        else if(NumberOfOctets==3)
          d = (e[2] & 0x3F) + ((e[1] & 0x3F) << 6)
          + ((e[0] & 0x0F) << 12);
        else
          d = (e[3] & 0x3F) + ((e[2] & 0x3F) << 6)
          + ((e[1] & 0x3F) << 12) + ((e[0] & 0x07) << 18);

        //Check for overly long sequences
        if((NumberOfOctets==2 && d < 0x80)
          || (NumberOfOctets==3 && d < 0x800)
          || (NumberOfOctets==4 && d < 0x10000))
          d = 63; //'?' character

        //Increment the pointer so long as we have reached the end.
        if(d!=0)
          e+=NumberOfOctets;

        //Return the decoded character.
        return d;
      }

      /**\brief Decodes a UTF-8 string into an array of pre-allocated
      UCS-4 words. \details You can use the Characters method to determine
      how many characters need to be allocated in the UCS-4 array.*/
      static void DecodeArray(UCS4* Destination, const ascii* Source)
      {
        while(*Source!=0)
        {
          *Destination=Decode(Source);
          Destination++;
        }
        *Destination = 0;
      }
    };

    //--------------------------//
    //Character Code Definitions//
    //--------------------------//

    //References: http://tlt.psu.edu/suggestions/international/index.html

    ///Extended Latin-1 Characters
    namespace latin
    {

      ///Latin-1 Symbols
      namespace symbols
      {
        static const UCS4 CentSign = 0xA2;
        static const UCS4 PoundSign = 0xA3;
        static const UCS4 CurrencySign = 0xA4;
        static const UCS4 YenSign = 0xA5;
        static const UCS4 CopyrightSign = 0xA9;
        static const UCS4 RegisteredSign = 0xAE;
        static const UCS4 BrokenBar = 0xA6;
        static const UCS4 SectionSign = 0xA7;
        static const UCS4 ParagraphSign = 0xB6;
        static const UCS4 MiddleDot = 0xB7;
        static const UCS4 Macron = 0xAF;
        static const UCS4 FractionalQuarter = 0xBC;
        static const UCS4 FractionalHalf = 0xBD;
        static const UCS4 FractionalThreeQuarters = 0xBE;
        static const UCS4 DegreeSign = 0xB0;
        static const UCS4 PlusMinus = 0xB1;
        static const UCS4 MultiplicationSign = 0xD7;
        static const UCS4 DivisionSign = 0xF7;
        static const UCS4 MicroSign = 0xB5;
        static const UCS4 NotSign = 0xAC;
        static const UCS4 SuperscriptOne = 0xB9;
        static const UCS4 SuperscriptTwo = 0xB2;
        static const UCS4 SuperscriptThree = 0xB3;
        static const UCS4 MasculineOrdinalIndicator = 0xBA;
        static const UCS4 FeminineOrdinalIndicator = 0xAA;
      }

      ///Latin-1 Extended Punctuation
      namespace punctuation
      {
        static const UCS4 LeftDoubleAngleQuote = 0xAB;
        static const UCS4 RightDoubleAngleQuote = 0xBB;
        static const UCS4 InvertedQuestionMark = 0xBF;
        static const UCS4 InvertedExclamation = 0xA1;
      }

      ///Latin-1 Letters with Diacritics
      namespace diacritics
      {
        static const UCS4 Umlaut = 0xA8;
        static const UCS4 AcuteAccent = 0xB4;
        static const UCS4 Cedilla = 0xB8;
        static const UCS4 Thorn = 0xDE;
        static const UCS4 ThornSmall = 0xFE;
        static const UCS4 A_Grave = 0xC0;
        static const UCS4 A_Acute = 0xC1;
        static const UCS4 A_Circumflex = 0xC2;
        static const UCS4 A_Tilde = 0xC3;
        static const UCS4 A_Umlaut = 0xC4;
        static const UCS4 A_Ring = 0xC5;
        static const UCS4 AE_Ligature = 0xC6;
        static const UCS4 C_Cedilla = 0xC7;
        static const UCS4 E_Grave = 0xC8;
        static const UCS4 E_Acute = 0xC9;
        static const UCS4 E_Circumflex = 0xCA;
        static const UCS4 E_Umlaut = 0xCB;
        static const UCS4 I_Grave = 0xCC;
        static const UCS4 I_Acute = 0xCD;
        static const UCS4 I_Circumflex = 0xCE;
        static const UCS4 I_Umlaut = 0xCF;
        static const UCS4 Eth = 0xD0;
        static const UCS4 N_Tilde = 0xD1;
        static const UCS4 O_Grave = 0xD2;
        static const UCS4 O_Acute = 0xD3;
        static const UCS4 O_Circumflex = 0xD4;
        static const UCS4 O_Tilde = 0xD5;
        static const UCS4 O_Umlaut = 0xD6;
        static const UCS4 O_Slash = 0xD8;
        static const UCS4 U_Grave = 0xD9;
        static const UCS4 U_Acute = 0xDA;
        static const UCS4 U_Circumflex = 0xDB;
        static const UCS4 U_Umlaut = 0xDC;
        static const UCS4 Y_Acute = 0xDD;
        static const UCS4 Szlig = 0xDF;
        static const UCS4 a_Grave = 0xE0;
        static const UCS4 a_Acute = 0xE1;
        static const UCS4 a_Circumflex = 0xE2;
        static const UCS4 a_Tilde = 0xE3;
        static const UCS4 a_Umlaut = 0xE4;
        static const UCS4 a_Ring = 0xE5;
        static const UCS4 ae_Ligature = 0xE6;
        static const UCS4 c_Cedilla = 0xE7;
        static const UCS4 e_Grave = 0xE8;
        static const UCS4 e_Acute = 0xE9;
        static const UCS4 e_Circumflex = 0xEA;
        static const UCS4 e_Umlaut = 0xEB;
        static const UCS4 i_Grave = 0xEC;
        static const UCS4 i_Acute = 0xED;
        static const UCS4 i_Circumflex = 0xEE;
        static const UCS4 i_Umlaut = 0xEF;
        static const UCS4 eth = 0xF0;
        static const UCS4 n_Tilde = 0xF1;
        static const UCS4 o_Grave = 0xF2;
        static const UCS4 o_Acute = 0xF3;
        static const UCS4 o_Circumflex = 0xF4;
        static const UCS4 o_Tilde = 0xF5;
        static const UCS4 o_Umlaut = 0xF6;
        static const UCS4 o_Slash = 0xF8;
        static const UCS4 u_Grave = 0xF9;
        static const UCS4 u_Acute = 0xFA;
        static const UCS4 u_Circumflex = 0xFB;
        static const UCS4 u_Umlaut = 0xFC;
        static const UCS4 y_Acute = 0xFD;
        static const UCS4 y_Umlaut = 0xFF;
      }
    }

    ///Cyrillic Characters
    namespace cyrillic
    {
      static const UCS4 A = 0x0410;
      static const UCS4 B = 0x0411;
      static const UCS4 V = 0x0412;
      static const UCS4 G = 0x0413;
      static const UCS4 D = 0x0414;
      static const UCS4 E = 0x0415;
      static const UCS4 Zh = 0x0416;
      static const UCS4 Z = 0x0417;
      static const UCS4 I = 0x0418;
      static const UCS4 Jot = 0x0419;
      static const UCS4 K = 0x041A;
      static const UCS4 L = 0x041B;
      static const UCS4 M = 0x041C;
      static const UCS4 N = 0x041D;
      static const UCS4 O = 0x041E;
      static const UCS4 P = 0x041F;
      static const UCS4 R = 0x0420;
      static const UCS4 S = 0x0421;
      static const UCS4 T = 0x0422;
      static const UCS4 U = 0x0423;
      static const UCS4 F = 0x0424;
      static const UCS4 X = 0x0425;
      static const UCS4 Ts = 0x0426;
      static const UCS4 Ch = 0x0427;
      static const UCS4 Sh = 0x0428;
      static const UCS4 Shh = 0x0429;
      static const UCS4 HardSign = 0x042A;
      static const UCS4 Ui = 0x042B;
      static const UCS4 SoftSign = 0x042C;
      static const UCS4 Eh = 0x042D;
      static const UCS4 Yu = 0x042E;
      static const UCS4 Ya = 0x042F;
      static const UCS4 a = 0x0430;
      static const UCS4 b = 0x0431;
      static const UCS4 v = 0x0432;
      static const UCS4 g = 0x0433;
      static const UCS4 d = 0x0434;
      static const UCS4 e = 0x0435;
      static const UCS4 zh = 0x0436;
      static const UCS4 z = 0x0437;
      static const UCS4 i = 0x0438;
      static const UCS4 jot = 0x0439;
      static const UCS4 k = 0x043A;
      static const UCS4 l = 0x043B;
      static const UCS4 m = 0x043C;
      static const UCS4 n = 0x043D;
      static const UCS4 o = 0x043E;
      static const UCS4 p = 0x043F;
      static const UCS4 r = 0x0440;
      static const UCS4 s = 0x0441;
      static const UCS4 t = 0x0442;
      static const UCS4 u = 0x0443;
      static const UCS4 f = 0x0444;
      static const UCS4 x = 0x0445;
      static const UCS4 ts = 0x0446;
      static const UCS4 ch = 0x0447;
      static const UCS4 sh = 0x0448;
      static const UCS4 shh = 0x0449;
      static const UCS4 HardSignSmall = 0x044A;
      static const UCS4 ui = 0x044B;
      static const UCS4 SoftSignSmall = 0x044C;
      static const UCS4 eh = 0x044D;
      static const UCS4 yu = 0x044E;
      static const UCS4 ya = 0x044F;
    }
  }
}

#endif
