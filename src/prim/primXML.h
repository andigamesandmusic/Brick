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

#ifndef primXML
#define primXML

#include "primFile.h"
#include "primString.h"
#include "primUnicode.h"

namespace prim { namespace XML
{
  /**Base class from which all "bound" elements derive.*/
  struct Parser
  {
    struct Token
    {
      const ascii* FirstCharacter;
      count ByteLength;
      String Text;

      Token() : FirstCharacter(0), ByteLength(0) {}

      ///Caches a String copy of the token in member Text.
      void UpdateString(void)
      {
        Text.Clear();
        if(ByteLength && FirstCharacter)
          Text.Append(FirstCharacter, ByteLength);
      }
      
      Token& operator= (const Token& Other)
      {
        FirstCharacter = Other.FirstCharacter;
        ByteLength = Other.ByteLength;
        Text = Other.Text;
        
        return *this;
      }
    };


    /**Parses the next 'word'. A word is found by first skipping over any
    white space (as defined by an array of UCS4 codepoints terminated by zero)
    and then parsing a run of characters until either a white space delimiter
    is found or a word delimiter is found (also defined by an array of UCS4
    codepoints terminated by zero). The parser automatically increments the 
    pointer to the first character after the word (whether it is white space 
    or not). If the first character in a word is a word delimiter then only 
    that character will be returned.*/
    static Token ParseNextWord(const ascii*& UTF8String,
      const unicode::UCS4* WhiteSpaceDelimiters,
      const unicode::UCS4* AdditionalWordDelimiters)
    {
      using namespace unicode;
      
      //1) Skip through the white space.
      Token Word;
      {
        const ascii* ptrPreviousCharacter = UTF8String;
        bool isSearchingWhiteSpace = true;
        UCS4 Character = 0;
        while(isSearchingWhiteSpace)
        {
          //Decode the next UTF8 character.
          ptrPreviousCharacter = UTF8String;
          Character = UTF8::Decode(UTF8String);
          
          //At the end of stream -- no word.
          if(!Character)
          {
            Word.UpdateString();
            return Word;
          }
          
          //Check to see if the character is white space.
          bool isWhiteSpace = false;
          const UCS4* WhiteSpaceDelimiterCheck = WhiteSpaceDelimiters;
          while(*WhiteSpaceDelimiterCheck)
          {
            if(Character == *WhiteSpaceDelimiterCheck)
            {
              isWhiteSpace = true;
              break;
            }
            WhiteSpaceDelimiterCheck++;
          }

          if(!isWhiteSpace)
          {
            //The beginning of the word has been found.
            isSearchingWhiteSpace = false;

            /*Decrement the UTF8 string pointer so that it points to the
            beginning of the word.*/
            Word.FirstCharacter = UTF8String = ptrPreviousCharacter;
          }
          
          //Otherwise keep skipping white space characters.
        }
      }

      /*2) Determine the word's length by searching for the next white space 
      or word delimiter.*/
      {
        const ascii* ptrPreviousCharacter = UTF8String;
        bool isSearchingWord = true;
        UCS4 Character = 0;
        count WordLength = 0;
        while(isSearchingWord)
        {
          //Decode the next UTF8 character.
          ptrPreviousCharacter = UTF8String;
          Character = UTF8::Decode(UTF8String);

          //At the end of the stream -- word runs to the end of the stream.
          if(!Character)
          {
            UTF8String = ptrPreviousCharacter;
            Word.ByteLength = (count)(UTF8String - Word.FirstCharacter);
            Word.UpdateString();
            return Word;
          }

          //Increment the character count.
          WordLength++;
          
          //Check to see if the character is white space.
          bool isDelimiter = false;
          const UCS4* WhiteSpaceDelimiterCheck = WhiteSpaceDelimiters;
          while(*WhiteSpaceDelimiterCheck)
          {
            if(Character == *WhiteSpaceDelimiterCheck)
            {
              isDelimiter = true;
              break;
            }
            WhiteSpaceDelimiterCheck++;
          }

          //Check to see if the character is word delimiter.
          const UCS4* AdditionalWordDelimiterCheck = AdditionalWordDelimiters;
          while(*AdditionalWordDelimiterCheck && !isDelimiter)
          {
            if(Character == *AdditionalWordDelimiterCheck)
            {
              isDelimiter = true;
              break;
            }
            AdditionalWordDelimiterCheck++;
          }


          if(isDelimiter)
          {
            //The beginning of the word has been found.
            isSearchingWord = false;

            /*Decrement the UTF8 string pointer so that it points to the
            beginning of the word. The exception is if the word is one 
            character in which case there is no need for a decrement.*/
            if(WordLength > 1)
              UTF8String = ptrPreviousCharacter;

            Word.ByteLength = (count)(UTF8String - Word.FirstCharacter);
          }
          
          //Otherwise keep decoding characters from the word.
        }
      }

      Word.UpdateString();
      return Word;
    }

