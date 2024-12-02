#include <stdio.h>
#include <parser.h>
#include <lexer.h>

FILE *src;

int main() 
{
    src = stdin;

    lookahead = gettoken(src);
    
    printf("\t%c\n", lookahead);

    program();
}