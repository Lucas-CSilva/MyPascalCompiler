#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>
#include <symtab.h>
#include <keywords.h>
#include <parser.h>

int lookahead;

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
}


void idlist(void)
{
_idlist:

    match(ID);
    if (lookahead == ',')
    {
        match(',');
        goto _idlist;
    }
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
    switch (lookahead)
    {
        case ID:
            match(ID);
            if (lookahead == ASGN)
            {
                match(ASGN);
                expr();
            }
            else if (lookahead == '(')
            {
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
    sbprgdef();
    beginend();
}

void vardef(void)
{
    if (lookahead == VAR){
        match(VAR);
_idlist:
        idlist();
        match(':');
        type();
        match(';');

        if (lookahead == ID)
            goto _idlist;
        
    }
}

void sbprgdef()
{
    int isfunc = 0;

    while (lookahead == PROCEDURE || lookahead == FUNCTION){
        isfunc = (lookahead == FUNCTION);
        match(lookahead);
        match(ID);
        parmlist();
        if (isfunc)
        {
            match(':');
            type();
        }
        match(';');
        block();
        match(';');
    }
}


void parmlist(void){
    if (lookahead == '('){
        match('(');
_parmlist:
        if (lookahead == VAR)
        {
            match(VAR);
        }

        idlist();

        match(':');

        type();

        if (lookahead == ';'){
            match(';');
            goto _parmlist;
        }

        match(')');
    }
}

void type(void){
    switch (lookahead)
    {
        case INTEGER:
        case REAL:
        case DOUBLE:
            match(lookahead);
            return;
    
        default:
            match(BOOLEAN);
            return;
    }
}

void match (int expected)
{
    if(lookahead == expected)
    {
        lookahead = gettoken(src);
    }
    else
    {
        fprintf(stderr, "\tSyntax error. expected: %d - recieved: %d in line: %d\n", expected, lookahead, linenum);
        exit(-1);
    }
}