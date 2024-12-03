#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>
#include <symtab.h>
#include <keywords.h>
#include <parser.h>

int lookahead;
int lexlevel = 1;
int error_count = 0;

#define COLUMN_SIZE 20

void printsymtab()
{
    printf("\n\n================================================================================================================\n");
    
    printf("symtab_next_entry: %d\n", symtab_next_entry);
    
    printf("%-*s | %-*s | %-*s | %-*s | %-*s | %-*s\n", COLUMN_SIZE, "NAME" , COLUMN_SIZE, "OBJTYPE", COLUMN_SIZE, "TYPE", COLUMN_SIZE, "LEXLEVEL", COLUMN_SIZE, "PARMFLAG", COLUMN_SIZE, "REFFLAG");
    for (int i = 0; i < symtab_next_entry; i++){
        printf("%-*s | %-*d | %-*d | %-*d | %-*d | %-*d\n", COLUMN_SIZE, symtab[i].name, COLUMN_SIZE, symtab[i].objtype, COLUMN_SIZE, symtab[i].type, COLUMN_SIZE, symtab[i].lexlevel, COLUMN_SIZE, symtab[i].parmflag, COLUMN_SIZE, symtab[i].referenceflag);
    }
    printf("================================================================================================================\n\n");
}

void program(void)
{
    match(PROGRAM);
    match(ID);
    match('(');

    idlist();
        
    match(')');
    match(';');

    block();
    symtab_next_entry = 0;
    // symtab_next_entry = 0;
    printf("\n\n================================================================================================================\n");
    
    printf("symtab_next_entry: %d\n", symtab_next_entry);
    
    printf("%-*s | %-*s | %-*s | %-*s | %-*s | %-*s\n", COLUMN_SIZE, "NAME" , COLUMN_SIZE, "OBJTYPE", COLUMN_SIZE, "TYPE", COLUMN_SIZE, "LEXLEVEL", COLUMN_SIZE, "PARMFLAG", COLUMN_SIZE, "REFFLAG");
    for (int i = 0; i < symtab_next_entry; i++){
        printf("%-*s | %-*d | %-*d | %-*d | %-*d | %-*d\n", COLUMN_SIZE, symtab[i].name, COLUMN_SIZE, symtab[i].objtype, COLUMN_SIZE, symtab[i].type, COLUMN_SIZE, symtab[i].lexlevel, COLUMN_SIZE, symtab[i].parmflag, COLUMN_SIZE, symtab[i].referenceflag);
    }
    printf("================================================================================================================\n\n");
}


int idlist(void)
{
    int error_stat = 0;
    int first = symtab_next_entry;
_idlist:
    error_stat = symtab_append(lexeme, lexlevel);

    if (error_stat)
    {
        fprintf(stderr, "\tFATAL ERROR: symbol '%s' already defined\n", lexeme);
        error_count++;
    }

    match(ID);
    if (lookahead == ',')
    {
        match(',');
        goto _idlist;
    }

    return first;
}

//statment list
void stmtlist(void)
{
_stmtlst:
    stmt();
    if (lookahead == ';')
    {
        match(';');
        goto _stmtlst;
    }
}

void stmt(void)
{
    switch (lookahead)
    {
        case ID:
            idstmt();
            break;
        case BEGIN:
            beginend();
            break;
        case IF:
            ifstmt();
            break;
        case WHILE:
            whilestmt();
            break;
        case REPEAT:
            repstmt();
            break;
        default:
            break;
    }
}

void idstmt(void)
{
    int id_position = symtab_lookup(lexeme, lexlevel);
    if (id_position < 0)
    {
        fprintf(stderr, "\tFATAL ERROR: symbol '%s' not defined\n", lexeme);
        error_count++;
    }

    match(ID);
    if (lookahead == ASGN){
        match(ASGN);
        expr();
    }
    else{
        exprlist();
    }
}

void ifstmt(void){
    match(IF);
    expr();
    printf("\ttaqui, %s\n", lexeme);
    match(THEN);
    stmt();

    if (lookahead == ELSE){
        match(ELSE);
        stmt();
    }
}

void whilestmt(void){
    match(WHILE);
    expr();
    match(DO);
    stmt();
}

void repstmt(void){
    match(REPEAT);
    stmtlist();
    match(UNTIL);
    expr();
}

int isrelop(void)
{
    return lookahead == LESS
        || lookahead == GREATER
        || lookahead == LEQ
        || lookahead == GEQ
        || lookahead == NEQ
        || lookahead == EQUAL;
}

