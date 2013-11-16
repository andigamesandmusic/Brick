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

#ifndef primTypeface
#define primTypeface

#include "primTypes.h"
#include "primMemory.h"
#include "primString.h"
#include "primEndian.h"

namespace prim
{
  namespace typeface
  {
    ///Abstract base class to describe font program languages
    class DescriptionLanguage
    {
    public:
      /**\brief Returns a pointer to the specific description language
      that the instance refers to. \details You do conditional logic using
      the following: if(DescriptionLanguage* TT =
      Language->Interface<TrueType>()) { TT->FamilyName(); // TrueType
      specific methods called here. } */
      template <typename LanguageName> LanguageName* Interface(void)
      {
        return dynamic_cast<LanguageName*>(this);
      }

      ///Default constructor
      DescriptionLanguage(){}

      ///Default destructor
      virtual ~DescriptionLanguage(){}
    };

    ///Unimplemented class for reading PostScript Type 1 fonts
    class PostScript : public DescriptionLanguage
    {
    public:
      PostScript(){}
      virtual ~PostScript(){}
    };

    /**Class for reading information encoded in a TrueType font program. Not
    for the weak of heart! Luc Devroye once said, "If you are not a computer
    scientist with at least a B average from MIT, you can forget about
    understanding TrueType specs." (Haralambous, 880) A bit vainglorious and
    absurd, wouldn't you say? I don't even have a computer science degree!
    Microsoft's specifications are here: 
    http://www.microsoft.com/typography/otspec/default.htm

    And they are actually quite good, though very deep.

    Also the Haralambous book on Fonts and Encodings is an excellent
    reference, and contains two appendices dedicated to TrueType.*/
    class TrueType : public DescriptionLanguage
    {
    public:
      //-----------------//
      //HELPER STRUCTURES//
      //-----------------//

      ///TrueType tag that stores a four-letter string.
      struct Tag
      {
        ascii FourLetterCode[4];

        ///Returns the name of the tag.
        String Name(void)
        {
          //Turn on the unicode namespace for the UCS-4 up-conversion.
          using namespace unicode;

          //Create the string from the four characters.
          String NameString;
          NameString.Append((UCS4)FourLetterCode[0],
                    (UCS4)FourLetterCode[1],
                    (UCS4)FourLetterCode[2],
                    (UCS4)FourLetterCode[3]);

          return NameString;
        }

        ///Constructor zeroes out the structure.
        Tag(){Memory::ClearObject(*this);}
      };

      ///Stores a reference to a table somewhere in the TrueType program.
      struct Table
      {
        Tag NameTag;
        uint32 CheckSum;
        uint32 ActualSum;
        uint32 Offset;
        uint32 Length;
        bool Valid;

        ///Constructor zeroes out the structure.
        Table(){Memory::ClearObject(*this);}
      };

      /**\brief Stores the header information found in the HEAD table of
      the TrueType program.*/
      struct Header
      {
        uint32 VersionNumber;
        uint32 FontRevision;
        uint32 CheckSumAdjustment;
        uint32 MagicNumber;
        uint16 Flags;
        uint16 UnitsPerEm;
        uint32 DateCreatedHigh;
        uint32 DateCreatedLow;
        uint32 DateModifiedHigh;
        uint32 DateModifiedLow;
        int16  xMin;
        int16  yMin;
        int16  xMax;
        int16  yMax;
        uint16 MacStyle;
        uint16 LowestRecPPEM;
        int16  FontDirectionHint;
        int16  IndexToLocFormat;
        int16  GlyphDataFormat;

        ///Constructor zeroes out the structure.
        Header(){Memory::ClearObject(*this);}
      };

      /**\brief Stores the horizontal header information found in the HHEA
      table of the TrueType program.*/
      struct HorizontalHeader
      {
        uint32 VersionNumber;
        int16  Ascender;
        int16  Descender;
        int16  LineGap;
        uint16 AdvanceWidthMax;
        int16  MinLeftSideBearing;
        int16  minRightSideBearing;
        int16  xMaxExtent;
        int16  CaretSlopeRise;
        int16  CaretSlopeRun;
        int16  CaretOffset;
        int16  Reserved1;
        int16  Reserved2;
        int16  Reserved3;
        int16  Reserved4;
        int16  MetricDataFormat;
        uint16 NumberOfHMetrics;

        ///Constructor zeroes out the structure.
        HorizontalHeader(){Memory::ClearObject(*this);}
      };

