/********* fce.c *********
 *
 * FIT VUT, IFJ 089
 * Author: Ivo Juracek, xjurac05
 * Summary: Inner functions.
 *
 */

/* **** Editation notes **** */
/*
 * 20.10: created functions file
 * 24.10: added comments
 */
/* **** end **** */

/* Include libraries */
#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<stdbool.h>
#include "ial.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function returns length of the string.
 */

int length(const char *text){
    //Initialize i: char counter
    int i = 0;
    //While is not text ended with 0
    while(text[i] != '\0'){
        i++;
    }
    // return char count
    return i;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function returns part of the string. Also tests if parameters are valid.
 */

char *substr(char *s, int i, int n){
    // If interval is very big, cut right border to length of the string
    if (i + n > length(s))
        n = length(s) - i;
    // Borders must be unsigned integers
    if(i >= 0 && n >= 0){
        // Allocate memory and test it
        char *text = malloc(n+1);
        if(text == NULL)
            return NULL;
        else {
            //Copy part of string, add \0 and return it
            memcpy(text, s+i, n);
            text[n] = '\0';
            return text;
        }
    }
    else
        // Borders are not valid
        return NULL;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function returns concatenation of the strings, inserted as parameters.
 */

char *concat(char *str_one, char *str_two){
    //Initialize strings' length
    int len_one = length(str_one);
    int len_two = length(str_two);

    //Try to allocate memory
    char *word = malloc(len_one + len_two + 1);
    if (word == NULL)
        return NULL;
    else {
        //Concatenate!
        memcpy(word, str_one, len_one);
        memcpy(word+len_one, str_two, len_two+1);

        return word;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function gets string and call QuickSort procedure with border, if is string < 1,
 * call QuickSort is not necessary.
 */
char* sort(char text[])
{
    //Test if string is longer than 1 letter)
    int l = strlen(text);
    char *news;
    if((news = malloc(l+1))){
        //copy memory to new allocated memory
        memcpy(news, text, l+1);
        if(l > 1)
            quicksort(news, 0, l-1);
    } else {news = NULL;}
    return news;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function tries find a pattern in text. If is pattern empty, return zero position.
 * If is text empty, return position -1. If is pattern 1 letter, faster than call all
 * BMA functions is walking 1 by 1. If is found, break from cycle and return position.
 * Else call BMA fast algorithm and return position or -1, if not found.
 */
int find(char *text, char* pattern){
    //Find lengths of strings
    int len_p = strlen(pattern);
    int len_t = strlen(text);

    //If is pattern empty, return 0
    if(len_p == 0)
        return 0;

    //If is text empty, return -1
    else if(len_t == 0)
        return -1;

    //Size of pattern is 1, faster is walking 1 by 1
    else if(len_p == 1){
        int position = 0;
        for(int i = 0; i < len_t; i++)
            //Is in text this letter?
            if(text[i] == pattern[0])
                //Yes, return position
                {position = i; break;}
        return position == 0 ? -1 : position;
    }
    else
        //More than 1 letter, call BMA
        {
            //If not found, return -1, else return position
            int position = bma(pattern, text, len_p, len_t);
            return position < len_t ? position - 1: -1;
        }
}
