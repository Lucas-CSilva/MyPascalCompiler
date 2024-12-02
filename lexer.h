#include <stdio.h>

#pragma once
#define MAXIDLEN 32

typedef enum _token_
{
    ID = 1024,
    OCT,
    HEX,
    NUM,
    ASGN,
    LESS,
    GREATER,
    LEQ,
    GEQ,
    NEQ,
    EQUAL,
    TYPE
}EToken;

extern char lexeme[];

extern int gettoken(FILE *p_source);
extern int linenum;