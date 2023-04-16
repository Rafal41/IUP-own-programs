
#ifndef _STACK_H_
#define _STACK_H_

#include <stdbool.h>
#define MAINSTACK 10

typedef unsigned int Item;

typedef struct stack
{
    Item item[MAINSTACK];
    int position;
}Stack;

/* dzialanie : inicjalizuje stos                                        */
/* warunek wstepny : wsk wskazuje na stos                               */
/* warunek koncowy : zwraca stos zainicjowany jako pusty                */
void InitializeStack(Stack *wsk);

/* dzialanie : sprawdza czy stos jest pełny                             */
/* warunek wstepny : wsk wskazuje na zainicjowany stos                  */
/* warunek koncowy : zwraca true jezeli stos jest pełny                 */
bool FullStack(Stack *wsk);

/* dzialanie : sprawdza czy stos jest pusty                             */
/* warunek wstepny : wsk wskazuje na zainicjowany stos                  */
/* warunek koncowy : zwraca true jezeli stos jest pusty                 */
bool EmptyStack(Stack *wsk);

/* dzialanie : określa liczbę pozycji na stosie                         */
/* warunek wstepny : wsk wskazuje na zainicjowany stos                  */
/* warunek koncowy : zwraca liczbe pozycji                              */
int AmountofPosition(Stack *wsk);

/* dzialanie : dodaje element na wierzch stosu                          */
/* warunek wstepny : wsk wskazuje na zainicjowany stos, item zawiera    */
/* pozycje dodawaną na wierzch stosu                                    */
/* warunek koncowy : jezeli stos nie jest pusty to dodaje element       */
/* na wierzch stosu,a funkcja zwraca true, w przeciwnym wypadku stos    */   
/* pozostaje niezmieniony, a funkcja zwraca false                       */          
bool PushStack(Stack *wsk, Item item);

/* dzialanie : pobiera pozycje ze stosu                                 */
/* warunek wstepny : wsk wskazuje na zainicjowany stos, a *iitem        */
/* wskazuje na pozycje docelową                                         */
/* warunek koncowy : jezeli stos nie jest pusty to pobiera element z    */
/* wierzchu stosu i przenosi do pozycji docolwej, a funkcja zwraca true */
/* w przeciwnym wypadku stos jest zerowany, a funkcja zwraca false      */      
/* Jezeli stos byl pusty juz wczesniej, to pozostaje niezmieniony,      */
/* a funkcja zwraca false                                               */
bool PopStack(Stack *wsk, Item *iitem);

/* dzialanie : czysci stos                                              */
/* warunek wstepny : wsk wskazuje na zainicjowany stos                  */
/* warunek koncowy : stos jest pusty                                    */
void ClearStack(Stack *wsk);


#endif