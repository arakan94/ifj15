/********* fce.h *********
 *
 * FIT VUT, IFJ 089
 * Author: Ivo Juracek, xjurac05
 * Summary: Inner functions.
 *
 */

#ifndef INNERFNC
#define INNERFNC

char *concat(char *str_one, char *str_two);
int find(char *text, char* pattern);
int length(const char *text);
char* sort(char text[]);
char* substr(char* s, int i, int n);

#endif

