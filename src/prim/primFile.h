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

#ifndef primFile
#define primFile

#include "primTypes.h"
#include "primString.h"
#include "primUnicode.h"

namespace prim
{
  class File
  {
  public:
    /**Reads a file into an array of prim::byte. You will have to delete the
    array when you are done with it by calling delete [] ByteArray. This
    method returns the number of bytes in the array.*/
    static count Read(const ascii* Filename, byte*& ByteArray);

    /**Writes a new file from an array of prim::byte. Returns whether or not the
    write was successful.*/
    static bool Write(const ascii* Filename, const byte* ByteArray, 
      count BytesToWrite);

    ///Reads the file as a UTF-8 string.
    static void ReadAsUTF8(const ascii* Filename, String& Destination)
    {
      byte* ByteArray = 0;
      count Bytes = Read(Filename, ByteArray);
      Destination.Clear();
      Destination.Append((const ascii*)ByteArray, Bytes);
      delete ByteArray;
    }

    ///Replaces a particular file with a string.
    static bool Replace(const ascii* Filename, const ascii* someString);

    ///Appends a string to a particular file.
    static bool Append(const ascii* Filename, const ascii* someString);
    
    ///Appends binary data to a particular file.
    static bool Append(const ascii* Filename, const byte* ByteArray, 
      count BytesToWrite);

    /**\brief Writes the string to an HTML file for easy viewing in the case
    of a UTF-8 encoding.*/
    static bool ToHTML(const ascii* Filename, const ascii* someString)
    {
      /*We will prepare the string by changing certain control codes
      such as new lines into HTML entities like <br/>*/
      String PreparedString;
      using namespace unicode;
      while(*someString!=0)
      {
        UCS4 d=UTF8::Decode(someString);
        switch(d)
        {
        case 10:
          PreparedString.Append("<br/>");
          break;
        case 32:
          PreparedString.Append("&nbsp;");
          break;
        case 34:
          PreparedString.Append("&quot;");
          break;
        case 39:
          PreparedString.Append("&apos;");
          break;
        case 38:
          PreparedString.Append("&amp;");
          break;
        case 60:
          PreparedString.Append("&lt;");
          break;
        case 62:
          PreparedString.Append("&gt;");
          break;
        default:
          PreparedString.Append(d);
        }
      }

      //Build the HTML file.
      String HTML;
      HTML &= "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 ";
      HTML &= "Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD";
      HTML &= "/xhtml1-strict.dtd\">\n\n";
      HTML &= "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
      HTML &= "\t<head>\n";
      HTML &= "\t\t<meta http-equiv=\"content-type\" ";
      HTML &= "content=\"text/html; charset=UTF-8\"/>\n";
      HTML &= "\t\t<title></title>\n";
      HTML &= "\t</head>\n";
      HTML &= "\t<body>\n";
      HTML &= "\t\t<p style=\"font : normal normal normal 12pt/14pt 'Courier New'\">";
      HTML &= PreparedString;
      HTML &= "</p>\n";
      HTML &= "\t</body>\n";
      HTML &= "</html>";
      return Replace(Filename, HTML);
    }
  };
}

#endif
