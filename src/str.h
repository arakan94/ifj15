/******** str.h *********
 *
 * FIT VUT, IFJ 089
 * Summary: hlavickovy soubor pro praci s nekonecne dlouhymi retezci
 * Notes: Used from official example, added strFirst(), strLast() and strDelChar()
 *
 */

#ifndef STR_INCLUDED
#define STR_INCLUDED

typedef struct
{
  char* str;		// misto pro dany retezec ukonceny znakem '\0'
  int length;		// skutecna delka retezce
  int allocSize;	// velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
char strDelChar(string *s);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
char strFirst(string *s);
char strLast(string *s);
int strGetLength(string *s);

#endif // STR_INCLUDED
