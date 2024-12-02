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

void program(void)
{
    match(PROGRAM);
    match(ID);
    match('(');

    idlist();
        
    match(')');
    match(';');

    block();

    printf("\n\n================================================================================================================\n");
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
        fprintf(stderr, "\tFATAL ERROR: symbol already defined\n");
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
        fprintf(stderr, "\tFATAL ERROR: symbol not defined\n");
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
                fprintf(stderr, "\tFATAL ERROR: symbol not defined\n");
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
        
        case('('):
            match('(');
            expr();
            match(')');
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

    sbprgdef();
    beginend();
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

void sbprgdef(){
    int isfunc = 0, local_init_position = 0, error_stat = 0;
    char *current_id_name = NULL;

    while (lookahead == PROCEDURE || lookahead == FUNCTION){
        isfunc = (lookahead == FUNCTION);
        match(lookahead);
        local_init_position = symtab_next_entry;
        error_stat = symtab_append(lexeme, lexlevel);
        if (error_stat)
        {
            fprintf(stderr, "\tFATAL ERROR: symbol already exists\n");
            error_count++;
        }
        match(ID);
        parmlist();
        if (isfunc)
        {
            symtab_set_objtype(local_init_position, TYPE_FUNCTION);
            match(':');
            type();
        }
        else{
            symtab_set_objtype(local_init_position, TYPE_PROCEDURE);
        }
        match(';');
        lexlevel++;
        block();
        match(';');
        lexlevel--;
    }
}

//lexlevel + 1, não deve ser incrementado
void parmlist(void){
    int ret_type, local_init_position = 0, is_reference = 0;
    if (lookahead == '('){
        match('(');
_parmlist:
        is_reference = 0;
        if (lookahead == VAR)
        {
            match(VAR);
            is_reference = 1;
        }

        lexlevel++;
        local_init_position = idlist();
        lexlevel--;

        match(':');

        ret_type = type();

        symtab_set_range_type(local_init_position, ret_type, TYPE_VARIABLE, 1, is_reference);

        if (lookahead == ';'){
            match(';');
            goto _parmlist;
        }

        match(')');
    }
}

int type(void){
    int auxType = lookahead;

    switch (auxType)
    {
        case INTEGER:
        case REAL:
        case DOUBLE:
            match(lookahead);
            return auxType;
    
        default:
            match(BOOLEAN);
            return auxType;
    }

//     if (lookahead == TYPE){
//         match(TYPE);
// _typelist:
//         match(ID);
//         match('=');
//         match(TYPE);
//         match(';');

//         if (lookahead == ID)
//             goto _typelist;
//     }
}

void match (int expected){
    if(lookahead == expected){
        lookahead = gettoken(src);
    }
    else{
        fprintf(stderr, "\tSyntax error. expected: %d - recieved: %d in line: %d\n", expected, lookahead, linenum);
        exit(-1);
    }
}