      /**\brief Stores the OS/2 information found in the OS/2 table of the
      TrueType program.*/
      struct OS_2
      {
        uint16 VersionNumber;
        int16  xAvgCharWidth;
        uint16 WeightClass;
        uint16 WidthClass ;
        uint16 Type;
        int16  SubscriptXSize;
        int16  SubscriptYSize;
        int16  SubscriptXOffset;
        int16  SubscriptYOffset;
        int16  SuperscriptXSize;
        int16  SuperscriptYSize;
        int16  SuperscriptXOffset;
        int16  SuperscriptYOffset;
        int16  StrikeoutSize;
        int16  StrikeoutPosition;
        int16  FamilyClass;
        uint8  PANOSE[10];
        uint32 UnicodeRange1;
        uint32 UnicodeRange2;
        uint32 UnicodeRange3;
        uint32 UnicodeRange4;
        Tag    VendorID;
        uint16 Selection;
        uint16 FirstCharIndex;
        uint16 LastCharIndex;
        int16  TypoAscender;
        int16  TypoDescender;
        int16  TypoLineGap;
        uint16 WinAscent;
        uint16 WinDescent;
        uint32 CodePageRange1;
        uint32 CodePageRange2;
        int16  Height;
        int16  CapHeight;
        int16  DefaultChar;
        int16  BreakChar;
        int16  MaxContext;

        ///Constructor zeroes out the structure.
        OS_2(){Memory::ClearObject(*this);}
      };

      /**\brief Stores the PostScript information found in the POST table
      of the TrueType program.*/
      struct PostScript
      {
        uint32 VersionNumber;
        int16  ItalicAngleInteger;
        uint16 ItalicAngleDecimal;
        int16  UnderlinePosition;
        int16  UnderlineThickness;
        uint32 isFixedPitch;
        uint32 MinMemType42;
        uint32 MaxMemType42;
        uint32 MinMemType1;
        uint32 MaxMemType1;

        ///Constructor zeroes out the structure.
        PostScript(){Memory::ClearObject(*this);}
      };

      /**\brief Stores the horizontal matrix information found in the HMTX
      table of the TrueType program.*/
      struct HorizontalMatrix
      {
        uint16  NumberOfHMetrics;
        uint16* AdvanceWidths;
        int16*  LeftSideBearings;

        ///Constructor zeroes out the structure.
        HorizontalMatrix(){Memory::ClearObject(*this);}

        ///Destructor frees any memory associated with the pointers.
        ~HorizontalMatrix()
        {
          delete [] AdvanceWidths;
          delete [] LeftSideBearings;
        }
      };

      /**\brief Stores the kerning pairs found in the KERN table of the
      TrueType program.*/
      struct KerningPairs
      {
        uint16  VersionNumber;
        uint16  NumTables;
        uint16  NumKerningPairs;
        uint16* GlyphLeft;
        uint16* GlyphRight;
        int16*  KernValue;

        ///Constructor zeroes out the structure.
        KerningPairs(){Memory::ClearObject(*this);}

        ///Destructor frees any memory associated with the pointers.
        ~KerningPairs()
        {
          delete [] GlyphLeft;
          delete [] GlyphRight;
          delete [] KernValue;
        }

        /**Finds a kerning pair if it exists. Returns zero if
        non-existant and returns the kerning value if it does exist.*/
        int16 FindKerningPair(uint16 LeftGlyph, uint16 RightGlyph)
        {
          for(count i=0;i<NumKerningPairs;i++)
          {
            if(GlyphLeft[i]==LeftGlyph && GlyphRight[i]==RightGlyph)
              return KernValue[i];
          }
          return 0;
        }
      };

      /**\brief Stores the character map found in the CMAP table of the
      TrueType program.*/
      struct CharacterMap
      {
        uint16  VersionNumber;
        uint16  NumTables;
        uint16  SegmentCount;
        uint16* EndCharacterCode;
        uint16* StartCharacterCode;
        int16*  DeltaID;
        uint16* RangeOffsetID;
        uint32  NumGlyphIndices;
        uint16* GlyphIndexArray;

        /**\brief Looks up a 16-bit UCS-2 Unicode codepoint in the
        character map, and returns an index to the associated glyph.
        \details Returns zero if a glyph does not exist for that
        codepoint.*/
        uint16 GlyphIndexFromCharacterCode(uint16 Codepoint)
        {
          count Segment=-1;

          if(!EndCharacterCode)
            return 0;

          for(count i=0;i<SegmentCount;i++)
          {
            if(Codepoint <= EndCharacterCode[i])
            {
              Segment=i;
              break;
            }
          }

          if(Segment<0)
            return 0;

          if(StartCharacterCode[Segment]<=Codepoint)
          {
            if(RangeOffsetID[Segment]==0)
              return DeltaID[Segment] + Codepoint;
            else
            {
              int32 innerOffset =
                Codepoint - StartCharacterCode[Segment];
              int32 rangeOffset =
                RangeOffsetID[Segment] / 2;
              int32 wordsInRangeLeft =
                (int32)SegmentCount - (int32)Segment;
              return GlyphIndexArray[
                innerOffset + rangeOffset - wordsInRangeLeft];
            }
          }
          else
            return 0;
        }

