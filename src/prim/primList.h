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

#ifndef primList
#define primList

#include "primHeap.h"
#include "primLink.h"
#include "primMath.h"
#include "primProfiler.h"
#include "primTypes.h"

namespace prim
{
  /**A prim::List is a doubly linked list with some special indexing properties.
  List is derived from a Heap of unit type Link.

  List recognizes and optimizes sequential access by maintaining three origins
  of traversal: the beginning, a variable index, and the end. The variable index
  is updated each time an element is requested by index. This way, sequential
  access is as efficient as an iterator. You can thus use array-like indexing
  with no performance penalty:

\code
  for(count i = 0; i < myList.n(); i++)
    myList[i].doSomethingQuick();
\endcode

  In the above example, the list automatically saves the last retrieved link
  so that if you ask for an adjacent index, it will find the element without
  having to retraverse the whole list. This cached index is saved until the
  list is modified.
  
  This indexing optimization does not make random access more efficient. If
  truly random access is a requirement, then the Array would be a better
  choice.*/
  template <class T>
  class List
  {
  private:
    ///Pointer to the first link
    DoubleLink<T>* First;

    ///Pointer to the last link
    DoubleLink<T>* Last;

    ///Pointer to the last referenced link
    mutable DoubleLink<T>* LastReferenced;

    ///Index of the last referenced link
    mutable count LastReferencedIndex;

    ///The number of items in the list
    count Items;

#ifdef PRIM_USING_SINGLE_THREADED
    /**A static memory heap used for all the elements of same-typed lists. This
    will not work in a multi-threaded environment, since there are no safeguards
    prevent multiple threads from accessing the static heap at the same time.*/
    static Heap< DoubleLink<T> > LinkHeap;
#else
    /**A memory heap used for all the elements of same-typed lists. Since this
    is declared as a single object within each list, it will be thread-safe
    (assuming that any List objects that are shared across multiple threads use
    the appropriate safeguards).*/
    Heap< DoubleLink<T> > LinkHeap;
#endif

  protected:
    ///Returns a pointer to the link containing the given data.
    DoubleLink<T>* GetLinkFromItem(const T& Item)
    {
      /*We're going to use a trick in order to get pointers to the DoubleLink by
      calculating the beginning of the class using a test link for reference to
      determine where the data part is in relation to the rest of the class. By
      doing this we don't need to make non-portable assumptions about what a
      compiler does with member order in physical memory.*/

      static DoubleLink<T> TestLink;
      return (DoubleLink<T>*)((uintptr)(void*)&Item + 
        (uintptr)(void*)&TestLink.Data - (uintptr)(void*)&TestLink);
    }

    /**Swaps the position of two elements without copying their internal data.
    Be careful to swap only elements belong to the same list!*/
    void Swap(const T& A, const T& B)
    {
      //Get pointers to the actual links.
      DoubleLink<T>* ptrA = GetLinkFromItem(A);
      DoubleLink<T>* ptrB = GetLinkFromItem(B);

      //Make sure we're not swapping the same link.
      if(ptrA == ptrB)
        return;

      //Remember the original link pointers.
      DoubleLink<T>* oldAPrev = ptrA->Prev;
      DoubleLink<T>* oldANext = ptrA->Next;
      DoubleLink<T>* oldBPrev = ptrB->Prev;
      DoubleLink<T>* oldBNext = ptrB->Next;

      //Update the traversal optimization.
      if(LastReferenced == ptrA)
        LastReferenced = ptrB;
      else if(LastReferenced == ptrB)
        LastReferenced = ptrA;

      //Perform the swap depending on what the condition is.
      if(ptrA->Next == ptrB)
      {
        //Element A is just to the left of B.
        ptrB->Prev = oldAPrev;
        ptrB->Next = ptrA;
        ptrA->Prev = ptrB;
        ptrA->Next = oldBNext;

        //Fix adjacent links.
        if(oldAPrev)
          oldAPrev->Next = ptrB;
        else
          First = ptrB;

        if(oldBNext)
          oldBNext->Prev = ptrA;
        else
          Last = ptrA;
      }
      else if(ptrB->Next == ptrA)
      {
        //Element B is just to the left of A.
        ptrA->Prev = oldBPrev;
        ptrA->Next = ptrB;
        ptrB->Prev = ptrA;
        ptrB->Next = oldANext;

        //Fix adjacent links.
        if(oldBPrev)
          oldBPrev->Next = ptrA;
        else
          First = ptrA;

        if(oldANext)
          oldANext->Prev = ptrB;
        else
          Last = ptrB;
      }
      else
      {
        //There is a positive number of elements between A and B.
        math::Swap(ptrA->Prev, ptrB->Prev);
        math::Swap(ptrA->Next, ptrB->Next);

        //Fix adjacent links.
        if(oldAPrev)
          oldAPrev->Next = ptrB;
        else
          First = ptrB;

        if(oldANext)
          oldANext->Prev = ptrB;
        else
          Last = ptrB;

        if(oldBPrev)
          oldBPrev->Next = ptrA;
        else
          First = ptrA;

        if(oldBNext)
          oldBNext->Prev = ptrA;
        else
          Last = ptrA;
      }
    }

