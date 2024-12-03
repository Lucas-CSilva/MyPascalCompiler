#pragma once

typedef enum _keyword_
{
    BEGIN = 0X10000, //65536
    PROGRAM,
    VAR,
    INTEGER,
    REAL,
    DOUBLE,
    BOOLEAN,
    PROCEDURE,
    FUNCTION,
    IF,
    THEN, 
    ELSE,
    WHILE,
    DO,
    REPEAT,
    UNTIL,
    DIV,
    AND,
    MOD,
    OR,
    NOT,
    END
} EKeyWord;

extern char *keyword[];

extern int iskeyword(char *word);
extern int is_token_keyword(int token);