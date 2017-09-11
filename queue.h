#if !defined(QUEUE_H)
/* ========================================================================
   $File: queue.h $
   $Date: 11.09.2017 (DD/MM/YYYY) $
   $Revision: 1 $
   $Creator: Johannes Spies $
   $Notice: (C) Copyright 2017 by Johannes Spies. All Rights Reserved. $

   /=======\
   |QUEUE.H|
   \=======/

   TL;DR:  Simple one-header-'library' for a single-link(-ed) queue for C++
   ------  using templates. It exists because I don't want to write code
           for using queues and other simple data structures everytime I
           code... Maybe there are coming some more simple libraries if this
           is going to be used frequently by my in my code -- but, who 
           knows, maybe this library will just be some stupid bullshit...
           IDK, yet.

   Short TL;DR: Queue-library for C++.
   ------------
   
   Note:   The library is not designed to be save -> There can be SIGSEGVs
   -----   and other errors at any time. There is _absolutly_ no warranty
           shipped with this piece of code. I don't know what it does and
           I don't know what it _may_ do to your computer.

   Usage:
   ======
   Include:
   --------
       The library can be included in every file, BUT: There is no 
       implementation (or reference) to any function unless you put

           #define QUEUE_IMPLEMENTAION
   
       BEFORE one

           #include "queue.h"
       
       . There are some more macros you can define to change the library
       behavior. _Some_ of them are: 
          - QUEUE_DEBUG: If this macro is defined (BEFORE including the
            library in _one_ compilation unit), there will be _some_
            _few_ debug checks combined with debug output in the library.
            This only works if there is stdio.h available (and NOT
            QUEUE_NO_STDIO defined, which leads to the next macro...).
          - QUEUE_NO_STDIO: If this macro is defined, the library knows
            that it is not allowed to include stdio.h as a dependency.
          - QUEUE_NO_MALLOC: If this macor is defined, the library knows
            that it is not allowed to include malloc.h. Further you are
            expected to provide another allocator for the library since
            it does not know what memory to use to (de)allocate new/old
            elements. You can either set a entire new DefaultAllocator
            or provide your own allocator when you are using allocation-
            using library-functions.

   Visible structures:
   -----------
   1. queue:           The representation of the data structure queue. 
                       Should be self-explanatory.
   2. queue-allocator: An allocator used by every queue. Can be set in
                       function that require allocations/dynamic memory
                       access (QueueAdd allocates bytes, size is size of
                       an struct-pointer + size of the contained element;
                       QueuePop deallocated previously aquired memory-
                       bytes requested by QueueAdd). You can set your own
                       malloc/free functions or pass a custom allocator to
                       functions (mentioned before) which take in an 
                       allocator as a _hidden_ parameter.

   Functions:
   ----------
   1. CreateQueue:  Create a new, empty queue to work with
                    Example:
                    --------
                        // Create a new empty queue
                        queue<data_type> Queue = CreateQueue<data_type>();

   2. QueueAdd:     Add elements (of type contained) to the queue
                    Example:
                    --------
                        queue<data_type> Queue = ...;
                        data_type NewElement = ...;
                        QueueAdd(&Queue, NewElement);

   3. QueueFront:   Get the frontmost element of the queue.

   4. QueuePop:     Pop the frontmost element of the queue (the function
                    does not return the _old_ content -> Call QueueFront
                    first)

   5. QueueCount:   Count all emenents in the queue.

   6. QueueGet:     Get a pointer (!) to the element at the given index.

   7. QueueForEach: Iterate over the elements in the queue -> Get a
                    callback for each element visited. Return the element
                    or a new element -> You can edit the content of an
                    element while the for loop is stil ongoing. You can
                    (and should) use the provided macro to 'generate'
                    functions that can be called as for-each functions.
                    Example:
                    --------
                        QUEUE_FOR_EACH_FUNCTION(SomeFunctionName, data_type)
                        {
                            // ...
                            // Use the pointer to the element (Element) or
                            // the index of the element (Index) (or don't).
                            return(*Element);
                        }
                        
                        // ...
                        queue<data_type> Queue = ...
                        QueueForEach(&Queue, &SomeFunctionName);
   TODOs(js):
   ----------
   1. Maybe make C++s' lambdas working with this library. 
   2. Handle/show errors if there is no std-lib or malloc.h.

   Long time story short: You can use this library pretty simple.
   That's possibly it.
   Have fun using it, let my know if someone other than me uses it, 
   go ahead, use it.

   ~js
   ======================================================================== */
#if defined(QUEUE_DEBUG) && !defined(QUEUE_NO_STDIO)
#define QueueAssert(Expression) if(!(Expression)) {fprintf(stdout, "Expression %s in queue failed.\n", # Expression);}
#else
#define QueueAssert(Expression)
#endif

