#include <ctype.h>
#include <stdio.h>
#include <keywords.h>
#include <lexer.h>

char *tokens[] = {
    "id",
    "oct",
    "hex",
    "num",
    "asgn",
    "less",
    "greater",
    "leq",
    "geq",
    "neq",
    "equal",
    "type"
};

char lexeme[MAXIDLEN + 1];
int linenum = 1;

void clear_lexeme(int index);
void skip_spaces(FILE *tape);
int is_ID(FILE *tape);
int is_ASGN(FILE *tape);
int is_NUM(FILE *tape);
int isRELOP(FILE *tape);
int istoken(int token);

#pragma region Public Functions

/// @brief Gets the next token from the source file
/// @param source The source file to read from
/// @return The next token
int gettoken(FILE *source)
{
    int token = 0;

    skip_spaces(source);

    if (token = is_ASGN(source)) return token;
    if (token = isRELOP(source)) return token;
    if (token = is_ID(source)) return token;
    if (token = is_NUM(source)) return token;

    return token = getc(source);
}

char* get_token_to_string(int token)
{    
    if (is_token_keyword(token)) 
    {
        return keyword[token - BEGIN];
    }

    if (istoken(token))
    {
        return tokens[token - ID];
    }

    return NULL;
}


#pragma endregion

#pragma region Private Functions

/// @brief Clears the lexeme at the given index
/// @param index The index to clear
void clear_lexeme(int index)
{
    lexeme[index] = 0;
}

/// @brief Skips spaces and comments in the input tape
/// @param tape The input tape
void skip_spaces(FILE *tape)
{
    int head;
_skipspaces:
    while (isspace(head = getc(tape)))
    {
        if (head == '\n') linenum++;
    }

    if (head == '{' ) // if it is a comment
    {
        while ((head = getc(tape)) != '}' && head != EOF) // read until the end of the comment
        {
            if (head == '\n') linenum++;
        }
        
        goto _skipspaces; // skip spaces after the comment
    }

    ungetc(head, tape);
}

/// @brief Ungets all read characters
/// @param tape The input tape
/// @param index The index to unget to
void unget_all_read_characters(FILE *tape, int index)
{
    for (int i = index; i > -1; i--)
    {
        ungetc(lexeme[i], tape);
        clear_lexeme(i);
    }
}

/// @brief checks if the character is zero
/// @param character 
/// @return 1 if the character is zero, 0 otherwise
int is_zero(int character)
{
    return character == '0';
}

/// @brief Checks if the character is a dot
/// @param character The character to check
/// @return 1 if the character is a dot, 0 otherwise
int is_dot(int character)
{
    return character == '.';
}

/// @brief Checks if the character is an exponent indicator
/// @param character The character to check
/// @return 1 if the character is an exponent indicator, 0 otherwise
int is_exp_indicator(int character)
{
    return character == 'e' || character == 'E';
}


/// @brief Checks if the next token in the tape is an assignment
/// @param tape The input tape
/// @return ASGN if the next token is an assignment, 0 otherwise
int is_ASGN(FILE *tape)
{
    lexeme[0] = getc(tape);

    if (lexeme[0] != ':') //if the token does not start with a colon then it is not an assignment
    {
        //put the read characters back in the tape
        ungetc(lexeme[0], tape);
        clear_lexeme(0);
        return 0;
    }

    lexeme[1] = getc(tape);

    if (lexeme[1] != '=') //if the next character is not an equal sign then it is not an assignment
    {
        ungetc(lexeme[1], tape);
        clear_lexeme(1);

        ungetc(lexeme[0], tape);
        clear_lexeme(0);
        return 0;
    }
    return ASGN;
}

/// @brief identifies if next token in the tape is an ID
/// @param tape 
/// @return ID if the next token read is an ID, 0 otherwise
int is_ID(FILE *tape)
{
    int i = 0;
    lexeme[i] = getc(tape);

    if (!isalpha(lexeme[i]))     //if the token does not start with a letter then it is not an ID
    {
        ungetc(lexeme[i], tape); //put the token back in the tape
        clear_lexeme(i);
        return 0;           
    }

    i++;
    while (isalnum(lexeme[i] = getc(tape))) 
    { 
        if (i < MAXIDLEN) i++; //while the next caracter is an alphanumeic, reads the tape
    } 

    ungetc(lexeme[i], tape); //put the character that is not an alphanumeric back into the tape
    clear_lexeme(i);
    
    

    if (i = iskeyword(lexeme)) //verifies if the lexeme is a keyword
    {
        return i; // if it is a keyword then return the keyword token
    }

    return ID;
}

