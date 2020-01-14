#include "parse.h"
#include <stdarg.h>

TOKEN *getNextToken(TOKEN **token)
{
    freeToken(token);
    return scanner();
}

NODE *program()
{
    NODE *node = calloc(sizeof(NODE), 1);
    node->type = PROGRAM_NODE;
    node->leftNode = statement();
    if (node->leftNode != NULL)
    {
        node->rightNode = program();
    }
    else
    {
        free(node);
        node = NULL;
    }
    return node;
}

NODE *statement()
{
    TOKEN *token = getNextToken(NULL);
    if (token == NULL)
    {
        return NULL;
    }
    else if (token->type == EOF_TOKEN)
    {
        freeToken(&token);
        return NULL;
    }

    NODE *node = calloc(sizeof(NODE), 1);
    node->type = STATEMENT_NODE;

    switch(token->type)
    {
        case IDENT_TOKEN:
            node->leftNode = assignStmt(&token);
            break;
        case REPEAT_TOKEN:
            node->leftNode = repeatStmt(&token);
            break;
        case PRINT_TOKEN:
            node->leftNode = printStmt(&token);
            break;
        default:
            error("Invalid token of type %d at start of statement.", token->type);
    }

    return node;
}

NODE *assignStmt(TOKEN **currToken)
{
    // TODO
}

NODE *repeatStmt(TOKEN **currToken)
{
    // TODO
}

NODE *printStmt(TOKEN **currToken)
{
    // TODO
}

NODE *expr(TOKEN **currToken)
{
    // TODO
}

NODE *term(TOKEN **currToken)
{
    // TODO
}

NODE *factor(TOKEN **currToken)
{
    // TODO
}

NODE *ident(TOKEN **currToken)
{
    // TODO
}

NODE *number(TOKEN **currToken)
{
    // TODO
}

void freeParseTree(NODE **node)
{
    if (node == NULL)
    {
        return;
    }

    if ((*node) == NULL)
    {
        return;
    }

    freeParseTree(&(*node)->leftNode);
    freeParseTree(&(*node)->rightNode);

    if ((*node)->type == IDENT_NODE)
    {
        free((*node)->data.identifier);
    }

    free(*node);
    *node = NULL;
}

void error(char *errorFormat, ...)
{
    char buffer[256];
    va_list args;
    va_start (args, errorFormat);
    vsnprintf (buffer, 255, errorFormat, args);

    printf("\nPARSING ERROR: %s\nExiting...\n", buffer);

    va_end (args);
    exit(1);
}