        ///Constructor zeroes out the structure.
        CharacterMap(){Memory::ClearObject(*this);}

        ///Destructor frees any memory associated with the pointers.
        ~CharacterMap()
        {
          delete [] EndCharacterCode;
          delete [] StartCharacterCode;
          delete [] DeltaID;
          delete [] RangeOffsetID;
          delete [] GlyphIndexArray;
        }
      };


      //-------------------------//
      //TRUETYPE FONT INFORMATION//
      //-------------------------//
    public:
      //Initial font program header information
      uint32 Length;
      uint32 SFNTVersion;
      uint16 NumTables;
      uint16 SearchRange;
      uint16 EntrySelector;
      uint16 RangeShift;

      //Pointer to an array of table references
      Table* Tables;

      //Cached tables
      Header        HEAD;
      HorizontalHeader  HHEA;
      OS_2        OS2;
      PostScript      POST;
      HorizontalMatrix  HMTX;
      KerningPairs    KERN;
      CharacterMap    CMAP;

      bool Valid;

    protected:
      const byte* Program;
      const byte* SeekPosition;

      //----------------//
      //TRUETYPE METHODS//
      //----------------//
    public:
      //------------//
      //READ HELPERS//
      //------------//
      void Seek(count newSeekingPosition)
      {
        SeekPosition=&Program[newSeekingPosition];
      }

      //------------//
      //READ METHODS//
      //------------//

      ///A universal data type reader which also respects endianness
      template <class BasicDataType>
      void Read(BasicDataType& Object)
      {
        //Read the raw value by doing a forced type conversion.
        Object=*((BasicDataType*)SeekPosition);

        //TrueType files are written in big-endian, so here we
        //remanifest the value in its big-endian form. (Thankfully, the
        //process is automatic with prim's run-time endian checking!)
        Endian::ConvertObjectToBigEndian<BasicDataType>(Object);

        //Increment the seeker.
        SeekPosition+=sizeof(BasicDataType);
      }

      ///Reads a Tag from the current seeking position in the program.
      void Read(Tag& someTag)
      {
        for(count i=0;i<4;i++)
        {
          someTag.FourLetterCode[i]=(ascii)*SeekPosition;
          SeekPosition++;
        }
      }

      ///Reads in a Table reference and validates the checksum.
      void Read(Table& someTable)
      {
        Read(someTable.NameTag);
        Read(someTable.CheckSum);
        Read(someTable.Offset);
        Read(someTable.Length);

        if(someTable.Length + someTable.Offset > Length)
        {
          someTable.ActualSum = 0;
          someTable.Valid = false;
          return;
        }

        String FourLetterCode = someTable.NameTag.Name();

        uint32 Sum=0;
        uint32 Units = ((someTable.Length + 3) & ~3) / 4;
        uint32* ptrToData = (uint32*)
          & Program[someTable.Offset];
        uint32* endPtrToData=ptrToData+Units;
        while(ptrToData<endPtrToData)
        {
          uint32 Value = *ptrToData;
          Endian::ConvertObjectToBigEndian(Value);
          Sum += Value;
          ptrToData++;
        }
        someTable.ActualSum=Sum;

        //Special handling for the 'head' table
        if(someTable.NameTag.Name()=="head")
        {
          //Save the current seek position.
          const byte* OldSeekPosition = SeekPosition;

          //Look up the checksum adjustment and subtract it out.
          Seek(someTable.Offset + 8);
          uint32 CheckSumAdjustment=0;
          Read(CheckSumAdjustment);
          someTable.ActualSum-=CheckSumAdjustment;

          //Restore the old seek position.
          SeekPosition = OldSeekPosition;
        }

        someTable.Valid = (someTable.ActualSum == someTable.CheckSum);
      }

      ///Reads the Header table from the font program.
      void Read(Header& tableHEAD)
      {
        if(!Tables)
          return;

        for(count i=0;i<NumTables;i++)
        {
          if(Tables[i].NameTag.Name()=="head")
          {
            Seek(Tables[i].Offset);
            break;
          }
          else if(i==NumTables-1)
            return;
        }

        Read(tableHEAD.VersionNumber);
        Read(tableHEAD.FontRevision);
        Read(tableHEAD.CheckSumAdjustment);
        Read(tableHEAD.MagicNumber);
        Read(tableHEAD.Flags);
        Read(tableHEAD.UnitsPerEm);
        Read(tableHEAD.DateCreatedHigh);
        Read(tableHEAD.DateCreatedLow);
        Read(tableHEAD.DateModifiedHigh);
        Read(tableHEAD.DateModifiedLow);
        Read(tableHEAD.xMin);
        Read(tableHEAD.yMin);
        Read(tableHEAD.xMax);
        Read(tableHEAD.yMax);
        Read(tableHEAD.MacStyle);
        Read(tableHEAD.LowestRecPPEM);
        Read(tableHEAD.FontDirectionHint);
        Read(tableHEAD.IndexToLocFormat);
        Read(tableHEAD.GlyphDataFormat);
      }

