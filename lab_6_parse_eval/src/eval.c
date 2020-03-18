#include "eval.h"
#include "parse.h"
#include <math.h>

//For defaulting a stand by value
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
    char* ident = node->leftNode->data.identifier;
    NUMBER value = evalExpr(node->rightNode); //sends to eval expression to get the NUMBER return

    //find symbol if in table, otherwise is NULL
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
        //add currSymbol to table
        addSymbolToTable(&symbolTable, currSymbolNode);
    }
    //TODO - evalAssignStmt - Work
}

void evalRepeatStmt(NODE *node)
{
    // REPEATSTMT ::= repeat (expr) <statement>
    //value stores the number of times repeat will repeat the statement
    //must repeat the statement evaluation each repetition
    NUMBER value = evalExpr(node->leftNode);

    //cannot be float type
    if(value.type == INT_TYPE)
    {
        for(int i = 0; i < value.value.integer; i++)
        {
            evalStatement(node->rightNode);
        }
    } else

        error("Expected integer for repeat statement repetitions.");

    //TODO - EvalRepeat - Work
}

void evalPrintStmt(NODE *node)
{
    //print outputs an expression.
    //print function an evalExpr's value
    NUMBER value = evalExpr(node->leftNode);
    switch (value.type)
    {
        //addressing Integer
        case INT_TYPE:

            printf("INT : %ld", value.value.integer);

            break;

        case FLOAT_TYPE:

            printf("FLOAT : %f", value.value.floating_point);

            break;

        default:

            error("Expected integer or float value in expression");

            break;
    }
    //TODO - evalPrint - Work
}

NUMBER evalExpr(NODE *node)
{
//Can have one or two nodes.
//If leftNode only
    //evaluate term and return
//else
    //evaluate term <addop> expression
//assumes the eval operations handle any errors
    NUMBER term = evalTerm(node->leftNode);
    NUMBER value = DEFAULT_NUMBER;

    if(node->rightNode != NULL)
    {
        //if right node is not NULL we have an expression.
        NUMBER expr = evalExpr(node->rightNode);
        //Value is the result of the addop and the term/expr
        switch (node->data.op)
        {
            case '+':
            case '-':
                value = evalOperation(term, expr, node->data.op);
                break;
            default:
                error("Expected operator to be +|- in expression.");
                break;
        }

    } else
        //otherwise value is only the term
        value = term;

    return value;
    //TODO - EvalExpression - Work
}

NUMBER evalTerm(NODE *node)
{
    //TERM ::= factor | factor <multop> term
    NUMBER factor = evalFactor(node->leftNode);
    NUMBER value = DEFAULT_NUMBER;

    if(node->rightNode != NULL)
    {
        NUMBER term = evalTerm(node->rightNode);
        //evaluate operation of node's operator with factor|term
        switch (node->data.op)
        {
            case '*':
            case '/':
            case '%':
                value = evalOperation(factor, term, node->data.op);
                break;
            default:
                error("Expected operator to be *|/|% in term");
                break;

        }
    } else

        value = factor;

    return value;
    //TODO - EvalTerm -
}

NUMBER evalFactor(NODE *node)
{
    //FACTOR ::= ID | number | <addop> factor | expr
    // switch case to identify what path to follow. Indicator is node type.

    NUMBER value = DEFAULT_NUMBER;

    switch(node->type)
    {
        case IDENT_NODE:

            value = evalId(node->leftNode);

            break;

        case NUMBER_NODE:

            value = evalNumber(node->leftNode);

            break;

        case FACTOR_NODE:
            //use default number to allow operation to apply to only the available left node.
            value = evalOperation(value, evalFactor(node->leftNode), node->data.op);

            break;

        case EXPR_NODE:

            value = evalExpr(node->leftNode);

            break;

        default:

            error("Expected an ID|number|<addOp>factor|expr in a factor.");

            break;
    }

    return value;
    //TODO - EvalFactor -
}

NUMBER evalNumber(NODE *node)
{
    //NUMBER ::= int|float
    //Switch case to address the number type.

    NUMBER value = DEFAULT_NUMBER;

    if(node != NULL)
    {
        switch (node->data.number.type)
        {
            case INT_TYPE:

                value.type = node->data.number.type;

                value.value.integer = node->data.number.value.integer;

                break;

            case FLOAT_TYPE:

                value.type = node->data.number.type;

                value.value.floating_point = node->data.number.value.floating_point;

                break;

            default:

                error("Expected integer or a float in Number.");

                break;
        }
    }

    return value;
    //TODO - EvalNumber
}