    ///Finds the i-th element by traversing the list from the beginning.
    T* TraverseFromBeginning(count i) const
    {
      _profile(primProfiles);
      if(i<0 || i>=Items)
      {
        LastReferencedIndex=-1;
        LastReferenced=0;
        _endprofile(primProfiles);
        return 0; //Out of range.
      }

      //Traverse the list from the beginning.
      DoubleLink<T>* Current=First;
      for(count j=0;j<Items;j++)
      {
        if(j==i)
        {
          LastReferencedIndex=i;
          LastReferenced=Current;
          _endprofile(primProfiles);
          return &Current->Data;
        }
        Current=Current->Next;
      }

      LastReferencedIndex=-1;
      LastReferenced=0;
      _endprofile(primProfiles);
      return 0; //Could not find the element.
    }

    ///Finds the i-th element by traversing the list from the end.
    T* TraverseFromEnd(count i) const
    {
      _profile(primProfiles);
      if(i<0 || i>=Items)
      {
        LastReferencedIndex=-1;
        LastReferenced=0;
        _endprofile(primProfiles);
        return 0; //Out of range.
      }

      //Traverse the list from the beginning.
      DoubleLink<T>* Current=Last;
      for(count j=Items-1;j>=0;j--)
      {
        if(j==i)
        {
          LastReferencedIndex=i;
          LastReferenced=Current;
          _endprofile(primProfiles);
          return &Current->Data;
        }
        Current=Current->Prev;
      }

      LastReferencedIndex=-1;
      LastReferenced=0;
      _endprofile(primProfiles);
      return 0; //Could not find the element.
    }

    T* TraverseFromElement(count i,
      DoubleLink<T>* Element, count ElementIndex) const
    {
      _profile(primProfiles);
      if(i<0 || i>=Items ||
        ElementIndex<0 || ElementIndex>=Items || Element==0)
      {
        LastReferencedIndex=-1;
        LastReferenced=0;
        _endprofile(primProfiles);
        return 0; //Out of range.
      }

      if(i == ElementIndex)
      {
        LastReferencedIndex=i;
        LastReferenced=Element;
        _endprofile(primProfiles);
        return &Element->Data; //No traversal necessary.
      }
      else if (i > ElementIndex)
      {
        for(count j=ElementIndex;j<=i;j++)
        {
          //Traverse forward to the element.
          if(j==i)
          {
            LastReferencedIndex=i;
            LastReferenced=Element;
            _endprofile(primProfiles);
            return &Element->Data;
          }
          Element=Element->Next;
        }
      }
      else
      {
        for(count j=ElementIndex;j>=i;j--)
        {
          //Traverse backwards to the element.
          if(j==i)
          {
            LastReferencedIndex=i;
            LastReferenced=Element;
            _endprofile(primProfiles);
            return &Element->Data;
          }
          Element=Element->Prev;
        }
      }

      LastReferencedIndex=-1;
      LastReferenced=0;
      _endprofile(primProfiles);
      return 0; //Could not find element.
    }
  public:
    ///Constructor
    List<T>() : First(0), Last(0), LastReferenced(0), LastReferencedIndex(0),
      Items(0) {}

