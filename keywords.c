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

/// @brief Checks if a given word is a keyword.
/// @param word The word to check.
/// @return The keyword token if the word is a keyword, 0 otherwise.
int iskeyword(char *word)
{
    // Iterate through the range of keyword tokens from BEGIN to END
    for (int i = BEGIN; i <= END; i++){
        // If the word matches the current keyword
        if (strcmp(word, keyword[i - BEGIN]) == 0){
            // Return the keyword token
            return i;
        } 
    }

    // Return 0 if the word is not a keyword
    return 0;
}

int is_token_keyword(int token)
{
    return (token >= BEGIN && token <= END);
}