    struct Delimiters
    {
      static const unicode::UCS4 None[1];
      static const unicode::UCS4 WhiteSpace[5];
      static const unicode::UCS4 TagEntry[2];
      static const unicode::UCS4 TagExit[2];
      static const unicode::UCS4 TagName[5];
      static const unicode::UCS4 TagAttributeName[4];
      static const unicode::UCS4 TagAttributeValue[3];
      static const unicode::UCS4 Generic[6];
    };

    typedef count Error;

    struct Errors
    {
      static const Error None = 0;
      static const Error EmptyDocument = 1;
      static const Error UnmatchedBracket = 2;
      static const Error UnexpectedTagName = 3;
      static const Error UnexpectedCharacter = 4;
      static const Error UninterpretableElement = 5;
      static const Error UnmatchedTagName = 6;
    };
  };

  /**An object can either be an element or a string of text. It is necessary
  to store a list of generic objects so that something like this:
  <foo>hello<foo2></foo2>there</foo>
  can accurately be represented.*/
  class Element;
  class Text;
  class Document;

  class Object
  {
  public:
    virtual Element* IsElement(void) {return 0;}
    virtual Text* IsText(void) {return 0;}
    virtual ~Object() {}
  };

  class Text : public Object, public String
  {
  public:
    friend class Document;

    Text() {}

    Text(const String& NewString) : String(NewString) {}

    Text* IsText(void)
    {
      return this;
    }

    virtual ~Text()
    {
    }
  };

  class Element : public Object
  {
  public:
    friend class Document;

    ///Stores an XML attribute by its name and value.
    struct Attribute
    {
      String Name;
      String Value;

      Attribute& operator= (const Attribute& Other)
      {
        Name = Other.Name;
        Value = Other.Value;
        return *this;
      }
    };
  protected:
    ///Stores the name of the element tag.
    String Name;

    ///Stores a list of the element tag's attributes.
    List<Attribute> Attributes;

    ///Stores a list of objects containing text and subelements.
    List<Object*> Objects;

  public:
    Element* IsElement(void) {return this;}

    ///Default constructor.
    Element() {}

    ///Constructor specifying the tag name of the element.
    Element(const String& Name)
    {
      Element::Name = Name;
    }
    
    ///Returns a reference to the list of this element's tag name.
    const String& GetName(void) {return Name;}

    ///Sets the tag name of this element.
    void SetName(const String& NewName)
    {
      Name = NewName;
    }

    ///Returns a reference to the list of this element's tag attributes.
    const List<Attribute>& GetAttributes(void) {return Attributes;}

    ///Adds an attribute to the attribute list.
    void AddAttribute(const Attribute& NewAttribute)
    {
      Attributes.Add() = NewAttribute;
    }

    ///Adds an attribute from a string pair.
    void AddAttribute(const String& Name, const String& Value)
    {
      Attribute a;
      a.Name = Name;
      a.Value = Value;
      Attributes.Add() = a;
    }

