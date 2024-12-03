#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <keywords.h>

char *keyword[] = {
    "begin",
    "program",
    "var",    
    "integer",
    "real",
    "double",
    "boolean",
    "procedure",
    "function",
    "if",
    "then",
    "else",
    "while",
    "do",
    "repeat",
    "until",
    "div",
    "and",
    "mod",
    "or",
    "not",
    "end"
};

int iskeyword(char *word)
{
    for (int i = BEGIN; i <= END; i++){
        if (strcmp(word, keyword[i - BEGIN]) == 0){
            return i;
        } 
    }

    return 0;
}

int is_token_keyword(int token)
{
    return (token >= BEGIN && token <= END);
}