#define QUEUE_MEMALLOC_FUNCTION(Name) void *Name(long unsigned ByteCount)
typedef QUEUE_MEMALLOC_FUNCTION(queue_memalloc_function);

#define QUEUE_MEMFREE_FUNCTION(Name) void Name(void *MemoryChunk)
typedef QUEUE_MEMFREE_FUNCTION(queue_memfree_function);

struct queue_allocator
{
    queue_memalloc_function *Alloc;
    queue_memfree_function *Free;
};

#define QueueAllocate(Allocator, type) (type *)Allocator.Alloc(sizeof(type))
#define QueueFree(Allocator, Pointer) Allocator.Free((void *)Pointer)

extern queue_allocator DefaultAllocator;

template<typename contained>
struct queue_element
{
    contained Content;
    queue_element<contained> *Next;
};

template<typename contained>
struct queue
{
    queue_element<contained> *First;
};

//
// NOTE(js): Create a new empty queue.
//
template<typename contained> queue<contained> CreateQueue();

//
// NOTE(js): This function inserts a new element into the queue.
//
template<typename contained> void QueueAdd(queue<contained> *Queue, contained NewContent, queue_allocator Allocator = DefaultAllocator);

//
// NOTE(js): Returns the first element in the queue. Be careful, it can case a SIGSEGV if the queue is empty.
//
template<typename contained> contained QueueFront(queue<contained> *Queue);

//
// NOTE(js): Pop the front of the queue
//
template<typename contained> void QueuePop(queue<contained> *Queue, queue_allocator Allocator = DefaultAllocator);

//
// NOTE(js): How many elements are in the queue.
//
template<typename contained> long int unsigned QueueCount(queue<contained> *Queue);

//
// NOTE(js): Get one element from the queue.
//
template<typename contained> contained *QueueGet(queue<contained> *Queue, long int unsigned Index);

//
// NOTE(js): Iterate over each element.
//
#define QUEUE_FOR_EACH_FUNCTION(Name, contained_type) contained_type (Name)(contained_type *Element, long int unsigned Index)
template<typename contained> void QueueForEach(queue<contained> *Queue, QUEUE_FOR_EACH_FUNCTION(*Function, contained));

#if defined(QUEUE_IMPLEMENTATION)
#if !defined(QUEUE_NO_MALLOC)
#include <malloc.h>
queue_allocator DefaultAllocator = {malloc, free};
#endif

template<typename contained> queue<contained>
CreateQueue()
{
    queue<contained> Result;
    Result.First = 0;
    return(Result);
}

template<typename contained> void
QueueAdd(queue<contained> *Queue, contained NewContent, queue_allocator Allocator)
{
    queue_element<contained> *NewElement = QueueAllocate(Allocator, queue_element<contained>);
    NewElement->Content = NewContent;
    NewElement->Next = 0;
    if(Queue->First != 0)
    {
        queue_element<contained> *Current = Queue->First;
        while(Current->Next)
        {
            Current = Current->Next;
        }
        Current->Next = NewElement;
    }
    else
    {
        Queue->First = NewElement;
    }
}

template<typename contained> contained
QueueFront(queue<contained> *Queue)
{
    contained Result = Queue->First.Content;
    return(Result);
}

template<typename contained> void
QueuePop(queue<contained> *Queue, queue_allocator Allocator)
{
    if(Queue->First)
    {
        queue_element<contained> *NewFront = Queue->First->Next;
        Allocator.Free(Queue->First);
        Queue->First = NewFront;
    }
}

template<typename contained> long int unsigned
QueueCount(queue<contained> *Queue)
{
    long int unsigned Result = 0;
    queue_element<contained> *Current = Queue->First;
    while(Current)
    {
        Current = Current->Next;
        ++Result;
    }
    return(Result);
}

template<typename contained> contained *
QueueGet(queue<contained> *Queue, long int unsigned Index)
{
#if defined(QUEUE_DEBUG)
    long int unsigned ElementCount = QueueCount(Queue);
    QueueAssert(Index < ElementCount);
#endif
    queue_element<contained> *ResultElement = Queue->First;
    while(Index--)
    {
        ResultElement = ResultElement->Next;
    }
    contained *Result = &ResultElement->Content;
    return(Result);
}

template<typename contained> void
QueueForEach(queue<contained> *Queue, QUEUE_FOR_EACH_FUNCTION(*Function, contained))
{
    long int unsigned Count = QueueCount(Queue);
    queue_element<contained> *Current = Queue->First;
    for(long int unsigned ElementIndex = 0;
        ElementIndex < Count;
        ++ElementIndex)
    {
        contained *Element = &Current->Content;
        contained NewElement = Function(Element, ElementIndex);
        Current->Content = NewElement;
        Current = Current->Next;
    }
}
#endif

#define QUEUE_H
#endif
