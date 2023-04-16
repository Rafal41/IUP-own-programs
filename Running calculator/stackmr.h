#ifndef _STACKMR_H_
#define _STACKMR_H_

#include <stdbool.h>
#include "stack.h"
#define MRSTACK 20

typedef struct stackmr
{
    Item item[MRSTACK];
    int position;
}Stackmr;

/* dzialanie : inicjalizuje stos                                        */
/* warunek wstepny : wsk wskazuje na stos                               */
/* warunek koncowy : zwraca stos zainicjowany jako pusty                */
void InitializeStackmr(Stackmr *wsk);

/* dzialanie : sprawdza czy stos jest pełny                             */
/* warunek wstepny : wsk wskazuje na zainicjowany stos                  */
/* warunek koncowy : zwraca true jezeli stos jest pełny                 */
bool FullStackmr(Stackmr *wsk);

/* dzialanie : sprawdza czy stos jest pusty                             */
/* warunek wstepny : wsk wskazuje na zainicjowany stos                  */
/* warunek koncowy : zwraca true jezeli stos jest pusty                 */
bool EmptyStackmr(Stackmr *wsk);

/* dzialanie : określa liczbę pozycji na stosie                         */
/* warunek wstepny : wsk wskazuje na zainicjowany stos                  */
/* warunek koncowy : zwraca liczbe pozycji                              */
int AmountofPositionmr(Stackmr *wsk);

/* dzialanie : dodaje element na wierzch stosu                          */
/* warunek wstepny : wsk wskazuje na zainicjowany stos, item zawiera    */
/* pozycje dodawaną na wierzch stosu                                    */
/* warunek koncowy : jezeli stos nie jest pusty to dodaje element       */
/* na wierzch stosu,a funkcja zwraca true, w przeciwnym wypadku stos    */   
/* pozostaje niezmieniony, a funkcja zwraca false                       */          
bool PushStackmr(Stackmr *wsk, Item item);

/* dzialanie : pobiera pozycje ze stosu                                 */
/* warunek wstepny : wsk wskazuje na zainicjowany stos, a *iitem        */
/* wskazuje na pozycje docelową                                         */
/* warunek koncowy : jezeli stos nie jest pusty to pobiera element z    */
/* wierzchu stosu i przenosi do pozycji docolwej, a funkcja zwraca true */
/* w przeciwnym wypadku stos jest zerowany, a funkcja zwraca false      */      
/* Jezeli stos byl pusty juz wczesniej, to pozostaje niezmieniony,      */
/* a funkcja zwraca false                                               */
bool PopStackmr(Stackmr *wsk, Item *iitem);

/* dzialanie : czysci stos                                              */
/* warunek wstepny : wsk wskazuje na zainicjowany stos                  */
/* warunek koncowy : stos jest pusty                                    */
void ClearStackmr(Stackmr *wsk);


#endif