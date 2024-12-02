#pragma once

#include <lexer.h>

typedef enum _type_
{
    INT32 = 0,
    INT64,
    FLOAT32,
    FLOAT64,
    BOOL
} EType;

typedef enum _objtype_
{
    TYPE_PROCEDURE = 0,
    TYPE_FUNCTION,
    TYPE_VARIABLE
} EObjType;


typedef struct _symtab_
{
    char name[MAXIDLEN];
    int objtype; // objtype ==  0 if procedure; == 1 if function; == 2 if variable
    int type; // type == 0 if int32; == int64; == 2 if float32; == 3 if float64; == 4 if bool
    int lexlevel;
    int parmflag; // == 0 if variable; == 1 if parameter 
    int referenceflag; // 0 if not reference; 1 if reference variable
    char offset[MAXIDLEN]; //variable //procedura //function address
} SYMTAB;

extern int symtab_append(char const *, int);
extern int symtab_lookup(char const *, int);
extern void symtab_set_range_type(int index_begin, int type, int objtype, int parmflag, int referenceflag);
extern void symtab_set_objtype(int index, int objtype);

extern SYMTAB symtab[];
extern int symtab_next_entry;