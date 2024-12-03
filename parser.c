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
    //printsymtab();
}


/// @brief Parses a list of identifiers.
/// @return The index of the first identifier in the symbol table.
int idlist(void)
{
    /*0*/
    // Declare a variable to store the error status
    int error_stat = 0;
    /*0*/

    /*1*/
    // Store the current value of symtab_next_entry as the first identifier position
    int first_symtab_entry = symtab_next_entry;
    /*1*/

_idlist:
    /*2*/
    // Append the current lexeme to the symbol table and store the error status
    error_stat = symtab_append(lexeme, lexlevel);

    // If there is an error (symbol already defined)
    if (error_stat)
    {
        // Print a fatal error message with the symbol name
        fprintf(stderr, "\tFATAL ERROR(ln: %d): symbol '%s' already defined\n", linenum, lexeme);
        // Increment the error count
        error_count++;
    }
    /*2*/

    // Match the identifier and advance to the next token
    match(ID);
    // If the current token is a comma
    if (lookahead == ',')
    {
        // Match the comma and advance to the next token
        match(',');
        // Go to the _idlist label to parse more identifiers
        goto _idlist;
    }

    // Return the index of the first identifier in the symbol table
    return first_symtab_entry;
}

/// @brief Parses a list of statements.
void stmtlist(void)
{
_stmtlst:
    // Parse a statement
    stmt();
    // If the current token is a semicolon
    if (lookahead == ';')
    {
        // Match the semicolon and advance to the next token
        match(';');
        // Go to the _stmtlst label to parse more statements
        goto _stmtlst;
    }
}

/// @brief Parses a statement.
void stmt(void)
{
    // Switch based on the current token
    switch (lookahead)
    {
        case ID:
            // Parse an identifier statement (assignment or function/procedure call)
            idstmt();
            break;
        case BEGIN:
            // Parse a begin-end block
            beginend();
            break;
        case IF:
            // Parse an if statement
            ifstmt();
            break;
        case WHILE:
            // Parse a while statement
            whilestmt();
            break;
        case REPEAT:
            // Parse a repeat statement
            repstmt();
            break;
        default:
            // Do nothing for unrecognized statements
            break;
    }
}

/// @brief Parses an identifier statement (assignment or function/procedure call).
void idstmt(void)
{
    /*0*/
    // Look up the identifier in the symbol table and store its position
    int id_position = symtab_lookup(lexeme, lexlevel);
    /*0*/

    /*1*/
    // If the identifier is not found in the symbol table
    if (id_position < 0)
    {
        // Print a fatal error message with the symbol name
        fprintf(stderr, "\tFATAL ERROR(ln: %d): symbol '%s' not defined\n", linenum, lexeme);
        // Increment the error count
        error_count++;
    }
    /*1*/

    // Match the identifier and advance to the next token
    match(ID);
    // If the current token is an assignment operator
    if (lookahead == ASGN){
        // Match the assignment operator and advance to the next token
        match(ASGN);
        // Parse the expression
        expr();
    }
    // Otherwise, assume it's a function/procedure call
    else{
        // Parse the list of expressions
        exprlist();
    }
}

/// @brief Parses an if statement.
void ifstmt(void){
    // Match the IF keyword and advance to the next token
    match(IF);
    // Parse the expression
    expr();
    // Match the THEN keyword and advance to the next token
    match(THEN);
    // Parse the statement
    stmt();

    // If the current token is ELSE
    if (lookahead == ELSE){
        // Match the ELSE keyword and advance to the next token
        match(ELSE);
        // Parse the statement
        stmt();
    }
}

/// @brief Parses a while statement.
void whilestmt(void){
    // Match the WHILE keyword and advance to the next token
    match(WHILE);
    // Parse the expression
    expr();
    // Match the DO keyword and advance to the next token
    match(DO);
    // Parse the statement
    stmt();
}

