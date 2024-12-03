
#pragma once

extern FILE *src;
extern int lookahead;
extern int error_count;

extern void program(void); // initial symbol
extern void block(void);
extern void vardef(void);
extern void sbprgdef(int *caller_symtab_next_entry);
extern void parmlist(void);
extern int idlist(void);
extern void beginend(void);
extern void stmtlist(void);
extern void stmt(void);
extern void idstmt(void);
extern int type(void);
extern void exprlist(void);
extern void match(int expected);
extern void ifstmt(void);
extern void whilestmt(void);
extern void repstmt(void);
extern void expr(void);
extern void smpexpr(void);
extern void termlist(void);
extern void factor(void);
extern void term(void);