      ///Reads the HorizontalHeader table from the font program.
      void Read(HorizontalHeader& tableHHEA)
      {
        if(!Tables)
          return;

        for(count i=0;i<NumTables;i++)
        {
          if(Tables[i].NameTag.Name()=="hhea")
          {
            Seek(Tables[i].Offset);
            break;
          }
          else if(i==NumTables-1)
            return;
        }

        Read(tableHHEA.VersionNumber);
        Read(tableHHEA.Ascender);
        Read(tableHHEA.Descender);
        Read(tableHHEA.LineGap);
        Read(tableHHEA.AdvanceWidthMax);
        Read(tableHHEA.MinLeftSideBearing);
        Read(tableHHEA.minRightSideBearing);
        Read(tableHHEA.xMaxExtent);
        Read(tableHHEA.CaretSlopeRise);
        Read(tableHHEA.CaretSlopeRun);
        Read(tableHHEA.CaretOffset);
        Read(tableHHEA.Reserved1);
        Read(tableHHEA.Reserved2);
        Read(tableHHEA.Reserved3);
        Read(tableHHEA.Reserved4);
        Read(tableHHEA.MetricDataFormat);
        Read(tableHHEA.NumberOfHMetrics);
      }

      ///Reads the OS_2 (OS/2) table from the font program.
      void Read(OS_2& tableOS2)
      {
        if(!Tables)
          return;

        for(count i=0;i<NumTables;i++)
        {
          if(Tables[i].NameTag.Name()=="OS/2")
          {
            Seek(Tables[i].Offset);
            break;
          }
          else if(i==NumTables-1)
            return;
        }

        Read(tableOS2.VersionNumber);
        Read(tableOS2.xAvgCharWidth);
        Read(tableOS2.WeightClass);
        Read(tableOS2.WidthClass);
        Read(tableOS2.Type);
        Read(tableOS2.SubscriptXSize);
        Read(tableOS2.SubscriptYSize);
        Read(tableOS2.SubscriptXOffset);
        Read(tableOS2.SubscriptYOffset);
        Read(tableOS2.SuperscriptXSize);
        Read(tableOS2.SuperscriptYSize);
        Read(tableOS2.SuperscriptXOffset);
        Read(tableOS2.SuperscriptYOffset);
        Read(tableOS2.StrikeoutSize);
        Read(tableOS2.StrikeoutPosition);
        Read(tableOS2.FamilyClass);
        for(count i=0;i<10;i++)
          Read(tableOS2.PANOSE[i]);
        Read(tableOS2.UnicodeRange1);
        Read(tableOS2.UnicodeRange2);
        Read(tableOS2.UnicodeRange3);
        Read(tableOS2.UnicodeRange4);
        for(count i=0;i<4;i++)
          Read(tableOS2.VendorID.FourLetterCode[i]);
        Read(tableOS2.Selection);
        Read(tableOS2.FirstCharIndex);
        Read(tableOS2.LastCharIndex);
        Read(tableOS2.TypoAscender);
        Read(tableOS2.TypoDescender);
        Read(tableOS2.TypoLineGap);
        Read(tableOS2.WinAscent);
        Read(tableOS2.WinDescent);
        Read(tableOS2.CodePageRange1);
        Read(tableOS2.CodePageRange2);
        Read(tableOS2.Height);
        Read(tableOS2.CapHeight);
        Read(tableOS2.DefaultChar);
        Read(tableOS2.BreakChar);
        Read(tableOS2.MaxContext);
      }

      //Reads in the PostScript table from the font program.
      void Read(PostScript& tablePOST)
      {
        if(!Tables)
          return;

        for(count i=0;i<NumTables;i++)
        {
          if(Tables[i].NameTag.Name()=="post")
          {
            Seek(Tables[i].Offset);
            break;
          }
          else if(i==NumTables-1)
            return;
        }

        Read(tablePOST.VersionNumber);
        Read(tablePOST.ItalicAngleInteger);
        Read(tablePOST.ItalicAngleDecimal);
        Read(tablePOST.UnderlinePosition);
        Read(tablePOST.UnderlineThickness);
        Read(tablePOST.isFixedPitch);
        Read(tablePOST.MinMemType42);
        Read(tablePOST.MaxMemType42);
        Read(tablePOST.MinMemType1);
        Read(tablePOST.MaxMemType1);
      }

