#include "print.h"
#include "parse.h"

//char* numToStr(NODE *node)
//{
//    switch (node->data.number.type)
//    {
//        case INT_TYPE:
//
//    }
//}
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
                printExpr(node->leftNode, indent + 1);
                break;
            default:
                //If the left node is populated, but is not the repeat node, we throw an error
                error("Expected an expression in repeat statement.");
                break; //break is not necessary
        }
    } else
        //if the left node is not populated, we throw an error. However, will this ever happen?
        error("Invalid repeat statement.");

    if(node->rightNode != NULL)
    {
        switch (node->rightNode->type)
        {
            case STATEMENT_NODE:
                printStatement(node->rightNode, indent + 1);
                break;
            default:
                error("Expected a statement in the repeat statement.");
                break;
        }
    }
    else
        error("Invalid repeat statement.");
    // TODO - PrintRepeatStmt - Work
    printfIndented(indent, "END REPEAT STATEMENT");
}

void printPrintStmt(NODE *node, int indent)
{
    printfIndented(indent, "START PRINT STATEMENT");

    if(node->leftNode != NULL)
    {
        switch (node->leftNode->type)
        {
            case EXPR_NODE:
                printExpr(node->leftNode, indent + 1);
                break;
            default:
                error("Expected an expression in a print statement.");
                break;
        }
    }
    else
        error("Invalid print statement.");

    printfIndented(indent, "END PRINT STATEMENT");
    // TODO - PrintPrintStmt
}

void printExpr(NODE *node, int indent)
{
    printfIndented(indent, "START EXPRESSION");
    if(node->leftNode != NULL)
    {
        switch (node->leftNode->type)
        {
            case TERM_NODE:
                printTerm(node->leftNode, indent + 1);
                if(node->rightNode != NULL)
                {
                    switch (node->rightNode->type)
                    {
                        case EXPR_NODE:
                            printExpr(node->rightNode, indent + 1);
                            break;
                        default:
                            error("Expected expression in expression statement.");
                            break;
                    }
                }
                break;
            default:
                error("Expected a term in the expression statement.");
                break;
        }
    } else
        error("Invalid expression.");

    printfIndented(indent, "END EXPRESSION");
    // TODO - PrintExpr
}

void printTerm(NODE *node, int indent)
{
    printfIndented(indent, "START TERM");

    if(node->leftNode != NULL)
    {
        switch (node->leftNode->type)
        {
            case FACTOR_NODE:
                printFactor(node->leftNode, indent + 1);
                if(node->rightNode != NULL)
                {
                    //Does not account for the operand
                    switch (node->rightNode->type)
                    {
                        case TERM_NODE:
                            printTerm(node->rightNode, indent + 1);
                            break;
                        default:
                            error("Expected a term in the term");
                            break;
                    }
                }
                break;
            default:
                error("Expected a factor in the term.");
                break;
        }
    } else
        error("Invalid term");
    printfIndented(indent, "END TERM");
    // TODO - PrintTerm - Work
}

void printFactor(NODE *node, int indent)
{
    printfIndented(indent , "START FACTOR");

    if(node->leftNode != NULL)
    {
        switch(node->leftNode->type)
        {
            case IDENT_NODE:
                printId(node->leftNode, indent + 1);
                break;
            case NUMBER_NODE:
                printNumber(node->leftNode, indent + 1);
                break;
            case FACTOR_NODE:
                printFactor(node->leftNode, indent + 1);
                break;
            case EXPR_NODE:
                printExpr(node->leftNode, indent + 1);
                break;
            default:
                error("Expected an ID, number, factor, or expression in factor.");
                break;
        }
    } else
        error("Invalid factor.");

    printfIndented(indent, "END FACTOR");
    // TODO - PrintFactor - Work
}

void printId(NODE *node, int indent)
{
    printfIndented(indent, "START IDENT");

    if(node->data.identifier != NULL)
    {

        printfIndented(indent + 1, (node->data.identifier));

    } else

        error("Invalid ID.");

    printfIndented(indent, "END IDENT");

    // TODO - PrintID - work
}

void printNumber(NODE *node, int indent)
{
    printfIndented(indent, "START NUMBER");

    if(node != NULL)
    {
        switch (node->data.number.type)
        {
            case INT_TYPE:

                node->data.identifier = calloc(BUF_SIZE, sizeof(char));

                sprintf(node->data.identifier,"%ld", node->data.number.value.integer);

                printfIndented(indent + 1, node->data.identifier);

                break;
            case FLOAT_TYPE:

                node->data.identifier = calloc(BUF_SIZE, sizeof(char));

                sprintf(node->data.identifier,"%f", node->data.number.value.floating_point);

                printfIndented(indent + 1, node->data.identifier);

                break;
            default:
                error("Expected a float or an int in a number.");
                break;
        }
    } else
        error("Invalid Number");

    printfIndented(indent, "END NUMBER");

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