void expr(void){
    smpexpr();
    if (isrelop()){
        match(lookahead);
        smpexpr();
    }
}

void smpexpr(void){
    if (lookahead == '+' || lookahead == '-')
    {
        match(lookahead);
    }

    termlist();
}

void termlist(void)
{
    term();

    _termlist:
        if (lookahead == '+' || lookahead == '-' || lookahead == OR)
        {
            match(lookahead);
            term();

            goto _termlist;
        }
}

void term(void)
{
    factor();

    _factorlist:
        if (lookahead == '*' || lookahead == '/' || lookahead == DIV || lookahead == MOD || lookahead == AND)
        {
            match(lookahead);
            factor();
            goto _factorlist;
        }
}

void factor(void)
{
    int index_id = 0;

    switch (lookahead)
    {
        // case INTEGER:
        //     match(INTEGER);
        //     break;
        
        // case DOUBLE:
        //     match(DOUBLE);
        //     break;
        
        // case REAL:
        //     match(REAL);
        //     break;
        
        // case BOOLEAN:
        //     match(BOOLEAN);
        //     break;

        case ID:
            printf("\tlexeme antes do match %s - %d\n", lexeme, lookahead);
            index_id = symtab_lookup(lexeme, lexlevel);

            if (index_id == -1)
            {
                fprintf(stderr, "\tFATAL ERROR: symbol '%s' not defined\n", lexeme);
                error_count++;
            }

            match(ID);
            if (lookahead == ASGN)
            {
                match(ASGN);
                expr();
            }
            else if (lookahead == '(')
            {
                if (symtab[index_id].objtype != TYPE_FUNCTION)
                {
                    fprintf(stderr, "\tFATAL ERROR: expression preceding parenthesis of apperent call must have function type\n");
                    error_count++;
                }

                exprlist();
            }
            break;

        case INTEGER:
        case DOUBLE:
        case REAL:
        case BOOLEAN:
        // case ID:
        //     printf("\tlexeme antes do match %s - %d\n", lexeme, lookahead);
            match(lookahead);
            break;

        case FUNCTION:
            match(FUNCTION);
            exprlist();
            break;

        case NOT:
            match(NOT);
            factor();
            break;

        case NUM:
            match(NUM);
            break;
    }
}

void exprlist(void){
    if (lookahead == '('){
        match('(');
_exprlist:
        expr();
        if (lookahead == ','){
            match(',');
            goto _exprlist;
        }

        match(')');
    }
}

void beginend(void)
{
    match(BEGIN);
    stmtlist();
    match(END);
}

void block(void){
    vardef();
    int local_next_entry = symtab_next_entry;
    
    sbprgdef(&local_next_entry); // caso seja um subprograma o nome estará na tabela de simbolos, por isso é necesário incrementar o next_entry
    beginend();

    printsymtab();
    symtab_next_entry = local_next_entry;
    printsymtab();
}

void vardef(void)
{
    int ret_type, first;

    if (lookahead == VAR){
        match(VAR);
_idlist:
        first = idlist();
        match(':');
        ret_type = type();
        match(';');

        symtab_set_range_type(first, ret_type, TYPE_VARIABLE, 0, 0);

        if (lookahead == ID)
            goto _idlist;
        
    }
}

/// @brief Defines subprograms (procedures and functions).
/// @param caller_symtab_next_entry Pointer to the next entry in the caller's context.
void sbprgdef(int *caller_symtab_next_entry)
{
    /*0*/
    // Declare variables to store whether it's a function, the initial position in the symbol table, and the error status
    int isfunc = 0, local_init_position = 0, error_stat = 0;
    // Declare a variable to store the current identifier name
    char *current_id_name = NULL;
    /*0*/

    // While the current token is PROCEDURE or FUNCTION parse the subprogram
    while (lookahead == PROCEDURE || lookahead == FUNCTION){
        /*1*/
        // Set the next entry in the symbol table to the caller's next entry
        symtab_next_entry = *caller_symtab_next_entry;
        // Increment the caller's next entry
        (*caller_symtab_next_entry)++;
        /*1*/

        // Set the isfunc flag if the current token is FUNCTION
        isfunc = (lookahead == FUNCTION);

        // Match the current token (PROCEDURE or FUNCTION) and advance to the next token
        match(lookahead);

        /*0*/
        // Store the initial position in the symbol table
        local_init_position = symtab_next_entry;
        /*0*/

        /*1*/
        // Append the current lexeme to the symbol table and store the error status
        error_stat = symtab_append(lexeme, lexlevel);
        
        // If there is an error (symbol already defined)
        if (error_stat)
        {
            fprintf(stderr, "\tFATAL ERROR: symbol '%s' already defined\n", lexeme);
            error_count++;
        }
        /*1*/

        // Match the subprogram identifier and advance to the next token
        match(ID);

        // Parse the parameter list
        parmlist();

        if (isfunc)
        {
            /*2*/
            // Set the object type in the symbol table to TYPE_FUNCTION
            symtab_set_objtype(local_init_position, TYPE_FUNCTION);
            /*2*/

            match(':');
            type();
        }
        else{
            /*2*/
            // Set the object type in the symbol table to TYPE_PROCEDURE
            symtab_set_objtype(local_init_position, TYPE_PROCEDURE);
            /*2*/
        }
        
        match(';');

        /*3*/
        // Increment the lexical level before parsing the block
        lexlevel++;
        /*3*/

        // Parse the block of the subprogram
        block();

        // Match the semicolon and advance to the next token
        match(';');

        /*3*/
        // Decrement the lexical level after the block is parsed
        lexlevel--;
        /*3*/
    }
}

