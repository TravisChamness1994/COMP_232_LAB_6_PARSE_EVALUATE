#include "eval.h"
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
    NUMBER value = evalExpr(node->rightNode);
    char *ident = node->leftNode->data.identifier;

    SYMBOL_TABLE_NODE *oldSymbol = findSymbol(symbolTable, ident);

    if (oldSymbol == NULL)
    {
        SYMBOL_TABLE_NODE *newSymbol = createSymbol(ident, value);
        addSymbolToTable(&symbolTable, newSymbol);
    }
    else
    {
        oldSymbol->value = value;
    }
}

void evalRepeatStmt(NODE *node)
{
    NUMBER numberOfRepeats = evalExpr(node->leftNode);

    if (numberOfRepeats.type != INT_TYPE)
    {
        error("Repeat called with non-integer number of repeats!");
    }

    for (int i = 0; i < numberOfRepeats.value.integer; ++i)
    {
        evalStatement(node->rightNode);
    }
}

void evalPrintStmt(NODE *node)
{
    NUMBER toPrint = evalExpr(node->leftNode);
    switch(toPrint.type)
    {
        case INT_TYPE:
            printf("\nINT : %ld", toPrint.value.integer);
            break;
        case FLOAT_TYPE:
            printf("\nFLOAT : %lf", toPrint.value.floating_point);
            break;
    }
}

NUMBER evalExpr(NODE *node)
{
    NUMBER val = evalTerm(node->leftNode);
    if (node->rightNode != NULL)
    {
        NUMBER rightVal = evalExpr(node->rightNode);
        char op = node->data.op;
        val = evalOperation(val, rightVal, op);
    }
    return val;
}

NUMBER evalTerm(NODE *node)
{
    NUMBER val = evalFactor(node->leftNode);
    if (node->rightNode != NULL)
    {
        NUMBER rightVal = evalTerm(node->rightNode);
        char op = node->data.op;
        val = evalOperation(val, rightVal, op);
    }
    return val;
}

NUMBER evalFactor(NODE *node)
{
    NUMBER val = DEFAULT_NUMBER;
    switch(node->leftNode->type)
    {
        case FACTOR_NODE:
            val = evalOperation(val, evalFactor(node->leftNode), node->data.op);
            break;
        case NUMBER_NODE:
            val = evalNumber(node->leftNode);
            break;
        case IDENT_NODE:
            val = evalId(node->leftNode);
            break;
        case EXPR_NODE:
            val = evalExpr(node->leftNode);
            break;
        default:
            error("Invalid child type %d for factor node.", node->leftNode->type);
    }
    return val;
}

NUMBER evalNumber(NODE *node)
{
    return node->data.number;
}

NUMBER evalId(NODE *node)
{
    char *ident = node->data.identifier;

    SYMBOL_TABLE_NODE *symbol = findSymbol(symbolTable, ident);

    if (symbol == NULL)
    {
        error("Undefined symbol %s", ident);
        return DEFAULT_NUMBER;
    }

    return evalSymbol(symbol);
}

NUMBER evalAdd(NUMBER op1, NUMBER op2)
{
    NUMBER result = DEFAULT_NUMBER;
    if (op1.type == INT_TYPE)
    {
        if (op2.type == INT_TYPE)
        {
            result.type = INT_TYPE;
            result.value.integer = op1.value.integer + op2.value.integer;
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = (double)op1.value.integer + op2.value.floating_point;
        }
    }
    else
    {
        if (op2.type == INT_TYPE)
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = op1.value.floating_point + (double)op2.value.integer;
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = op1.value.floating_point + op2.value.floating_point;
        }
    }
    return result;
}

NUMBER evalSub(NUMBER op1, NUMBER op2)
{
    NUMBER result = DEFAULT_NUMBER;
    if (op1.type == INT_TYPE)
    {
        if (op2.type == INT_TYPE)
        {
            result.type = INT_TYPE;
            result.value.integer = op1.value.integer - op2.value.integer;
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = (double)op1.value.integer - op2.value.floating_point;
        }
    }
    else
    {
        if (op2.type == INT_TYPE)
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = op1.value.floating_point - (double)op2.value.integer;
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = op1.value.floating_point - op2.value.floating_point;
        }
    }
    return result;
}

NUMBER evalMult(NUMBER op1, NUMBER op2)
{
    NUMBER result = DEFAULT_NUMBER;
    if (op1.type == INT_TYPE)
    {
        if (op2.type == INT_TYPE)
        {
            result.type = INT_TYPE;
            result.value.integer = op1.value.integer * op2.value.integer;
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = (double)op1.value.integer * op2.value.floating_point;
        }
    }
    else
    {
        if (op2.type == INT_TYPE)
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = op1.value.floating_point * (double)op2.value.integer;
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = op1.value.floating_point * op2.value.floating_point;
        }
    }
    return result;
}

NUMBER evalDiv(NUMBER op1, NUMBER op2)
{
    NUMBER result = DEFAULT_NUMBER;
    if (op1.type == INT_TYPE)
    {
        if (op2.type == INT_TYPE)
        {
            result.type = INT_TYPE;
            result.value.integer = op1.value.integer / op2.value.integer;
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = (double)op1.value.integer / op2.value.floating_point;
        }
    }
    else
    {
        if (op2.type == INT_TYPE)
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = op1.value.floating_point / (double)op2.value.integer;
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = op1.value.floating_point / op2.value.floating_point;
        }
    }
    return result;
}

NUMBER evalMod(NUMBER op1, NUMBER op2)
{
    NUMBER result = DEFAULT_NUMBER;
    if (op1.type == INT_TYPE)
    {
        if (op2.type == INT_TYPE)
        {
            result.type = INT_TYPE;
            result.value.integer = op1.value.integer % op2.value.integer;
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = fmod((double)op1.value.integer, op2.value.floating_point);
        }
    }
    else
    {
        if (op2.type == INT_TYPE)
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = fmod(op1.value.floating_point, (double)op2.value.integer);
        }
        else
        {
            result.type = FLOAT_TYPE;
            result.value.floating_point = fmod(op1.value.floating_point, op2.value.floating_point);
        }
    }
    return result;
}

NUMBER evalOperation(NUMBER operand1, NUMBER operand2, char op)
{
    switch (op)
    {
        case '+':
            return evalAdd(operand1, operand2);
        case '-':
            return evalSub(operand1, operand2);
        case '*':
            return evalMult(operand1, operand2);
        case '/':
            return evalDiv(operand1, operand2);
        case '%':
            return evalMod(operand1, operand2);
        default:
            error("Invalid operation character %c.", op);
    }
    return DEFAULT_NUMBER; // unreachable
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
