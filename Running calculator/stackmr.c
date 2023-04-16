#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stackmr.h"

void InitializeStackmr(Stackmr *wsk)
{
    int j = MRSTACK - 1;
    for (int i = 0; i < MRSTACK; i++, j--)
    {
        wsk->item[i] = j;
        wsk->position = i+1;
    }
}

bool FullStackmr(Stackmr *wsk)
{
    return wsk->position == MRSTACK;
}

bool EmptyStackmr(Stackmr *wsk)
{
    return wsk->position == 0;
}

int AmountofPositionmr(Stackmr *wsk)
{
    return wsk->position;
}

bool PushStackmr(Stackmr *wsk, Item item)
{
    if (FullStackmr(wsk))
        return false;
    wsk->item[wsk->position] = item;
    wsk->position++; 
    return true;
}
bool PopStackmr(Stackmr *wsk, Item *iitem)
{
    if (EmptyStackmr(wsk))
        return false;

    Stackmr pol = *wsk;

    pol.position--;
    *iitem = wsk->item[pol.position];

    wsk->item[pol.position] = 0;
    // wsk->item[wsk->position] = 0;
    wsk->position--;

    if (wsk->position == 0)
        wsk->item[0]  = 0;
    return true;
}


void ClearStackmr(Stackmr *wsk)
{
    Item dummy;
    while (!EmptyStackmr(wsk))
        PopStackmr(wsk, &dummy);
}