    ///Destructor
    ~List<T>()
    {
      _profile(primProfiles);
      //Traverse the list and delete all of the elements
      DoubleLink<T>* Current = First;
      for(count i=0;i<Items;i++)
      {
        DoubleLink<T>* Next = Current->Next;
        LinkHeap.Delete(Current);
        Current = Next;
      }
      _endprofile(primProfiles);
    }

    ///Returns the number of items in the list.
    inline count Size(void) const {return Items;}

    ///Returns the number of items in the list as a shorthand for Size().
    inline count n(void) const {return Items;}

    /**Returns an item by index. If the next item returned is near to this
    index (i.e. sequential iteration), then the traversal time will be very
    fast.*/
    T& Item(count i) const
    {
      if(LastReferencedIndex>0)
      {
        if(i < LastReferencedIndex)
        {
          //Determine whether to traverse from the reference element.
          if(i <= LastReferencedIndex / 2)
            return *TraverseFromBeginning(i);
          else
            return *TraverseFromElement(i,
            LastReferenced,LastReferencedIndex);
        }
        else
        {
          //Determine whether to traverse from the reference element.
          if(i <= Items + LastReferencedIndex / 2)
            return *TraverseFromElement(i,
            LastReferenced,LastReferencedIndex);
          else
            return *TraverseFromEnd(i);
        }
      }
      else
      {
        //Determine which end has the quickest traversal
        if(i <= Items / 2)
          return *TraverseFromBeginning(i);
        else
          return *TraverseFromEnd(i);
      }
    }

    ///Gets an element reference using the familiar bracket notation.
    inline T& operator [] (count i) const {return Item(i);}

    ///Shorthand for getting the first element.
    inline T& first(void) const {return *TraverseFromBeginning(0);}

    ///Shorthand for getting the last element.
    inline T& last(void) const {return *TraverseFromEnd(Items-1);}

    ///Swaps the position of two elements without copying their internal data.
    void Swap(count i, count j)
    {
      //Check bounds and make sure the elements are not the same.
      if(i == j || i < 0 || j < 0 || i >= Items || j >= Items)
        return;

      //Swap the items.
      Swap(Item(i), Item(j));
    }

    ///Appends an element to the end of the list by copying the argument.
    void Append(const T& newElement)
    {
      _profile(primProfiles);
      DoubleLink<T>* NewLink = LinkHeap.New();
      if(!Items)
        Last=First=NewLink;
      else
      {
        NewLink->Prev=Last;
        Last=Last->Next=NewLink;
      }

      NewLink->Data = newElement;
      Items++;

      _endprofile(primProfiles);
    }

    ///For adding an element to the list using its default constructor.
    T& Add(void)
    {
      _profile(primProfiles);
      
      //Old code:
      //T newElement;
      //Append(newElement);

      //New code:
      DoubleLink<T>* NewLink = LinkHeap.New();
      if(!Items)
        Last=First=NewLink;
      else
      {
        NewLink->Prev=Last;
        Last=Last->Next=NewLink;
      }
      Items++;

      _endprofile(primProfiles);
      return last();
    }

    ///Prepends an element to the beginning of the list.
    void Prepend(const T& newElement)
    {
      _profile(primProfiles);
      DoubleLink<T>* NewLink = LinkHeap.New();
      if(!Items)
        Last=First=NewLink;
      else
      {
        NewLink->Next=First;
        First=First->Prev=NewLink;
      }

      NewLink->Data = newElement;

      Items++;

      //The reference link must be updated.
      LastReferencedIndex++;
      _endprofile(primProfiles);
    }

