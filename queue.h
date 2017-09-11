#if !defined(QUEUE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Johannes Spies $
   $Notice: (C) Copyright 2017 by Johannes Spies. All Rights Reserved. $

   Simple one-header-'library' for a single-link(-ed) queue for C++ using
   templates.
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
