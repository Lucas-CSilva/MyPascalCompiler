#include <string.h>
#include <symtab.h>

#define MAXSTBSIZE 0x100000

SYMTAB symtab[MAXSTBSIZE];

int symtab_next_entry = 0;

//looks up a predefined symbol regarding possible conflict with the current symbol to append to the symbol table
int symtab_append(char const *name, int lexlevel)
{
    int index = symtab_lookup(name, lexlevel);

    if (index != -1) return -1;

    symtab[symtab_next_entry].lexlevel = lexlevel;

    strcpy(symtab[symtab_next_entry].name, name);

    symtab_next_entry++;

    return 0;
}

int symtab_lookup(char const *name, int lexlevel)
{  
    while (lexlevel > 0)
    {
        for (int i = 0; i < symtab_next_entry; i++)
        {
            if (symtab[i].lexlevel != lexlevel) continue;

            if (strcmp(name, symtab[i].name) == 0)
            {
                return i;
            }
        }
        
        lexlevel--;
    }

    return -1;
}

void symtab_set_range_type(int index_begin, int type, int objtype, int parmflag, int referenceflag)
{
    for (int i = index_begin; i < symtab_next_entry; i++)
    {
        symtab[i].type = type;
        symtab[i].objtype = objtype;
        symtab[i].parmflag = parmflag;
        symtab[i].referenceflag = referenceflag;
    }
}

void symtab_set_objtype(int index, int objtype)
{
    symtab[index].objtype = objtype;
}