    ///Inserts an element before some other element referenced by index.
    void InsertBefore(const T& newElement, count ElementAfter)
    {
      _profile(primProfiles);
      //Use normal appends if the index is at one of the ends.
      if(ElementAfter <= 0)
      {
        Prepend(newElement);
        _endprofile(primProfiles);
        return;
      }
      if(ElementAfter >= Items)
      {
        Append(newElement);
        _endprofile(primProfiles);
        return;
      }

      //Traverse the list to update the reference link.
      Item(ElementAfter);

      //Determine the correct links to squeeze the new element between.
      DoubleLink<T>* LeftLink = LastReferenced->Prev;
      DoubleLink<T>* RightLink = LastReferenced;
      DoubleLink<T>* NewLink = LinkHeap.New();

      //Update the link pointers.
      RightLink->Prev = LeftLink->Next = NewLink;
      NewLink->Prev = LeftLink;
      NewLink->Next = RightLink;

      //Copy the data.
      NewLink->Data = newElement;

      //Increment the size.
      Items++;

      //Update the reference link.
      LastReferencedIndex = ElementAfter;
      LastReferenced = NewLink;
      _endprofile(primProfiles);
    }

    ///Inserts an element after some other element referenced by index.
    void InsertAfter(const T& newElement, count ElementBefore)
    {
      _profile(primProfiles);
      //Use normal appends if the index is at one of the ends.
      if(ElementBefore < 0)
      {
        Prepend(newElement);
        _endprofile(primProfiles);
        return;
      }
      if(ElementBefore >= Items - 1)
      {
        Append(newElement);
        _endprofile(primProfiles);
        return;
      }

      //Traverse the list to update the reference link
      Item(ElementBefore);

      //Determine the correct links to squeeze the new element between.
      DoubleLink<T>* LeftLink = LastReferenced;
      DoubleLink<T>* RightLink = LastReferenced->Next;
      DoubleLink<T>* NewLink = LinkHeap.New();

      //Update the link pointers
      RightLink->Prev = LeftLink->Next = NewLink;
      NewLink->Prev = LeftLink;
      NewLink->Next = RightLink;

      //Copy the data
      NewLink->Data = newElement;

      //Increment the size
      Items++;

      //The reference link is no good anymore
      LastReferencedIndex = ElementBefore + 1;
      LastReferenced = NewLink;
      _endprofile(primProfiles);
    }

    void Remove(count i)
    {
      _profile(primProfiles);
      if(i<0 || i >= Items)
      {
        _endprofile(primProfiles);
        return;
      }

      /*Calling this will find the element using the reference link instead of
      having to traverse all the way through.*/
      Item(i);

      DoubleLink<T>* LeftLink=LastReferenced->Prev;
      DoubleLink<T>* RightLink=LastReferenced->Next;

      if(!LeftLink && !RightLink)
        First=Last=0;
      else
      {
        if(LeftLink==0)
          First=RightLink;
        else
          LeftLink->Next=RightLink;

        if(RightLink==0)
          Last=LeftLink;
        else
          RightLink->Prev=LeftLink;
      }

      LinkHeap.Delete(LastReferenced);

      Items--;

      if(LeftLink != 0)
      {
        LastReferencedIndex = i - 1;
        LastReferenced = LeftLink;
      }
      else if(RightLink != 0)
      {
        LastReferencedIndex = i;
        LastReferenced = RightLink;
      }
      else
      {
        LastReferencedIndex = -1;
        LastReferenced = 0;
      }
      _endprofile(primProfiles);
    }

    /**Removes an element from the list and calls delete on it. Only use this
    if the element is a pointer to an object on the heap.*/
    void RemoveAndDelete(count i)
    {
      delete Item(i);
      Remove(i);
    }

    /**Removes each element from the list and calls its destructor. Note if the
    element is a pointer to an object, then call RemoveAndDeleteAll to do call
    delete on the pointers automatically.*/
    void RemoveAll(void)
    {
      _profile(primProfiles);
      while(Items > 0)
        Remove(0);

      _endprofile(primProfiles);
    }

