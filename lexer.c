#include <ctype.h>
#include <stdio.h>
#include <keywords.h>
#include <lexer.h>

char lexeme[MAXIDLEN + 1];
int linenum = 0;

void clear_lexeme(int index);
void skip_spaces(FILE *p_tape);
int is_ID(FILE *p_tape);
int is_ASGN(FILE *p_tape);
int is_NUM(FILE *p_tape);
int isRELOP(FILE *p_tape);

#pragma region Public Functions
int gettoken(FILE *p_source)
{
    int token = 0;

    skip_spaces(p_source);

    if (token = is_ASGN(p_source)) return token;
    if (token = isRELOP(p_source)) return token;
    if (token = is_ID(p_source)) return token;
    if (token = is_NUM(p_source)) return token;

    return token = getc(p_source);
}
#pragma endregion

#pragma region Private Functions

void clear_lexeme(int index)
{
    lexeme[index] = 0;
}

void skip_spaces(FILE *p_tape)
{
    int head;
_skipspaces:
    while (isspace(head = getc(p_tape)))
    {
        if (head == '\n') linenum++;
    }

    if (head == '{' )
    {
        while ((head = getc(p_tape)) != '}' && head != EOF)
        {
            if (head == '\n') linenum++;
        }
        
        goto _skipspaces;
    }

    ungetc(head, p_tape);
}

void unget_all_read_characters(FILE *p_tape, int index)
{
    for (int i = index; i > -1; i--)
    {
        ungetc(lexeme[i], p_tape);
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

int is_dot(int character)
{
    return character == '.';
}

int is_exp_indicator(int character)
{
    return character == 'e' || character == 'E';
}

int is_ASGN(FILE *p_tape)
{
    lexeme[0] = getc(p_tape);

    if (lexeme[0] != ':')
    {
        ungetc(lexeme[0], p_tape);
        clear_lexeme(0);
        return 0;
    }

    lexeme[1] = getc(p_tape);

    if (lexeme[1] != '=')
    {
        ungetc(lexeme[1], p_tape);
        clear_lexeme(1);

        ungetc(lexeme[0], p_tape);
        clear_lexeme(0);
        return 0;
    }
    
    printf("\tASNG: %s\n", lexeme);
    return ASGN;
}

/// @brief identifies if next token in the tape is an ID
/// @param tape 
/// @return ID if the next token read is an ID, 0 otherwise
int is_ID(FILE *p_tape)
{
    int i = 0;
    lexeme[i] = getc(p_tape);

    if (!isalpha(lexeme[i]))     //if the token does not start with a letter then it is not an ID
    {
        ungetc(lexeme[i], p_tape); //put the token back in the tape
        clear_lexeme(i);
        return 0;           
    }

    i++;
    while (isalnum(lexeme[i] = getc(p_tape))) 
    { 
        if (i < MAXIDLEN) i++; //while the next caracter is an alphanumeic, reads the tape
    } 

    ungetc(lexeme[i], p_tape); //put the character that is not an alphanumeric back into the tape
    clear_lexeme(i);
    
    

    if (i = iskeyword(lexeme)) 
    {
        printf("\tlexem iskeyword: %s\n", lexeme);
        return i;
    }

    printf("\tlexem isID: %s\n", lexeme);
    return ID;
}

// (uint.[0−9]∗|′.′[0−9]+)([eE][′+′′−′]?[0−9]+)?|uint[eE][′+′′−′]?[0-9]+
int is_NUM(FILE *p_tape)
{
    int i = 0;

    lexeme[i] = getc(p_tape);
    if (!isdigit(lexeme[i]) && !is_dot(lexeme[i])) // if the first char is not a digit or a dot then it is not a NUM
    {
        ungetc(lexeme[i], p_tape);
        clear_lexeme(i);
        return 0;
    }
    
    if (is_dot(lexeme[i]))
    {
        i++;
        if (!isdigit(lexeme[i] = getc(p_tape)) && !is_exp_indicator(lexeme[i])) 
        {
            unget_all_read_characters(p_tape, i);
            clear_lexeme(i);
            return 0;
        }
    }

    if (isdigit(lexeme[i]))
    {
        i++;
        while (isdigit(lexeme[i] = getc(p_tape))) i++; //while the next caracter is a digit, reads the tape

        if (!is_dot(lexeme[i]) && !is_exp_indicator(lexeme[i])) // if the next char is not a dot or an exp indicator then it is a NUM
        {
            ungetc(lexeme[i], p_tape);
            clear_lexeme(i);
            return NUM;
        }
    }

    if (is_dot(lexeme[i]))
    {
        i++;
        if (!isdigit(lexeme[i] = getc(p_tape)) && !is_exp_indicator(lexeme[i])) 
        {
            ungetc(lexeme[i], p_tape);
            clear_lexeme(i);
            return NUM;
        }

        if (is_exp_indicator(lexeme[i])) goto _HANDLE_EXP;//verificar 1.e8

        i++;
        while (isdigit(lexeme[i] = getc(p_tape))) i++; // le ate encontrar um char diferente de digito

        if (!is_exp_indicator(lexeme[i])) 
        {
            ungetc(lexeme[i], p_tape);
            clear_lexeme(i);
            return NUM;
        }
    }

    // if (is_exp_indicator(lexeme[i]))
    // {

    // se chegou até aqui é pq é um numero com notação cientifica 
_HANDLE_EXP:
        i++;
        lexeme[i] = getc(p_tape);
        if (!isdigit(lexeme[i]) && lexeme[i] != '+' && lexeme[i] != '-') 
        {
            unget_all_read_characters(p_tape, i);
            return 0;
        }

        i++;
        while (isdigit(lexeme[i] = getc(p_tape))) i++; 
        
        ungetc(lexeme[i], p_tape);
        clear_lexeme(i);
        return NUM;
    // }
}

int isRELOP (FILE *tape)
{
    lexeme[0] = getc(tape);
    switch (lexeme[0]){
        case '<':
            lexeme[1] = getc(tape);

            if (lexeme[1] == '=') return LEQ;
            if (lexeme[1] == '>') return NEQ;
 
            ungetc(lexeme[1], tape);
            printf("\tlexem isrelop: %s\n", lexeme);
            return LESS;
        
        case '>':
            lexeme[1] = getc(tape);

            if (lexeme[1] == '='){
                printf("\tlexem isrelop: %s\n", lexeme);
                return GEQ;
            }

            ungetc(lexeme[1], tape);
            printf("\tlexem isrelop: %s\n", lexeme);
            return GREATER;
        
        case '=':
            printf("\tlexem isrelop: %s\n", lexeme);
            return EQUAL;
    }

    ungetc(lexeme[0], tape);
    return 0;
}
#pragma endregion