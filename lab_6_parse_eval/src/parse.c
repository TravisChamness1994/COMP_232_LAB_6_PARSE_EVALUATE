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
    TOKEN *token = getNextToken(NULL); //Getting the next token and storing it in token pointer
    if (token == NULL) //Edge case
    {
        return NULL;
    }
    else if (token->type == EOF_TOKEN) //Edge case
    {
        freeToken(&token);
        return NULL;
    }

    NODE *node = calloc(sizeof(NODE), 1); //Create node
    node->type = STATEMENT_NODE;    //This new node is of statement_node

    switch(token->type)
    {
        case IDENT_TOKEN: //If token is identifier_token
            node->leftNode = assignStmt(&token); //then we know the node will be representing an assignmentStatement. View language for context.
            break;
        case REPEAT_TOKEN: //...
            node->leftNode = repeatStmt(&token);
            break;
        case PRINT_TOKEN: //...
            node->leftNode = printStmt(&token);
            break;
        default: //default is that we are not entering a statment type
            error("Invalid token of type %d at start of statement.", token->type);

    }

    return node;
}

NODE *assignStmt(TOKEN **currToken)
{
    NODE *node = calloc(sizeof(NODE), 1);
    //assume that currToken is not a Null token because it has been passed to us with that check
    node->type = ASSIGN_STMT_NODE;
    //Assign Left_Node
    node->leftNode = ident(currToken);
    //Now we need to test for assignment opperator
    *currToken = getNextToken(currToken);

    if((*currToken)->type != ASSIGNMENT_TOKEN)
    {
        //will stop program if executes
        error("Expected an assignment token in an assignment statement.");
    }
    //token will not be created into a node, so we will getnexttoken the given token
    *currToken = getNextToken(currToken);

    node -> rightNode = expr(currToken);

    *currToken = getNextToken(currToken);

    if((*currToken)->type != SEMICOLON_TOKEN)
    {
        error("Expected a semicolon token in an assignment statement.");
    }
    //we will not get next or pass currentToken forward so we need to free
    freeToken(currToken);

    return node;
    // TODO - assignStmt - Done
}

NODE *repeatStmt(TOKEN **currToken)
{
    //repeat ( <expr> ) <statement>
    //Assume it is a repeat token because it is tested for in Statement production
    NODE *node = calloc(sizeof(NODE),1);
    node->type = REPEAT_STMT_NODE;
    //expecting a lPar
    *currToken = getNextToken(currToken);

    if((*currToken)->type != LPAREN_TOKEN)
    {
        error("Expected a left parenthesis in a repeat statement.");
    }

    *currToken = getNextToken(currToken);

    node->leftNode = expr(currToken);

    *currToken = getNextToken(currToken);

    if((*currToken)->type != RPAREN_TOKEN)
    {
        error("Expected a right parenthesis in a repeat statement.");
    }
    //statement pulls next token, no free, no next token
    node->rightNode = statement();

    return node;
    // TODO - repeatStmt - Work
}

NODE *printStmt(TOKEN **currToken)
{
    //print <expr> ;
    //Assume it is a print token because it is tested for in Statement production
    NODE *node = calloc(sizeof(NODE), 1);
    node->type = PRINT_STMT_NODE;

    *currToken = getNextToken(currToken);

    node->leftNode = expr(currToken);
    //expecting semicolon
    *currToken = getNextToken(currToken);

    if((*currToken)->type != SEMICOLON_TOKEN)
    {
        error("Expected a left parenthesis in a print statement.");
    }

    freeToken(currToken);

    return node;
    // TODO - printStmt - Work
}

NODE *expr(TOKEN **currToken)
{
    // <term> | <term> <addop> <expr>
    //Dont need to check token, need to check what is expexted
    NODE *node = calloc(sizeof(NODE), 1);
    node->type = EXPR_NODE;

    node->leftNode = term(currToken);

    *currToken = getNextToken(currToken);

    switch ((*currToken)->type)
    {
        case ADD_OP_TOKEN:

                node->data.op = (*currToken)->val.op;

                *currToken = getNextToken(currToken);

                node->rightNode = expr(currToken);

            break;

        default:

            ungetToken(currToken);

            break;
    }

    return node;
    // TODO - expr - Work
}

NODE *term(TOKEN **currToken)
{
    NODE* node = calloc(sizeof(NODE), 1);
    node -> type = TERM_NODE;

    node -> leftNode = factor(currToken);

    *currToken = getNextToken(currToken);

    switch ((*currToken)->type)
    {
        case MULT_OP_TOKEN:

            node->data.op = (*currToken)->val.op;

            *currToken = getNextToken(currToken);

            node->rightNode = term(currToken);

            break;

        default:

            ungetToken(currToken);

            break;
    }
    return node;
    // TODO - term - work
}

NODE *factor(TOKEN **currToken)
{
    NODE *node = calloc(sizeof(NODE), 1);
    node ->type = FACTOR_NODE;

    switch ((*currToken)->type)
    {
        case IDENT_TOKEN:

            node->leftNode = ident(currToken);

            break;

        case FLOAT_TOKEN:

        case INT_TOKEN:

            node->leftNode = number(currToken);

            break;

        case ADD_OP_TOKEN:

                node->data.op = (*currToken)->val.op;

                *currToken = getNextToken(currToken);

                node->leftNode = number(currToken);

            break;

        case RPAREN_TOKEN:

                node->leftNode = expr(currToken);

            break;

        default:

            ungetToken(currToken);

            error("Expected ID, Number, Factor, or Expression");

            break;
    }

    return node;
    // TODO - Factor - Work
}

NODE *ident(TOKEN **currToken)
{
    NODE *node = calloc(sizeof(NODE), 1);
    node->type = IDENT_NODE;

    if((*currToken)->val.string == NULL)
    {

        ungetToken(currToken);

        error("Expected a string for an ID.");

    }
    else
        {
            node->data.identifier = calloc(BUF_SIZE, sizeof(char));

            strcpy(node->data.identifier, (*currToken)->val.string);

            printf("\n");
        }

    //not passing forward, achieved a decorated leaf, can free
    freeToken(currToken);
    return node;
    // TODO - Ident - Work
}

NODE *number(TOKEN **currToken)
{
    NODE *node = calloc(sizeof(NODE), 1);
    node->type = NUMBER_NODE;

    switch ((*currToken)->type) {
        case INT_TOKEN:

            node->data.number.type = INT_TYPE;

            node->data.number.value.integer = (*currToken)->val.integral;

            break;

        case FLOAT_TOKEN:

            node->data.number.type = FLOAT_TYPE;

            node->data.number.value.floating_point = (*currToken)->val.floating_point;

            break;

        default:

            ungetToken(currToken);

            error("Expected a integer or float for a number.");

            break;
    }

    //No passing forward, can free the current token
    freeToken(currToken);
    return node;
    // TODO - number - work
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
    //No passing forward, can free current token
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