    /**Deletes all elements and then removes them from the list. Only call this
    if the element is some kind of pointer to an object on the heap.*/
    void RemoveAndDeleteAll(void)
    {
      _profile(primProfiles);
      while(Items > 0)
      {
        delete Item(0);
        Remove(0);
      }

      _endprofile(primProfiles);
    }

    /**Copy constructor (use sparingly). Normally you would want to use 
    references to a List rather than copy the whole thing.*/
    List<T>(const List<T> &other) : First(0), Last(0),
      LastReferenced(0), LastReferencedIndex(0), Items(0)
    {
      count ItemCount = other.Items;
      for(count i = 0; i < ItemCount; i++)
        Add() = other[i];
    }

    List<T>& operator=(const List<T>& other)
    {
      if(&other == this)
        return *this; //Exit if the other list is the same as this one.

      RemoveAll();
      count ItemCount = other.Items;
      for(count i = 0; i < ItemCount; i++)
        Add() = other[i];
      return *this;
    }

    private:
    /**Internal data structure for QuickSort recursing. Since the QuickSort
    algorithm is massively recursive it overflows stack space very quickly. To
    cope with this inherent problem a non-recursing algorithm was implemented
    utilizing a heap-based stack. Using Heap this routine will only request 
    memory a few times, but keep in mind that for lists in the hundred-thousands
    of elements, the algorithm will quickly consume around 10 or 20 megabytes.*/
    struct QuickSortStack
    {
      DoubleLink<T>* Left;
      DoubleLink<T>* Right;
      DoubleLink<T>* Start;
      DoubleLink<T>* Current;
      DoubleLink<T>* OldCurrent;
      count Control;
      QuickSortStack* Next;

      ///Pushes a state onto the stack.
      void Push(QuickSortStack*& VarHead,
        DoubleLink<T>* VarLeft,
        DoubleLink<T>* VarRight,
        DoubleLink<T>* VarStart,
        DoubleLink<T>* VarCurrent,
        DoubleLink<T>* VarOldCurrent,
        count VarControl)
      {
        Next = VarHead;
        VarHead = this;
        Left = VarLeft;
        Right = VarRight;
        Start = VarStart;
        Current = VarCurrent;
        OldCurrent = VarOldCurrent;
        Control = VarControl;
      }

      ///Pops the most recent state off the stack.
      QuickSortStack* Pop(QuickSortStack*& VarHead,
        DoubleLink<T>*& VarLeft,
        DoubleLink<T>*& VarRight,
        DoubleLink<T>*& VarStart,
        DoubleLink<T>*& VarCurrent,
        DoubleLink<T>*& VarOldCurrent,
        count VarControl)
      {
        VarLeft = VarHead->Left;
        VarRight = VarHead->Right;
        VarStart = VarHead->Start;
        VarCurrent = VarHead->Current;
        VarOldCurrent = VarHead->OldCurrent;
        VarControl = VarHead->Control;
        QuickSortStack* OldHead = VarHead;
        VarHead = VarHead->Next;
        return OldHead;
      }


      ///Code control types which gate execution to one of three given paths.
      enum ControlType
      {
        FirstBranch,
        SecondBranch,
        ThirdBranch
      };

      ///Zeroing constructor.
      QuickSortStack() : Left(0), Right(0), Start(0), Current(0), OldCurrent(0),
        Control(0), Next(0) {}
    };
    