    /**Gets the value of a particular attribute. If the attribute can not be
    located, this method will return an empty string.*/
    const String& AttributeValue(const ascii* Attribute)
    {
      for(count i = 0; i < Attributes.n(); i++)
      {
        if(Attributes[i].Name == Attribute)
          return Attributes[i].Value;
      }
    }

    /**Removes an attribute by name from the attribute list. Note that it will
    remove all instances of the attribute (however there should only be 
    one).*/
    void RemoveAttribute(const String& AttributeName)
    {
      for(count i = 0; i < Attributes.n(); i++)
      {
        if(Attributes[i].Name == AttributeName)
        {
          Attributes.Remove(i);
          i--;
        }
      }
    }

    ///Returns a reference to the list of this element's objects.
    const List<Object*>& GetObjects(void) {return Objects;}

    ///Adds a pointer to an object in the object list.
    void AddObject(Object* NewObject, bool AddAtBeginning = false)
    {
      if(AddAtBeginning)
        Objects.Prepend(NewObject);
      else
        Objects.Append(NewObject);
    }

    ///Adds a pointer to an object in the object list.
    void AddElementInOrder(Element* NewElement, const String* Order)
    {
      String& NewElementName = NewElement->Name;
      count NewElementOrder = -1;
      count OrderNumber = 0;

      const String* ptrOrder = Order;

      while(*ptrOrder != "")
      {
        if(*ptrOrder == NewElementName)
          NewElementOrder = OrderNumber;
        ptrOrder++;
        OrderNumber++;
      }

      if(NewElementOrder == -1)
      {
        /*Could not find element in the ordering scheme so just add to the end
        and exit.*/
        AddObject(NewElement);
        return;
      }

      count ElementToInsertAfter = -1;
      for(count i = Objects.n() - 1; i >= 0; i--)
      {
        if(Element* e = Objects[i]->IsElement())
        {
          //Get the order index of the element in question.
          count CurrentOrder = -1;
          for(count j = 0; j < OrderNumber; j++)
          {
            if(Order[j] == e->Name)
            {
              CurrentOrder = j;
              break;
            }
          }

          if(CurrentOrder != -1 && CurrentOrder <= NewElementOrder)
          {
            /*Found either the same kind of element or the last element that
            would appear before this kind of element.*/
            ElementToInsertAfter = i;
            break;
          }
        }
      }

      if(ElementToInsertAfter != -1)
      {
        /*Insert the element after either the last element of the same kind or
        the last element whose order comes before an element of this kind.*/
        Objects.InsertAfter(NewElement,ElementToInsertAfter);
      }
      else if(NewElementOrder == 0)
      {
        /*This happens when the new element is ordered first and thus has no
        other element to go after.*/
        AddObject(NewElement, true);
      }
      else
      {
        //Unknown situation -- just add the object to the end in this case.
        AddObject(NewElement, false);
      }
    }

    ///Removes all children, both text and elements.
    void RemoveAllObjects(void)
    {
      Objects.RemoveAndDeleteAll();
    }

    ///Removes all elements by a particular tag name.
    void RemoveElementType(const String& ElementTagName)
    {
      for(count i = 0; i < Objects.n(); i++)
      {
        if(Element* e = Objects[i]->IsElement())
        {
          if(e->Name == ElementTagName)
          {
            Objects.RemoveAndDelete(i);
            i--;
          }
        }  
      }
    }

    /**Removes an element by its searching for its tag name and id attribute.
    If there are more than one by the same id, then they are all removed.*/
    void RemoveElementByID(const String& ElementTagName, const String& ID)
    {
      for(count i = 0; i < Objects.n(); i++)
      {
        if(Element* e = Objects[i]->IsElement())
        {
          if(e->Name == ElementTagName && e->AttributeValue("id") == ID)
          {
            Objects.RemoveAndDelete(i);
            i--;
          }
        }
      }
    }

    //Gets the ith child of a particular tag name.
    Element* GetChildByTagName(const String& tag, count index = 0)
    {
      count Count = 0;
      for(count i = 0; i < Objects.n(); i++)
      {
        if(Element* e = Objects[i]->IsElement())
        {
          if(e->GetName() == tag)
          {
            if(Count == index)
              return e;
            Count++;
          }
        }
      }
      return 0;
    }