/// @brief Parses the parameter list of a function or procedure.
void parmlist(void){
    // Declare variables to store the return type, the initial position in the symbol table, and a flag for reference parameters
    /*0*/
    int parameters_type, local_init_position = 0, is_reference = 0;
    /*0*/

     // If the current token is an opening parenthesis
    if (lookahead == '('){
        match('(');
_parmlist:
        /*0*/
        // Reset the reference flag
        is_reference = 0;
        /*0*/

        // If the current token is the keyword VAR
        if (lookahead == VAR)
        {
            match(VAR);
            /*0*/
            // Set the reference flag to true
            is_reference = 1;
            /*0*/
        }

        /*1*/
        // Increment the lexical level, the id list needs to be in the next lexical level
        lexlevel++;
        // Parse the identifier list and store the initial position in the symbol table
        local_init_position = idlist();
        // Decrement the lexical level
        lexlevel--;
        /*1*/

        match(':');

        // Determine the type of the parameter
        /*0*/
        parameters_type = type();
        /*0*/

        // Set the range type in the symbol table for the parsed identifiers
        /*2*/
        symtab_set_range_type(local_init_position, parameters_type, TYPE_VARIABLE, 1, is_reference);
        /*2*/

        // If the current token is a semicolon
        if (lookahead == ';'){
            // Match the semicolon and advance to the next token
            match(';');
            // Go to the _parmlist label to parse more parameters
            goto _parmlist;
        }

        // Match the closing parenthesis and advance to the next token
        match(')');
    }
}

/// @brief Mathces the current type in the lookahead and advances to the next token.
/// @return The type in the lookahead.
int type(void){
    int current_type = lookahead;

    switch (current_type)
    {
        // If the type is INTEGER, REAL, or DOUBLE
        case INTEGER:
        case REAL:
        case DOUBLE:
            // Match the current type and advance to the next token
            match(lookahead);
            return current_type;
    
        default:
            // If the type is not INTEGER, REAL, or DOUBLE than it must be BOOLEAN and advance to the next token
            match(BOOLEAN);
            return current_type;
    }
}

/// @brief Matches the current token with the expected token and advances to the next token if they match.
/// @param expected The expected token to match.
void match (int expected)
{
    // If the current token matches the expected token
    if(lookahead == expected)
    {
         // Get the next token from the source
        lookahead = gettoken(src);
    }
    else
    {
        // Get the string representation of the expected token
        const char *expected_str = get_token_to_string(expected);
        // Get the string representation of the current token
        const char *lookahead_str = get_token_to_string(lookahead);

        // Buffers to hold single character tokens if they are not recognized
        char expected_buf[2];
        char lookahead_buf[2];

        // If the expected token string is NULL, convert the expected token to a string
        if (expected_str == NULL)
        {
            sprintf(expected_buf, "%c", expected);
            expected_str = expected_buf;
        }

        // If the current token string is NULL, convert the current token to a string
        if (lookahead_str == NULL)
        {
            sprintf(lookahead_buf, "%c", lookahead);
            lookahead_str = lookahead_buf;
        }

        // Print a syntax error message with the expected and received tokens and the line number
        fprintf(stderr, "\tSyntax error. expected: %s - recieved: %s in line: %d\n", expected_str, lookahead_str, linenum);
        exit(-1);
    }
}