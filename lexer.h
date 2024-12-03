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


extern char *tokens[];
extern char lexeme[];
extern int linenum;

extern int gettoken(FILE *source);
extern char* get_token_to_string(int token);