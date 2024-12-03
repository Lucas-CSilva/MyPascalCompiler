#include <string.h>
#include <symtab.h>

#define MAXSTBSIZE 0x100000

SYMTAB symtab[MAXSTBSIZE];

int symtab_next_entry = 0;

/// @brief Appends a symbol to the symbol table if there is no conflict.
/// @param name The name of the symbol to append.
/// @param lexlevel The lexical level of the symbol.
/// @return 0 if the symbol was successfully appended, -1 if there was a conflict.
int symtab_append(char const *name, int lexlevel)
{
    // Look up the symbol in the symbol table to check for conflicts
    int index = symtab_lookup(name, lexlevel);

    // If the symbol is found (conflict exists), return -1
    if (index != -1) return -1;

    // Set the lexical level of the new symbol
    symtab[symtab_next_entry].lexlevel = lexlevel;

    // Copy the name of the new symbol to the symbol table
    strcpy(symtab[symtab_next_entry].name, name);

    // Increment the next entry index in the symbol table
    symtab_next_entry++;

    // Return 0 to indicate successful append
    return 0;
}

/// @brief Looks up a symbol in the symbol table.
/// @param name The name of the symbol to look up.
/// @param lexlevel The lexical level of the symbol.
/// @return The index of the symbol in the symbol table if found, -1 otherwise.
int symtab_lookup(char const *name, int lexlevel)
{
    // While the lexical level is greater than 0
    while (lexlevel > 0)
    {
        // Iterate through the symbol table entries
        for (int i = 0; i < symtab_next_entry; i++)
        {
            // If the lexical level of the current entry does not match, continue to the next entry
            if (symtab[i].lexlevel != lexlevel) continue;

            // If the name of the current entry matches the name being looked up
            if (strcmp(name, symtab[i].name) == 0)
            {
                // Return the index of the matching entry
                return i;
            }
        }
        
        // Decrement the lexical level to check the next outer scope
        lexlevel--;
    }

    // Return -1 if the symbol is not found
    return -1;
}

/// @brief Sets the type, object type, parameter flag, and reference flag for a range of symbols in the symbol table.
/// @param index_begin The starting index in the symbol table.
/// @param type The type to set for the symbols.
/// @param objtype The object type to set for the symbols.
/// @param parmflag The parameter flag to set for the symbols.
/// @param referenceflag The reference flag to set for the symbols.
void symtab_set_range_type(int index_begin, int type, int objtype, int parmflag, int referenceflag)
{
    // Iterate through the symbol table entries starting from index_begin to symtab_next_entry
    for (int i = index_begin; i < symtab_next_entry; i++)
    {
        // Set the type for the current symbol
        symtab[i].type = type;
        // Set the object type for the current symbol
        symtab[i].objtype = objtype;
        // Set the parameter flag for the current symbol
        symtab[i].parmflag = parmflag;
        // Set the reference flag for the current symbol
        symtab[i].referenceflag = referenceflag;
    }
}

void symtab_set_objtype(int index, int objtype)
{
    symtab[index].objtype = objtype;
}