    ///Gets the ith child of a particular type.
    template<class ChildType>
    ChildType* GetChildOfType(count index = 0)
    {
      count Count = 0;
      for(count i = 0; i < Objects.n(); i++)
      {
        if(ChildType* c = dynamic_cast<ChildType*>(Objects[i]->IsElement()))
        {
          if(Count == index)
            return c;
          Count++;
        }
      }
      return 0;
    }

    //Gets the ith child of a particular tag name.
    count CountChildrenByTagName(const String& tag)
    {
      count Count = 0;
      for(count i = 0; i < Objects.n(); i++)
      {
        if(Element* e = Objects[i]->IsElement())
        {
          if(e->GetName() == tag)
            Count++;
        }
      }
      return Count;
    }

    ///Determines the number of children in an element of a particular type.
    template<class ChildType>
    count CountChildrenOfType(void)
    {
      count Count = 0;
      for(count i = 0; i < Objects.n(); i++)
      {
        if(dynamic_cast<ChildType*>(Objects[i]->IsElement()))
          Count++;
      }
      return Count;
    }

    ///Virtual destructor.
    virtual ~Element()
    {
      Objects.RemoveAndDeleteAll();
    }

  protected:
    /**Overloaded method for creating virtual elements from an arbitrary name.
    You'll want to do something like:
    /code
    if(TagName == "foo")
      return new foo; //'foo' is a derived Element.
    else if(TagName == "blah")
      return new blah; //'blah' is another derived Element.
    else
      return 0; //Indicates that a generic Element will be created.
    /endcode*/
    virtual Element* CreateChild(const String& TagName)
    {
      TagName.Merge(); //Stop unreferenced formal parameter warning.
      return 0;
    }

    /**Used by virtual elements to cache data in a more efficient manner. This
    is called automatically immediately after parsing. You can use this to
    store cached copies of any data so that the structure of the XML document
    can be used as a way to facilitate organization without having to parse
    strings of text every time you need access to something.*/
    virtual bool Interpret(void)
    {
      bool Success = true;
      for(count i = 0; i < Objects.n(); i++)
      {
        if(Element* e = dynamic_cast<Element*>(Objects[i]))
        {
          if(!e->Interpret())
            Success = false;
        }
      }
      return Success;
    }

    /**Translates cached data in virtual elements back into the XML structure.
    This method is automatically called before XML data is saved to file for
    example.*/
    virtual void Translate(void)
    {
      for(count i = 0; i < Objects.n(); i++)
      {
        if(Element* e = dynamic_cast<Element*>(Objects[i]))
          e->Translate();
      }
    }

    Parser::Error Parse(const ascii*& Markup)
    {
      //Local variables to use for parsing.
      Parser::Token t;

      //Entering tag.
      t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
          Parser::Delimiters::TagEntry);
      if(t.Text != "<")
        return Parser::Errors::EmptyDocument;

      //Parse tag name.
      t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
          Parser::Delimiters::TagName);
      Name = t.Text;

      //Parse the attributes.
      bool isParsingAttributes = true;
      while(isParsingAttributes)
      {
        t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
            Parser::Delimiters::TagAttributeName);

        if(t.Text == "/")
        {
          //Element is in the self-closing notation, i.e. <br/>.
          
          //Look for the exit character.
          t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
            Parser::Delimiters::TagExit);
          if(t.Text != ">")
            return Parser::Errors::UnexpectedCharacter;

          //Element is finished parsing, so return.
          return Parser::Errors::None;
        }
        else if(t.Text == ">")
        {
          //There are no more attributes.
          isParsingAttributes = false;
        }
        else if(t.Text == "=")
        {
          return Parser::Errors::UnexpectedCharacter;
        }
        else
        {
          //Parsed attribute name.
          Attributes.Add();
          Attributes.last().Name = t.Text;

          //Look for the equal sign.
          t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
            Parser::Delimiters::TagAttributeName);
          if(t.Text != "=")
            return Parser::Errors::UnexpectedCharacter;

