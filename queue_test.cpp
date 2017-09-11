/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Johannes Spies $
   $Notice: (C) Copyright 2017 by Johannes Spies. All Rights Reserved. $

   Test file for queue.h.
   ======================================================================== */
#include <stdio.h>
#define QUEUE_IMPLEMENTATION
#define QUEUE_DEBUG
#include "queue.h"

static QUEUE_FOR_EACH_FUNCTION(TestForEach, int) {
    fprintf(stdout, "Queue[%lu]=%d\n", Index, *Element);
    return(*Element);
}

int
main(int ArgCount, char **Args) {
    queue<int signed> Queue = CreateQueue<int>();
    QueueAdd(&Queue, 10);
    QueueAdd(&Queue, 22);
    QueueAdd(&Queue, 23);
    QueueAdd(&Queue, 14);
    QueueAdd(&Queue, 45);
    QueueAdd(&Queue, 69);

    int *Element3 = QueueGet(&Queue, 3);
    fprintf(stdout, "Element[3]=%d\n", *Element3);
    *Element3 = 4;
    fprintf(stdout, "Element[3]=%d\n", *Element3);
    
    QueueForEach(&Queue, &TestForEach);
    
    QueuePop(&Queue);
    QueuePop(&Queue);
    QueuePop(&Queue);
    QueuePop(&Queue);
    QueueForEach(&Queue, &TestForEach);
}
