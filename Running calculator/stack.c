#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

void InitializeStack(Stack *wsk)
{
    int j = MAINSTACK - 1;
    for (int i = 0; i < MAINSTACK; i++, j--)
    {
        wsk->item[i] = j;
        wsk->position = i+1;
    }
}

bool FullStack(Stack *wsk)
{
    return wsk->position == MAINSTACK;
}

bool EmptyStack(Stack *wsk)
{
    return wsk->position == 0;
}

int AmountofPosition(Stack *wsk)
{
    return wsk->position;
}

bool PushStack(Stack *wsk, Item item)
{
    if (FullStack(wsk))
        return false;
    wsk->item[wsk->position] = item;
    wsk->position++; 
    return true;
}
bool PopStack(Stack *wsk, Item *iitem)
{
    if (EmptyStack(wsk))
        return false;

    Stack pol = *wsk;

    pol.position--;
    *iitem = wsk->item[pol.position];

    wsk->item[pol.position] = 0;
    // wsk->item[wsk->position] = 0;
    wsk->position--;

    if (wsk->position == 0)
        wsk->item[0]  = 0;
    return true;
}


void ClearStack(Stack *wsk)
{
    Item dummy;
    while (!EmptyStack(wsk))
        PopStack(wsk, &dummy);
}

