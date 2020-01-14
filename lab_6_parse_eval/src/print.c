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
        }
    }

    printfIndented(indent, "END STATEMENT");
}

void printAssignStmt(NODE *node, int indent)
{
    // TODO
}

void printRepeatStmt(NODE *node, int indent)
{
    // TODO
}

void printPrintStmt(NODE *node, int indent)
{
    // TODO
}

void printExpr(NODE *node, int indent)
{
    // TODO
}

void printTerm(NODE *node, int indent)
{
    // TODO
}

void printFactor(NODE *node, int indent)
{
    // TODO
}

void printId(NODE *node, int indent)
{
    // TODO
}

void printNumber(NODE *node, int indent)
{
    // TODO
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