/// @brief Identifies if the next token in the tape is a number
/// using the following regular expression: 
/// (uint.[0−9]∗|′.′[0−9]+)([eE][′+′′−′]?[0−9]+)?|uint[eE][′+′′−′]?[0-9]+</para>
/// @param tape The input tape
/// @return NUM if the next token read is a number, 0 otherwise
int isNUM(FILE *tape)
{
    int i = 0;

    lexeme[i] = getc(tape);

    // a number can begin with a digit or a dot: 1 or .1
    if (!is_valid_start_of_num(lexeme[i])) // if the first char is not a digit or a dot then it is not a NUM
    {
        ungetc(lexeme[i], tape);
        clear_lexeme(i);
        return 0;
    }

    // if the first char is a dot then it must be followed by a digit: .1
    if (is_dot(lexeme[i]))
    {
        i++;
        if (!isdigit(lexeme[i] = getc(tape))) 
        {
            unget_all_read_characters(tape, i);
            return 0;
        }
        
    }

    // if the first char is a digit then it is a NUM
    if (isdigit(lexeme[i]))
    {
        i++;
        while (isdigit(lexeme[i] = getc(tape))) i++; //while the next caracter is a digit, reads the tape

        if (!is_dot(lexeme[i]) && !is_exp_indicator(lexeme[i])) // if the next char is not a dot or an exp indicator then it is a NUM
        {
            ungetc(lexeme[i], tape);
            clear_lexeme(i);
            return NUM;
        }
    }

    // if the read character is a dot then it is a NUM
    if (is_dot(lexeme[i]))
    {
        i++;
        if (!isdigit(lexeme[i] = getc(tape)) && !is_exp_indicator(lexeme[i]))// if the next char is not a digit or an exp indicator then the number sequence ended
        {
            ungetc(lexeme[i], tape);
            clear_lexeme(i);
            return NUM;
        }

        if (is_exp_indicator(lexeme[i])) goto _HANDLE_EXP;//verificar 1.e8

        i++;
        while (isdigit(lexeme[i] = getc(tape))) i++; // read the tape until the end of the digit sequence

        if (!is_exp_indicator(lexeme[i])) // if the next char is not an exp indicator then the number sequence ended
        {
            ungetc(lexeme[i], tape);
            clear_lexeme(i);
            return NUM;
        }
    }

// reads the exp indicator
_HANDLE_EXP:
        i++;
        lexeme[i] = getc(tape);
        if (!isdigit(lexeme[i]) && lexeme[i] != '+' && lexeme[i] != '-') // if the next char is not a digit or a sign then it is not a NUM
        {
            unget_all_read_characters(tape, i);
            return 0;
        }

        i++;
        while (isdigit(lexeme[i] = getc(tape))) i++; // read the tape until the end of the digit sequence
        
        ungetc(lexeme[i], tape);
        clear_lexeme(i);
        return NUM;
}


/// @brief Identifies if the next token in the tape is a relational operator
int isRELOP (FILE *tape)
{
    lexeme[0] = getc(tape);
    switch (lexeme[0]){
        case '<': // if the token starts with a less than sign. it can be: less or equal then, less than or not equal
            lexeme[1] = getc(tape);

            if (lexeme[1] == '=') return LEQ; // if the next char is an equal sign then it is a less or equal
            if (lexeme[1] == '>') return NEQ; // if the next char is a greater than sign then it is a not equal
 
            ungetc(lexeme[1], tape);
            return LESS;
        
        case '>': // if the token starts with a greater than sign. it can be: greater or equal then, greater than
            lexeme[1] = getc(tape);

            if (lexeme[1] == '='){ // if the next char is an equal sign then it is a greater or equal
                return GEQ;
            }

            ungetc(lexeme[1], tape);
            return GREATER;
        
        case '=':
            return EQUAL;
    }

    ungetc(lexeme[0], tape);
    return 0;
}

int istoken(int token)
{
    return token >= ID && token <= TYPE;
}
#pragma endregion