      //Reads in the HorizontalMatrix table from the font program.
      void Read(HorizontalMatrix& tableHMTX)
      {
        if(!Tables)
          return;

        for(count i=0;i<NumTables;i++)
        {
          if(Tables[i].NameTag.Name()=="hmtx")
          {
            Seek(Tables[i].Offset);
            break;
          }
          else if(i==NumTables-1)
            return;
        }

        tableHMTX.NumberOfHMetrics=HHEA.NumberOfHMetrics;

        if(!tableHMTX.NumberOfHMetrics)
          return;

        if(tableHMTX.AdvanceWidths)
          delete [] tableHMTX.AdvanceWidths;
        if(tableHMTX.LeftSideBearings)
          delete [] tableHMTX.LeftSideBearings;

        tableHMTX.AdvanceWidths =
          new uint16[tableHMTX.NumberOfHMetrics];
        tableHMTX.LeftSideBearings =
          new int16[tableHMTX.NumberOfHMetrics];

        for(count i=0;
          i<tableHMTX.NumberOfHMetrics;i++)
        {
          Read(tableHMTX.AdvanceWidths[i]);
          Read(tableHMTX.LeftSideBearings[i]);
        }
      }

      ///Reads the KerningPairs table from the font program.
      void Read(KerningPairs& tableKERN)
      {
        if(!Tables)
          return;

        for(count i=0;i<NumTables;i++)
        {
          if(Tables[i].NameTag.Name()=="kern")
          {
            Seek(Tables[i].Offset);
            break;
          }
          else if(i==NumTables-1)
            return;
        }

        Read(tableKERN.VersionNumber);
        Read(tableKERN.NumTables);

        for(count i=0;i<tableKERN.NumTables;i++)
        {
          uint16 subtable_version;
          Read(subtable_version);

          uint16 Length;
          Read(Length);

          uint16 coverage;
          Read(coverage);

          if((coverage & 3) && (coverage<256))
          {
            Read(tableKERN.NumKerningPairs);
            tableKERN.GlyphLeft =
              new uint16[tableKERN.NumKerningPairs];
            tableKERN.GlyphRight =
              new uint16[tableKERN.NumKerningPairs];
            tableKERN.KernValue =
              new int16[tableKERN.NumKerningPairs];

            uint16 searchRange;
            uint16 entrySelector;
            uint16 rangeShift;

            Read(searchRange);
            Read(entrySelector);
            Read(rangeShift);
            for(count j=0;
              j<tableKERN.NumKerningPairs;j++)
            {
              Read(tableKERN.GlyphLeft[j]);
              Read(tableKERN.GlyphRight[j]);
              Read(tableKERN.KernValue[j]);
            }

            return;
          }
          else
          {
            //Have to skip through the values...
            int16 dummy=0;
            Read(dummy); //number of kerning pairs
            Read(dummy); //searchRange
            Read(dummy); //entrySelector
            Read(dummy); //rangeShift

            for(count j=0;
              j<tableKERN.NumKerningPairs;j++)
            {
              Read(dummy); //left glyph
              Read(dummy); //right glyph
              Read(dummy); //kern value
            }
          }
        }
      }

      ///Reads the CharacterMap table from the font program.
      void Read(CharacterMap& tableCMAP)
      {
        if(!Tables)
          return;

        count originalPosition = 0;
        for(count i = 0; i < NumTables; i++)
        {
          if(Tables[i].NameTag.Name() == "cmap")
          {
            originalPosition = Tables[i].Offset;
            Seek(originalPosition);
            break;
          }
          else if(i == NumTables - 1)
            return;
        }

        Read(tableCMAP.VersionNumber);
        Read(tableCMAP.NumTables);

        for(count i = 0; i < tableCMAP.NumTables; i++)
        {
          uint16 platformID;
          uint16 encodingID;
          uint32 Offset;

          Read(platformID);
          Read(encodingID);
          Read(Offset);

          //Look for the Microsoft Unicode table
          if(platformID == 3 && encodingID == 1)
          {
            Seek(originalPosition + Offset);
            break;
          }
        }

        //We are now reading at a Microsoft Unicode table
        uint16 format;
        Read(format);
        if(format != 4)
          return;

        uint16 Length;
        Read(Length);
        uint16 language;
        Read(language);
        uint16 segCountX2;
        Read(segCountX2);
        uint16 searchRange;
        Read(searchRange);
        uint16 entrySelector;
        Read(entrySelector);
        uint16 rangeShift;
        Read(rangeShift);
        tableCMAP.SegmentCount = segCountX2 >> 1;
        uint16 SegmentCount = tableCMAP.SegmentCount;
        tableCMAP.EndCharacterCode = new uint16[SegmentCount];
        tableCMAP.StartCharacterCode = new uint16[SegmentCount];
        tableCMAP.DeltaID = new int16[SegmentCount];
        tableCMAP.RangeOffsetID = new uint16[SegmentCount];

        for(count i = 0; i < SegmentCount; i++)
          Read(tableCMAP.EndCharacterCode[i]);

        uint16 pad;
        Read(pad);

        for(count i = 0; i < SegmentCount; i++)
          Read(tableCMAP.StartCharacterCode[i]);

        for(count i = 0; i < SegmentCount; i++)
          Read(tableCMAP.DeltaID[i]);

        for(count i = 0; i < SegmentCount; i++)
          Read(tableCMAP.RangeOffsetID[i]);

        tableCMAP.NumGlyphIndices =
          (Length - 8 * SegmentCount - 16) / 2;
        tableCMAP.GlyphIndexArray =
          new uint16[tableCMAP.NumGlyphIndices];

        for(uint32 i = 0; i < tableCMAP.NumGlyphIndices; i++)
          Read(tableCMAP.GlyphIndexArray[i]);
      }