    public:
    ///Sorts the list using the QuickSort algorithm.
    void QuickSort(bool Ascending = true)
    {
      //Ensure there are at least two elements in the list.
      if(Items < 2)
        return;

      //Stack state variables
      DoubleLink<T>* Left = First;
      DoubleLink<T>* Right = Last;
      DoubleLink<T>* Start = 0;
      DoubleLink<T>* Current = 0;
      DoubleLink<T>* OldCurrent = 0;
      count Control = QuickSortStack::FirstBranch;

      //Initialize stack state.
      Heap<QuickSortStack> QuickSortHeap;
      QuickSortStack* Head = 0;

      //Start the control loop.
      do
      {
        switch(Control)
        {
        case QuickSortStack::FirstBranch:
          //Set the Start and the Current item pointers.
          Start = Left;
          Current = Left; //This is the algorithm's "pivot".

          //Loop until we encounter the right pointer.
          while(Current != Right)
          {
            //Move to the next item in the list
            Current = Current->Next;

            //Swap items if they are out of order.
            if(Ascending && Start->Data < Current->Data)
              prim::math::Swap(Current->Data, Start->Data);
            else if(!Ascending && Start->Data > Current->Data)
              prim::math::Swap(Current->Data, Start->Data);
          }

          //Swap the First and Current items.
          prim::math::Swap(Left->Data, Current->Data);

          //Save this Current item.
          OldCurrent = Current;

          //Check if we need to sort the left-hand side of the Current point.
          Current = OldCurrent->Prev;

          if(Current && Left->Prev != Current && Current->Next != Left &&
            Left != Current)
          {
            Control = QuickSortStack::SecondBranch;
            //---------------SAVE----------------
            //Save the stack state.
            QuickSortHeap.New()->Push(Head, Left, Right, Start, Current, 
              OldCurrent, Control);
            //-----------------------------------
            Left = Left;
            Right = Current;
            Start = 0;
            Current = 0;
            OldCurrent = 0;
            Control = QuickSortStack::FirstBranch;
            break;
          }
          else
          {
            Control = QuickSortStack::SecondBranch;
            break;
          }
        case QuickSortStack::SecondBranch:
          //Check if we need to sort the right hand side of the Current point.
          Current = OldCurrent->Next;
          if(Current && Current->Prev != Right && Right->Next != Current
            && Current != Right)
          {
            Control = QuickSortStack::ThirdBranch;
            //---------------SAVE----------------
            //Save the stack state.
            QuickSortHeap.New()->Push(Head, Left, Right, Start, Current, 
              OldCurrent, Control);
            //-----------------------------------
            Left = Current;
            Right = Right;
            Start = 0;
            Current = 0;
            OldCurrent = 0;
            Control = QuickSortStack::FirstBranch;
            break;
          }
          else
          {
            Control = QuickSortStack::ThirdBranch;
            break;
          }
        case QuickSortStack::ThirdBranch:
          if(Head) //If no head, then we're ending anyway.
          {
            //--------------REVERT---------------
            //Revert to the previous stack state.
            QuickSortStack* StackToDelete = Head->Pop(Head, Left, Right, 
              Start, Current, OldCurrent, Control);
            QuickSortHeap.Delete(StackToDelete);
            //-----------------------------------
          }
        };
      } while(Head);
    }

    ///Bubble-sorts the list in either ascending or descending order.
    void BubbleSort(bool Ascending = true)
    {
      if(Ascending)
      {
        for(count i = 0; i < Items - 1; i++)
          for(count j = i + 1; j < Items; j++)
            if(Item(i) > Item(j))
              Swap(Item(i), Item(j));
      }
      else
      {
        for(count i = 0; i < Items - 1; i++)
          for(count j = i + 1; j < Items; j++)
            if(Item(i) < Item(j))
              Swap(Item(i), Item(j));
      }
    }

    /**Sorts using one of two algorithms. For smaller lists, BubbleSort is used
    in conjunction with link-pointer swapping. For bigger lists, the QuickSort 
    algorithm is used with data copying. Since QuickSort is massively recursive, 
    link-pointer swapping is impossible since it would take more time to track 
    the changes made to the pointers at each level of the stack.*/
    void Sort(bool Ascending = true)
    {
      if(Items < 50)
        BubbleSort(Ascending);
      else
        QuickSort(Ascending);
    }
  };

#ifdef PRIM_USING_SINGLE_THREADED
  /*Instantiate the static heap of links. This allows multiple list objects of
  the same storage type to access a single garbage-collected pool of memory. You
  can only use this though if only a single thread uses the List object.*/
  template <class T> Heap< DoubleLink<T> > List<T>::LinkHeap;
#endif
}
#endif
