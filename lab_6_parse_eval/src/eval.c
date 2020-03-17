#include "eval.h"
#include "parse.h"
#include <math.h>

#define DEFAULT_NUMBER (NUMBER){INT_TYPE, 0}

SYMBOL_TABLE_NODE *symbolTable = NULL;

void evalProgram(NODE *node)
{
    if (node == NULL)
    {
        return;
    }

    if (node->leftNode != NULL)
    {
        evalStatement(node->leftNode);
    }

    if (node->rightNode != NULL)
    {
        evalProgram(node->rightNode);
    }
}

void evalStatement(NODE *node)
{
    if (node->leftNode != NULL)
    {
        switch (node->leftNode->type)
        {
            case ASSIGN_STMT_NODE:
                evalAssignStmt(node->leftNode);
                break;
            case REPEAT_STMT_NODE:
                evalRepeatStmt(node->leftNode);
                break;
            case PRINT_STMT_NODE:
                evalPrintStmt(node->leftNode);
                break;
            default:
                error("Invalid child of type %d in statement node.", node->leftNode->type);
        }
    }
}

void evalAssignStmt(NODE *node)
{
    //assign ::= ID = EXPR
   //need to address a value of NUMBER and an ident char* in findSymbol and createSymbol
   char* ident = node->data.identifier;
   NUMBER value = evalExpr(node->rightNode);

   //try to find symbol
   SYMBOL_TABLE_NODE *currSymbolNode = findSymbol(symbolTable,ident);

   //if In Symbol Tree
   if(currSymbolNode != NULL)
   {
       //if exists, only need to update symbol value
        currSymbolNode->value = node->data.number;
   }
   else
       {
            //currSymbolNode is Null, needs space & data
            currSymbolNode = createSymbol(ident,value);
            addSymbolToTable(&symbolTable, currSymbolNode);
       }
   //TODO - evalAssignStmt - Work
}

void evalRepeatStmt(NODE *node)
{
    //TODO
}

void evalPrintStmt(NODE *node)
{
   //TODO
}

NUMBER evalExpr(NODE *node)
{
   //TODO
}

NUMBER evalTerm(NODE *node)
{
   //TODO
}

NUMBER evalFactor(NODE *node)
{
   //TODO
}

NUMBER evalNumber(NODE *node)
{
    //TODO
}

NUMBER evalId(NODE *node)
{
   //TODO
}

NUMBER evalAdd(NUMBER op1, NUMBER op2)
{
    //TODO
}

NUMBER evalSub(NUMBER op1, NUMBER op2)
{
   //TODO
}

NUMBER evalMult(NUMBER op1, NUMBER op2)
{
  //TODO
}

NUMBER evalDiv(NUMBER op1, NUMBER op2)
{
   //TODO
}

NUMBER evalMod(NUMBER op1, NUMBER op2)
{
    //TODO
}

NUMBER evalOperation(NUMBER operand1, NUMBER operand2, char op)
{
    //TODO
}

SYMBOL_TABLE_NODE *findSymbol(SYMBOL_TABLE_NODE *table, char *ident)
{
    while (table != NULL)
    {
        if (strcmp(ident, table->ident) == 0)
        {
            return table;
        }
        table = table->next;
    }
    return NULL;
}

SYMBOL_TABLE_NODE *createSymbol(char *ident, NUMBER value)
{
    SYMBOL_TABLE_NODE *newSymbol = calloc(sizeof(SYMBOL_TABLE_NODE), 1);
    newSymbol->ident = calloc(sizeof(char), strlen(ident)+1);
    strcpy(newSymbol->ident, ident);
    newSymbol->value = value;
    return newSymbol;
}

void addSymbolToTable(SYMBOL_TABLE_NODE **table, SYMBOL_TABLE_NODE *newSymbol)
{
    newSymbol->next = *table;
    *table = newSymbol;
}

NUMBER evalSymbol(SYMBOL_TABLE_NODE *symbol)
{
    return symbol->value;
}

void freeSymbolTable(SYMBOL_TABLE_NODE **table)
{
    if (table == NULL)
    {
        return;
    }
    if (*table == NULL)
    {
        return;
    }

    freeSymbolTable(&(*table)->next);
    free((*table)->ident);
    free(*table);
    *table = NULL;
}

void cleanUpSymbolTables()
{
    freeSymbolTable(&symbolTable);
}