      /**\brief Decodes the TrueType font and reads all of the tables
      defined by this class. \details If this method is successful you
      will have immediate access to the information via the public tables
      HEAD, HHEA, etc.*/
      void Decode(const byte* ptrTrueTypeProgram,
        count ProgramLength)
      {
        /*Clear out any information that was already here (if the
        function was called twice for example on two different fonts).*/
        if(Tables)
        {
          delete [] Tables;
          Tables = 0;
        }

        Valid=true; //Assume true and disprove.
        Length=(uint32)ProgramLength;
        Program=ptrTrueTypeProgram;
        Seek(0);
        Read(SFNTVersion);

        Read(NumTables);
        Read(SearchRange);
        Read(EntrySelector);
        Read(RangeShift);

        //Assume a maximum, just in case of corruption.
        if(NumTables > 0 && NumTables < 200)
        {
          Tables = new Table[NumTables];
          for(count i=0;i<NumTables;i++)
            Read(Tables[i]);
        }
        else
        {
          Tables=0;
          Valid=false;
          return;
        }

        Read(HEAD);
        Read(HHEA);
        Read(OS2);
        Read(POST);
        Read(HMTX); //Depends on HHEA for the NumberOfHMetrics
        Read(KERN);
        Read(CMAP);
      }

      /**\brief Makes a string summarizing the information in the tables
      given in the parameter list set to true.*/
      String Summarize(bool general=true, bool tabs=true,
        bool tableHEAD=true, bool tableHHEA=true, bool tableOS2=true,
        bool tablePOST=true, bool tableHMTX=true, bool tableKERN=true,
        bool tableCMAP=true)
      {
        using namespace prim;

        String Summary;
        if(general)
        {
          Summary += "Byte Length: "; Summary &= (integer)Length;
          Summary += "sfnt_version: ";
          Summary &= (integer)SFNTVersion;
          Summary += "NumTables: "; Summary &= (integer)NumTables;
          Summary += "SearchRange: ";
          Summary &= (integer)SearchRange;
          Summary += "EntrySelector: ";
          Summary &= (integer)EntrySelector;
          Summary += "RangeShift: "; Summary &= (integer)RangeShift;
          Summary++;
        }

        if(tabs)
        {
          for(int i=0;i<NumTables;i++)
          {
            Summary += "Table ";
            Summary &= Tables[i].NameTag.Name();
            Summary += "==========";
            Summary += "Offset:   ";
            Summary &= (integer)Tables[i].Offset;
            Summary += "Length:   ";
            Summary &= (integer)Tables[i].Length;
            Summary += "Checksum: ";
            Summary &= (integer)Tables[i].CheckSum;
            Summary += "Sum:      ";
            Summary &= (integer)Tables[i].ActualSum;
            Summary += "Is Valid: "; Summary &=
              (Tables[i].Valid ? "yes" : "no");
            Summary++;
          }
        }

        if(tableHEAD)
        {
          Summary += "HEAD";
          Summary += "====";
          Summary += "version: ";
          Summary &= (integer)HEAD.VersionNumber;
          Summary += "revision: ";
          Summary &= (integer)HEAD.FontRevision;
          Summary += "check-sum adjustment: ";
          Summary &= (integer)HEAD.CheckSumAdjustment;
          Summary += "magic number: ";
          Summary &= (integer)HEAD.MagicNumber;
          Summary += "Flags: "; Summary &= (integer)HEAD.Flags;
          Summary += "UnitsPerEm: ";
          Summary &= (integer)HEAD.UnitsPerEm;
          Summary += "date created high: ";
          Summary &= (integer)HEAD.DateCreatedHigh;
          Summary += "date created low: ";
          Summary &= (integer)HEAD.DateCreatedLow;
          Summary += "date modified high: ";
          Summary &= (integer)HEAD.DateModifiedHigh;
          Summary += "date modified low: ";
          Summary &= (integer)HEAD.DateModifiedLow;
          Summary += "x min: "; Summary &= (integer)HEAD.xMin;
          Summary += "y min: "; Summary &= (integer)HEAD.yMin;
          Summary += "x max: "; Summary &= (integer)HEAD.xMax;
          Summary += "y max: "; Summary &= (integer)HEAD.yMax;
          Summary += "mac style: "; Summary &= (integer)HEAD.MacStyle;
          Summary += "LowestRecPPEM: ";
          Summary &= (integer)HEAD.LowestRecPPEM;
          Summary += "hint direction: ";
          Summary &= (integer)HEAD.FontDirectionHint;
          Summary += "IndexToLocFormat: ";
          Summary &= (integer)HEAD.IndexToLocFormat;
          Summary += "glyph data format: ";
          Summary &= (integer)HEAD.GlyphDataFormat;
          Summary++;
        }

        if(tableHHEA)
        {
          Summary += "HHEA";
          Summary += "====";
          Summary += "version: ";
          Summary &= (integer)HHEA.VersionNumber;
          Summary += "ascender: ";
          Summary &= (integer)HHEA.Ascender;
          Summary += "descender: ";
          Summary &= (integer)HHEA.Descender;
          Summary += "line gap: ";
          Summary &= (integer)HHEA.LineGap;
          Summary += "advance width max: ";
          Summary &= (integer)HHEA.AdvanceWidthMax;
          Summary += "min lsb: ";
          Summary &= (integer)HHEA.MinLeftSideBearing;
          Summary += "min rsb: ";
          Summary &= (integer)HHEA.minRightSideBearing;
          Summary += "xMaxExtent: ";
          Summary &= (integer)HHEA.xMaxExtent;
          Summary += "CaretSlopeRise: ";
          Summary &= (integer)HHEA.CaretSlopeRise;
          Summary += "CaretSlopeRun: ";
          Summary &= (integer)HHEA.CaretSlopeRun;
          Summary += "CaretOffset: ";
          Summary &= (integer)HHEA.CaretOffset;
          Summary += "reserved1: ";
          Summary &= (integer)HHEA.Reserved1;
          Summary += "reserved2: ";
          Summary &= (integer)HHEA.Reserved2;
          Summary += "reserved3: ";
          Summary &= (integer)HHEA.Reserved3;
          Summary += "reserved4: ";
          Summary &= (integer)HHEA.Reserved4;
          Summary += "MetricDataFormat: ";
          Summary &= (integer)HHEA.MetricDataFormat;
          Summary += "NumberOfHMetrics: ";
          Summary &= (integer)HHEA.NumberOfHMetrics;
          Summary++;
        }

        if(tableOS2)
        {
          Summary += "OS/2";
          Summary += "====";
          Summary += "version: ";
          Summary &= (integer)OS2.VersionNumber;
          Summary += "xAvgCharWidth: ";
          Summary &= (integer)OS2.xAvgCharWidth;
          Summary += "WeightClass: ";
          Summary &= (integer)OS2.WeightClass;
          Summary += "WidthClass: ";
          Summary &= (integer)OS2.WidthClass;
          Summary += "Type: "; Summary &= (integer)OS2.Type;
          Summary += "SubscriptXSize: ";
          Summary &= (integer)OS2.SubscriptXSize;
          Summary += "SubscriptYSize: ";
          Summary &= (integer)OS2.SubscriptYSize;
          Summary += "SubscriptXOffset: ";
          Summary &= (integer)OS2.SubscriptXOffset;
          Summary += "SubscriptYOffset: ";
          Summary &= (integer)OS2.SubscriptYOffset;
          Summary += "SuperscriptXSize: ";
          Summary &= (integer)OS2.SuperscriptXSize;
          Summary += "SuperscriptYSize: ";
          Summary &= (integer)OS2.SuperscriptYSize;
          Summary += "SuperscriptXOffset: ";
          Summary &= (integer)OS2.SuperscriptXOffset;
          Summary += "SuperscriptYOffset: ";
          Summary &= (integer)OS2.SuperscriptYOffset;
          Summary += "StrikeoutSize: ";
          Summary &= (integer)OS2.StrikeoutSize;
          Summary += "StrikeoutPosition: ";
          Summary &= (integer)OS2.StrikeoutPosition;
          Summary += "FamilyClass: ";
          Summary &= (integer)OS2.FamilyClass;
          Summary += "UnicodeRange1: ";
          Summary &= (integer)OS2.UnicodeRange1;
          Summary += "UnicodeRange2: ";
          Summary &= (integer)OS2.UnicodeRange2;
          Summary += "UnicodeRange3: ";
          Summary &= (integer)OS2.UnicodeRange3;
          Summary += "UnicodeRange4: ";
          Summary &= (integer)OS2.UnicodeRange4;
          Summary += "VendorID: "; Summary &= OS2.VendorID.Name();
          Summary += "Selection: "; Summary &= (integer)OS2.Selection;
          Summary += "FirstCharIndex: ";
          Summary &= (integer)OS2.FirstCharIndex;
          Summary += "LastCharIndex: ";
          Summary &= (integer)OS2.LastCharIndex;
          Summary += "TypoAscender: ";
          Summary &= (integer)OS2.TypoAscender;
          Summary += "TypoDescender: ";
          Summary &= (integer)OS2.TypoDescender;
          Summary += "TypoLineGap: ";
          Summary &= (integer)OS2.TypoLineGap;
          Summary += "WinAscent: "; Summary &= (integer)OS2.WinAscent;
          Summary += "WinDescent: ";
          Summary &= (integer)OS2.WinDescent;
          Summary += "CodePageRange1: ";
          Summary &= (integer)OS2.CodePageRange1;
          Summary += "CodePageRange2: ";
          Summary &= (integer)OS2.CodePageRange2;
          Summary += "Height: "; Summary &= (integer)OS2.Height;
          Summary += "CapHeight: "; Summary &= (integer)OS2.CapHeight;
          Summary += "DefaultChar: ";
          Summary &= (integer)OS2.DefaultChar;
          Summary += "BreakChar: ";
          Summary &= (integer)OS2.BreakChar;
          Summary += "MaxContext: ";
          Summary &= (integer)OS2.MaxContext;
          Summary++;
        }

        if(tablePOST)
        {
          Summary += "POST";
          Summary += "====";
          Summary += "version: ";
          Summary &= (integer)POST.VersionNumber;
          Summary += "italic angle (int): ";
          Summary &= (integer)POST.ItalicAngleInteger ;
          Summary += "italic angle (dec): ";
          Summary &= (integer)POST.ItalicAngleDecimal ;
          Summary += "Underline position: ";
          Summary &= (integer)POST.UnderlinePosition ;
          Summary += "Underline thickness: ";
          Summary &= (integer)POST.UnderlineThickness ;
          Summary += "isFixedPitch: ";
          Summary &= (integer)POST.isFixedPitch ;
          Summary += "MinMemType42: ";
          Summary &= (integer)POST.MinMemType42 ;
          Summary += "MaxMemType42: ";
          Summary &= (integer)POST.MaxMemType42 ;
          Summary += "MinMemType1: ";
          Summary &= (integer)POST.MinMemType1 ;
          Summary += "MaxMemType1: ";
          Summary &= (integer)POST.MaxMemType1 ;
          Summary++;
        }

        if(tableHMTX)
        {
          Summary += "HTMX";
          Summary += "====";
          Summary += "number of metrics: ";
          Summary &= (integer)HMTX.NumberOfHMetrics;
          for(count i=0;i<HMTX.NumberOfHMetrics;i++)
          {
            Summary += "["; Summary &= (integer)i;
            Summary &= "]: wid = ";
            Summary &= (integer)HMTX.AdvanceWidths[i];
            Summary &= " | lsb = ";
            Summary &= (integer)HMTX.LeftSideBearings[i];
          }
        }

        if(tableKERN)
        {
          Summary += "KERN";
          Summary += "====";
          Summary += "number of kern pairs: ";
          Summary &= (integer)KERN.NumKerningPairs;
          for(count i=0;i<KERN.NumKerningPairs;i++)
          {
            Summary += "["; Summary &= (integer)i;
            Summary &= "]: l = ";
            Summary &= (integer)KERN.GlyphLeft[i];
            Summary &= ", r = "; Summary &=
              (integer)KERN.GlyphRight[i];
            Summary &= ", kern = ";
            Summary &= (integer)KERN.KernValue[i];
          }
        }

        if(tableCMAP)
        {
          Summary += "CMAP";
          Summary += "====";
          for(count i=0;i<256;i++)
          {
            Summary +=
              "["; Summary &= (integer)i;
            Summary &= "]: glyph = ";
            Summary &=
              (integer)CMAP.GlyphIndexFromCharacterCode(
              (uint16)i);
          }
        }

        Summary.Merge();

        return Summary;
      }

      //----------------------//
      //CONSTRUCTOR/DESTRUCTOR//
      //----------------------//

      ///Constructor attempts to decode the font program.
      TrueType(const byte* ptrTrueTypeProgram, count ProgramLength) 
        : Length(0), SFNTVersion(0), NumTables(0), SearchRange(0),
        EntrySelector(0), RangeShift(0), Tables(0), Valid(false), Program(0),
        SeekPosition(0)
      {
        Decode(ptrTrueTypeProgram, ProgramLength);
      }

      ///Destructor frees any memory associated with the pointers.
      virtual ~TrueType()
      {
        delete [] Tables;
      }
    };

    ///OpenType for now derives straight from TrueType.
    class OpenType : public TrueType
    {
    public:
      OpenType(const byte* ptrTrueTypeProgram, count ProgramLength) :
        TrueType(ptrTrueTypeProgram,ProgramLength) {}
      virtual ~OpenType(){}
    };
  }
}

#endif