/// @brief Parses a repeat statement.
void repstmt(void){
    // Match the REPEAT keyword and advance to the next token
    match(REPEAT);
    // Parse the list of statements
    stmtlist();
    // Match the UNTIL keyword and advance to the next token
    match(UNTIL);
    // Parse the expression
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

/// @brief Parses an expression.
void expr(void){
    // Parse a simple expression
    smpexpr();
    // If the current token is a relational operator
    if (isrelop()){
        // Match the relational operator and advance to the next token
        match(lookahead);
        // Parse another simple expression
        smpexpr();
    }
}

/// @brief Parses a simple expression.
void smpexpr(void){
    // If the current token is a plus or minus operator
    if (lookahead == '+' || lookahead == '-')
    {
        // Match the operator and advance to the next token
        match(lookahead);
    }

    // Parse the list of terms
    termlist();
}

/// @brief Parses a list of terms in an expression.
void termlist(void)
{
    // Parse a term
    term();

_termlist:
    // If the current token is a plus, minus, or OR operator
    if (lookahead == '+' || lookahead == '-' || lookahead == OR)
    {
        // Match the operator and advance to the next token
        match(lookahead);
        // Parse another term
        term();
        // Go to the _termlist label to parse more terms
        goto _termlist;
    }
}

/// @brief Parses a term in an expression.
void term(void)
{
    // Parse a factor
    factor();

_factorlist:
    // If the current token is a multiplication, division, DIV, MOD, or AND operator
    if (lookahead == '*' || lookahead == '/' || lookahead == DIV || lookahead == MOD || lookahead == AND)
    {
        // Match the operator and advance to the next token
        match(lookahead);
        // Parse another factor
        factor();
        // Go to the _factorlist label to parse more factors
        goto _factorlist;
    }
}

/// @brief Parses a factor in an expression.
void factor(void)
{
    /*0*/
    // Declare a variable to store the index of the identifier in the symbol table
    int index_identifier = 0;
    /*0*/

    switch (lookahead)
    {
        case ID:
            
            /*1*/
            // Look up the identifier in the symbol table
            index_identifier = symtab_lookup(lexeme, lexlevel);
            
            // If the identifier is not found in the symbol table
            if (index_identifier == -1)
            {
                // Print a fatal error message with the symbol name
                fprintf(stderr, "\tFATAL ERROR(ln: %d): symbol '%s' not defined\n", linenum, lexeme);
                
                // Increment the error count
                error_count++;
            }
            /*1*/

            // Match the identifier and advance to the next token
            match(ID);

            // If the current token is an assignment operator
            if (lookahead == ASGN)
            {
                // Match the assignment operator and advance to the next token
                match(ASGN);
                // Parse the expression
                expr();
            }
            // If the current token is an opening parenthesis
            else if (lookahead == '(')
            {
                /*1*/
                // If the object type of the identifier is not a function than the identifier is not callable
                if (symtab[index_identifier].objtype != TYPE_FUNCTION)
                {
                    // Print a fatal error message
                    fprintf(stderr, "\tFATAL ERROR(ln: %d): expression preceding parenthesis of apparent call must have function type\n", linenum);
                    // Increment the error count
                    error_count++;
                }
                /*1*/
                
                // Parse the list of expressions
                exprlist();
            }
            break;

        case INTEGER:
        case DOUBLE:
        case REAL:
        case BOOLEAN:
            match(lookahead);
            break;

        case FUNCTION:
            // Match the FUNCTION keyword and advance to the next token
            match(FUNCTION);
            // Parse the list of expressions
            exprlist();
            break;

        case NOT:
            // Match the NOT keyword and advance to the next token
            match(NOT);
            // Parse the factor
            factor();
            break;

        case NUM:
            match(NUM);
            break;
    }
}

/// @brief Parses a list of expressions.
void exprlist(void){
    // If the current token is an opening parenthesis
    if (lookahead == '('){
        match('(');
_exprlist:
        // Parse an expression
        expr();

        // If the current token is a comma
        if (lookahead == ','){
            
            match(',');
            
            // Go to the _exprlist label to parse more expressions
            goto _exprlist;
        }

        // Match the closing parenthesis and advance to the next token
        match(')');
    }
}

/// @brief Parses a begin-end block.
void beginend(void)
{
    // Match the BEGIN keyword and advance to the next token
    match(BEGIN);
    // Parse the list of statements
    stmtlist();
    // Match the END keyword and advance to the next token
    match(END);
}

/// @brief Parses a block of code, including variable definitions, subprogram definitions, and a begin-end block.
void block(void){
    // Parse variable definitions
    vardef();

    /*0*/
    // Store the current value of symtab_next_entry in a local variable
    int local_next_entry = symtab_next_entry;

    // Parse subprogram definitions and update the local_next_entry
    sbprgdef(&local_next_entry); // If it's a subprogram, the name will be in the symbol table, so it's necessary to increment next_entry
    /*0*/
    
    // Parse the begin-end block
    beginend();

    //printsymtab();

    /*0*/
    // Update symtab_next_entry with the local_next_entry value. Remove the tokens read in the subprogram symbol from the table
    symtab_next_entry = local_next_entry;
    /*0*/

    //printsymtab();
}

/// @brief Parses variable definitions.
void vardef(void)
{
    /*0*/
    // Declare variables to store the return type and the first identifier position in the symbol table
    int ret_type = 0, first_symtab_entry = 0;
    /*0*/

    if (lookahead == VAR){
        match(VAR);
_idlist:
        /*1*/
        // Parse the identifier list and store the first identifier position in the symbol table
        first_symtab_entry = idlist();
        /*1*/

        // Match the colon and advance to the next token
        match(':');
        
        /*1*/
        // Determine the type of the variables
        ret_type = type();
        /*1*/
        
        // Match the semicolon and advance to the next token
        match(';');

        /*2*/
        // Set the range type in the symbol table for the parsed identifiers
        symtab_set_range_type(first_symtab_entry, ret_type, TYPE_VARIABLE, 0, 0);
        /*2*/

        // If the current token is an identifier
        if (lookahead == ID)
        {
            // Go to the _idlist label to parse more identifiers
            goto _idlist;
        }
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
            fprintf(stderr, "\tFATAL ERROR(ln: %d): symbol '%s' already defined\n", linenum, lexeme);
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