          //Look for the beginning quotation mark.
          t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
            Parser::Delimiters::TagAttributeValue);
          if(t.Text != "\"")
            return Parser::Errors::UnexpectedCharacter;

          //Parse the attribute text.
          t = Parser::ParseNextWord(Markup, Parser::Delimiters::None,
            Parser::Delimiters::TagAttributeValue);

          if(t.Text != "\"")
          {
            //Attribute value is a non-empty string.
            Attributes.last().Value = t.Text;

            //Look for closing quotation mark.
            t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
            Parser::Delimiters::TagAttributeValue);
            if(t.Text != "\"")
              return Parser::Errors::UnexpectedCharacter;
          }
        }
      }

      //Parse the element's objects (text and elements).
      bool isParsingObjects = true;
      while(isParsingObjects)
      {
        //Save the parse pointer in case it is a new tag.
        const ascii* TagBeginning = Markup;

        t = Parser::ParseNextWord(Markup, Parser::Delimiters::None,
          Parser::Delimiters::TagEntry);
        if(t.Text == "<") //Making new element or closing this one.
        {
          t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
            Parser::Delimiters::TagName);

          if(t.Text == "!")
          {
            //Inside of a comment. Skip to the '>' character.
            t = Parser::ParseNextWord(Markup, Parser::Delimiters::None,
              Parser::Delimiters::TagExit);
            t = Parser::ParseNextWord(Markup, Parser::Delimiters::None,
              Parser::Delimiters::TagExit);
            if(t.Text != ">")
              return Parser::Errors::UnexpectedCharacter;
          }
          else if(t.Text == "/")
          {
            //Closing this element. Make sure tag matches.
            t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
              Parser::Delimiters::TagName);
            if(t.Text != Name)
              return Parser::Errors::UnmatchedTagName;

            //Look for '>'
            t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
              Parser::Delimiters::TagExit);
            if(t.Text != ">")
              return Parser::Errors::UnexpectedCharacter;

            isParsingObjects = false;
          }
          else //New child element.
          {
            //Create new child element using virtual overloaded method.
            Element* e = CreateChild(t.Text);
            if(!e)
              e = new Element; //Default to generic element.
            Objects.Add() = e;

            //Reverse the markup pointer to the beginning of the tag.
            Markup = TagBeginning;

            //Parse the child element.
            Parser::Error Error = e->Parse(Markup);

            /*If there was an error then this causes a cascading failure and
            the entire parse aborts.*/
            if(Error)
              return Error;

            /*We will let the root element trigger Interpret() after the
            document has been completely parsed. This is done because the
            Interpret() method is itself recursive.*/
            //e->Interpret();
          }
        }
        else
        {
          //Making new text object.
          Text* NewText = new Text;
          *NewText &= t.Text;
          Objects.Add() = NewText;
        }
      }
      return Parser::Errors::None;
    }

  public:
    ///Appends this element tag, contents, and closing tag to a string.
    void AppendToString(String& XMLOutput);

    ///Returns all of this element's text as a single string.
    String GetAllSubTextAsString(void)
    {
      String s;
      for(count i = 0; i < Objects.n(); i++)
      {
        Text* t = Objects[i]->IsText();
        if(t)
          s &= *(String*)t;
      }
      return s;
    }
  };

  class Document
  {
  public:
    Element* Root;

    Document() : Root(0) {}
    virtual ~Document() {delete Root;}

    virtual Element* CreateRootElement(String& RootTagName)
    {
      RootTagName.Merge(); //Stop unreferenced formal parameter warning.
      return 0;
    }
  protected:
    Parser::Error ParseHeader(const ascii*& Markup, String& RootTagName)
    {
      RootTagName.Merge(); //Stop unreferenced formal parameter warning.

      //Local variables to use for parsing.
      Parser::Token t;

      bool isParsingHeader = true;
      while(isParsingHeader)
      {
        //Save the position of the tag beginning for later reference.
        const ascii* TagBeginning = Markup;

        /*At the document level we look for the XML header and the DOCTYPE 
        before attempting to parse the root element.*/
        t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
          Parser::Delimiters::TagEntry);

        //Look inside the tag.
        if(t.Text == "<")
        {
          t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
            Parser::Delimiters::TagName);

          if(t.Text == "?")
          {
            /*Header typically looks something like this:
            <?xml version="1.0" encoding="UTF-8"?>
            Ignoring for now.*/
            while(t.Text != ">")
            {
              t = Parser::ParseNextWord(Markup, 
                Parser::Delimiters::WhiteSpace,
                Parser::Delimiters::TagExit);

              if(t.Text == "")
                return Parser::Errors::UnmatchedBracket;
            }        
          }
          else if(t.Text == "!")
          {
            t = Parser::ParseNextWord(Markup, Parser::Delimiters::WhiteSpace,
              Parser::Delimiters::TagName);
            if(t.Text == "DOCTYPE")
            {
              /*Skip through the DOCTYPE definitions.*/
              while(t.Text != ">")
              {
                t = Parser::ParseNextWord(Markup, 
                  Parser::Delimiters::WhiteSpace,
                  Parser::Delimiters::TagExit);

                if(t.Text == "")
                  return Parser::Errors::UnmatchedBracket;
              }
            }
            else
            {
              //Unexpected tag name. Parser failed.
              return Parser::Errors::UnexpectedTagName;
            }
          }
          else
          {
            //Now we are inside a tag.
            Markup = TagBeginning;
            isParsingHeader = false;
          }
        }
        else
        {
          //Presumably an empty document. Parse failed.
          return Parser::Errors::EmptyDocument;
        }
      }

      return Parser::Errors::None;
    }
    
    /**Method called before writing the root node to the output stream. Should
    specify the encoding and DOCTYPE if necessary.*/
    virtual void WriteHeader(String& XMLOutput)
    {
      XMLOutput.Merge(); //Stop unreferenced formal parameter warning.
    }
  public:
    Parser::Error ParseDocument(const String& MarkupDocument)
    {
      _profile(primProfiles);
      //Get a pointer to the markup string.
      const ascii* Markup = MarkupDocument.Merge();

      String RootTagName;
      Parser::Error Error = ParseHeader(Markup,RootTagName);
      if(Error)
        return Error;

      //Delete root note if it already exists.
      delete Root;

      //Create the root node.
      Root = CreateRootElement(RootTagName);
      if(!Root)
        Root = new Element;

      //Parse the document tree.
      Error = Root->Parse(Markup);
      if(Error)
      {
        delete Root;
        return Error;
      }

      //Interpret the entire document.
      Root->Interpret();

      _endprofile(primProfiles);
      
      return Parser::Errors::None;
    }
  
    void WriteToString(String& XMLOutput)
    {
      _profile(primProfiles);
      WriteHeader(XMLOutput);
      Root->AppendToString(XMLOutput);
      _endprofile(primProfiles);
    }

    void WriteToFile(String Filename)
    {
      String XMLOutput;
      WriteToString(XMLOutput);
      File::Replace(Filename, XMLOutput);
    }
  };

  template <class T>
  class Property
  {
    T Data;
    prim::String Name;
  public:
    Property(const prim::String& InitName) : Name(InitName){}
    operator T& (void) {return Data;}
    T& operator = (const T& In)
    {
      Data = In;
      return Data;
    }

    T& operator = (const prim::String& In)
    {
      Data = (T)In;
      return Data;
    }

    prim::String GetName(void)
    {
      return Name;
    }
    
    inline bool SetFrom(Element* e)
    {
      if(e && e->GetName() == Name)
      {
        Data = (T)e->GetAllSubTextAsString();
        return true;
      }
      else return false;
    }

    inline bool SetFrom(Object* o)
    {
      return SetFrom(o->IsElement());
    }
  };
}}

#endif
