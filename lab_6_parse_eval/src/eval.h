#ifndef __EVAL_H
#define __EVAL_H

#include "parse.h"

typedef struct symbol_table_node
{
    char *ident;
    NUMBER value;
    struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;

SYMBOL_TABLE_NODE *findSymbol(SYMBOL_TABLE_NODE *table, char *ident); //used to see if symbol exists already. Returns Node or Null
SYMBOL_TABLE_NODE *createSymbol(char *ident, NUMBER value); //if symbol does not exist already, creats new symbol, returns a symbol node
void addSymbolToTable(SYMBOL_TABLE_NODE **table, SYMBOL_TABLE_NODE *newSymbol); //adds new symbol to symbol table
NUMBER evalSymbol(SYMBOL_TABLE_NODE *symbol); //returns the value of the symbol
void cleanUpSymbolTables(); //frees the symbol table

//First, see if the symbol is on the symbol table.
//  If symbol exists, update the value if eval symbol and value are different.
//  Else, generate new symbol
//     Then add symbol to table



void evalProgram(NODE *node);
void evalStatement(NODE *node);
void evalAssignStmt(NODE *node);
void evalRepeatStmt(NODE *node);
void evalPrintStmt(NODE *node);
NUMBER evalExpr(NODE *node);
NUMBER evalTerm(NODE *node);
NUMBER evalFactor(NODE *node);
NUMBER evalNumber(NODE *node);
NUMBER evalId(NODE *node);
NUMBER evalOperation(NUMBER operand1, NUMBER operand2, char op);

#endif //PARSER_EVAL_H
