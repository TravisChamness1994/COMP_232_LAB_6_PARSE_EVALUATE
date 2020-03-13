#include "print.h"

void printProgram(NODE *node, int indent)
{
    if (node == NULL)
    {
        return;
    }

    if (node->leftNode != NULL)
    {
        printStatement(node->leftNode, indent);
    }
    if (node->rightNode != NULL)
    {
        printProgram(node->rightNode, indent);
    }
}

void printStatement(NODE *node, int indent)
{
    printfIndented(indent, "START STATEMENT");

    if (node->leftNode != NULL)
    {
        switch (node->leftNode->type)
        {
            case ASSIGN_STMT_NODE:
                printAssignStmt(node->leftNode, indent + 1);
                break;
            case REPEAT_STMT_NODE:
                printRepeatStmt(node->leftNode, indent + 1);
                break;
            case PRINT_STMT_NODE:
                printPrintStmt(node->leftNode, indent + 1);
                break;
            default:
                error("Child of statement node has invalid type %d, cannot print.", node->leftNode->type);
                break;
        }
    }

    printfIndented(indent, "END STATEMENT");
}

void printAssignStmt(NODE *node, int indent)
{
    printfIndented(indent, "START ASSIGN STATEMENT");

    if(node->leftNode != NULL)
    {
        switch (node->leftNode->type)
        {
            case IDENT_NODE:
                    printId(node->leftNode, indent + 1);
                break;
            default:
                error("Expected ID in assignment statement.");
                break;
        }
    }
    else
            error("Expected ID in assignment Statement.");

    if(node->rightNode != NULL)
    {
        switch (node->rightNode->type)
        {
            case EXPR_NODE:
                printExpr(node->rightNode, indent + 1);
                break;
            default:
                error("Expected an expression in an assignment statement.");
                break;
        }
    }
    else
        error("Expected an expression in an assignment statement");

    printfIndented(indent ,"END ASSIGN STATEMENT");
    // TODO - PrintAssignStmt - Work
}

void printRepeatStmt(NODE *node, int indent)
{
    printfIndented(indent, "START REPEAT STATEMENT");

    if(node->leftNode != NULL)
    {
        switch (node->leftNode->type)
        {
            case EXPR_NODE:
                printRepeatStmt(node->leftNode, indent + 1);
                break;
            default:
                //If the left node is populated, but is not the repeat node, we throw an error
                error("Expected an expression in repeat statement.");
                break; //break is not necessary
        }
    } else
        //if the left node is not populated, we throw an error. However, will this ever happen?
        error("Expected an expression in repeat statement.");

    if(node->rightNode != NULL)
    {
        switch (node->rightNode->type)
        {
            case STATEMENT_NODE:
                printStatement(node->rightNode, indent + 1);
                break;
            default:
                error("Expected a statement in the repeat statment.");
                break;
        }
    }
    else
        error("Expected a statement in the repeat statement");
    // TODO - PrintRepeatStmt - Work
}

void printPrintStmt(NODE *node, int indent)
{
    // TODO - PrintPrintStmt
}

void printExpr(NODE *node, int indent)
{
    // TODO - PrintExpr
}

void printTerm(NODE *node, int indent)
{
    // TODO - PrintTerm
}

void printFactor(NODE *node, int indent)
{
    // TODO - PrintFactor
}

void printId(NODE *node, int indent)
{
    // TODO - PrintID
}

void printNumber(NODE *node, int indent)
{
    // TODO - PrintNumber
}

void printfIndented(int indent, char *format, ...)
{
    printf("\n");

    for (int i = 0; i < indent; ++i)
    {
        printf("|  ");
    }

    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 255, format, args);

    printf("%s", buffer);

    va_end (args);
}

