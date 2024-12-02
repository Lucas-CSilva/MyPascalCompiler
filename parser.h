
#pragma once

extern FILE *src;
extern int lookahead;

void program(void); // initial symbol
void block(void);
void vardef(void);
void sbprgdef(void);
void parmlist(void);
int idlist(void);
void beginend(void);
void stmtlist(void);
void stmt(void);
void idstmt(void);
int type(void);
void exprlist(void);
void match(int expected);
void ifstmt(void);
void whilestmt(void);
void repstmt(void);
void expr(void);
void smpexpr(void);
void termlist(void);
void factor(void);
void term(void);