NUMBER evalId(NODE *node)
{
    //can use findSymbol to see if the identity is in the table.

    char* ident = node->data.identifier;

    SYMBOL_TABLE_NODE *currSymbolNode = findSymbol(symbolTable, ident);

    if(currSymbolNode == NULL)
    {

        currSymbolNode = createSymbol(ident, DEFAULT_NUMBER);

        addSymbolToTable(&symbolTable, currSymbolNode);

    }

    return evalSymbol(currSymbolNode);

    //TODO - EvalID - Work
}

NUMBER evalAdd(NUMBER op1, NUMBER op2)
{
//int and float types can be present, therefore we need to address all versions that can pass through.
//4 permutations for total cases
    NUMBER result = DEFAULT_NUMBER;

    switch(op1.type)
    {
        case INT_TYPE:

            switch(op2.type)
            {
                case INT_TYPE:

                    result.type = INT_TYPE;
                    result.value.integer = op1.value.integer + op2.value.integer;

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.integer + op2.value.floating_point;

                    break;
            }

            break;
        case FLOAT_TYPE:
            //All result types will be float
            switch(op2.type)
            {
                case INT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.floating_point + op2.value.integer;

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.floating_point + op2.value.floating_point;

                    break;

            }

            break;
        default:
            break;
    }

    return result;
    //TODO - evalAdd - Work
}

NUMBER evalSub(NUMBER op1, NUMBER op2)
{

    NUMBER result = DEFAULT_NUMBER;

    switch (op1.type)
    {
        case INT_TYPE:

            switch (op2.type)
            {
                case INT_TYPE:

                    result.type = INT_TYPE;
                    result.value.integer = op1.value.integer - op2.value.integer;

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.integer - op2.value.floating_point;

                    break;
            }

            break;
        case FLOAT_TYPE:

            switch (op2.type)
            {
                case INT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.floating_point - op2.value.integer;

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.floating_point - op2.value.floating_point;

                    break;
            }

            break;
    }

    return result;
    //TODO - evalSub
}

NUMBER evalMult(NUMBER op1, NUMBER op2)
{
    NUMBER result = DEFAULT_NUMBER;

    switch (op1.type)
    {
        case INT_TYPE:

            switch (op2.type)
            {
                case INT_TYPE:

                    result.type = INT_TYPE;
                    result.value.integer = op1.value.integer * op2.value.integer;

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.integer * op2.value.floating_point;

                    break;
            }

            break;
        case FLOAT_TYPE:

            switch (op2.type)
            {
                case INT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.floating_point * op2.value.integer;

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.floating_point * op2.value.floating_point;

                    break;
            }

            break;
    }

    return result;
    //TODO
}

NUMBER evalDiv(NUMBER op1, NUMBER op2)
{
    NUMBER result = DEFAULT_NUMBER;

    switch (op1.type)
    {
        case INT_TYPE:

            switch (op2.type)
            {
                case INT_TYPE:

                    result.type = INT_TYPE;
                    result.value.integer = op1.value.integer / op2.value.integer;

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.integer / op2.value.floating_point;

                    break;
            }

            break;
        case FLOAT_TYPE:

            switch (op2.type)
            {
                case INT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.floating_point / op2.value.integer;

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = op1.value.floating_point / op2.value.floating_point;

                    break;
            }

            break;
    }

    return result;
    //TODO
}

NUMBER evalMod(NUMBER op1, NUMBER op2)
{
    NUMBER result = DEFAULT_NUMBER;

    switch (op1.type)
    {
        case INT_TYPE:

            switch (op2.type)
            {
                case INT_TYPE:

                    result.type = INT_TYPE;
                    result.value.integer = op1.value.integer % op2.value.integer;

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = fmod((double)(op1.value.integer), op2.value.floating_point);

                    break;
            }

            break;
        case FLOAT_TYPE:

            switch (op2.type)
            {
                case INT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = fmod(op1.value.floating_point, (double)(op2.value.integer));

                    break;
                case FLOAT_TYPE:

                    result.type = FLOAT_TYPE;
                    result.value.floating_point = fmod(op1.value.floating_point, op2.value.floating_point);

                    break;
            }

            break;
    }

    return result;
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