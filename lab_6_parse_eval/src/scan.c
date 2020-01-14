#include "scan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 32

TOKEN *ungottenToken = NULL;

void ungetToken(TOKEN **token)
{
    ungottenToken = *token;
    *token = NULL;
}

//
// clean up the token structure
//
void freeToken(TOKEN **token)
{
    if (token == NULL)
        return;

    if (*token == NULL)
        return;

    if ((*token)->type == IDENT_TOKEN)
    {
        free((*token)->val.string);
    }

    free(*token);
    *token = NULL;
}

bool updateKeywordOrId(TOKEN *token, char *str)
{
    if (strcmp(str, "repeat") == 0)
    {
        token->type = REPEAT_TOKEN;
    }
    else if (strcmp(str, "print") == 0)
    {
        token->type = PRINT_TOKEN;
    }
    else
    {
        token->type = IDENT_TOKEN;
        token->val.string = malloc((strlen(str) + 1) * sizeof(char));
        strcpy(token->val.string, str);
        return false;
    }
    return true;
}

void printToken (TOKEN **token)
{
    switch((*token)->type)
    {
        case INVALID_TOKEN:
            printf("<INVALID, %c>", (*token)->val.op);
            break;
        case REPEAT_TOKEN:
            printf("<KEYWORD, repeat>");
            break;
        case PRINT_TOKEN:
            printf("<KEYWORD, print>");
            break;
        case IDENT_TOKEN:
            printf("<IDENT, %s>", (*token)->val.string);
            break;
        case INT_TOKEN:
            printf("<INT, %ld>", (*token)->val.integral);
            break;
        case FLOAT_TOKEN:
            printf("<FLOAT, %lf>", (*token)->val.floating_point);
            break;
        case ASSIGNMENT_TOKEN:
            printf("<ASSIGN>");
            break;
        case LPAREN_TOKEN:
            printf("<LPAREN>");
            break;
        case RPAREN_TOKEN:
            printf("<RPAREN>");
            break;
        case ADD_OP_TOKEN:
            printf("<ADD_OP, %c>", (*token)->val.op);
            break;
        case MULT_OP_TOKEN:
            printf("<MULT_OP, %c>", (*token)->val.op);
            break;
        case SEMICOLON_TOKEN:
            printf("<SEMICOLON>");
            break;
        case EOF_TOKEN:
            printf("<EOF>");
            break;
        case NO_TOKEN_TYPE:
            printf("<NO_TYPE, something when wrong, should have been invalid token...>");
            break;
    }
    printf("\n");
}

TOKEN *scanner()
{
    if (ungottenToken != NULL)
    {
        TOKEN *token = ungottenToken;
        ungottenToken = NULL;
        return token;
    }

    // buffer to store a token's contained characters while it is being tokenized
    size_t bufferSize = BUFFER_SIZE;
    char *stringValue = calloc(sizeof(char), bufferSize);
    int stringValueIndex = 0;

    // allocate space for the new token
    TOKEN *token = (TOKEN *) malloc(sizeof(TOKEN));

    // initialize the token type to invalid
    token->type = NO_TOKEN_TYPE;

    // set state machine to starting state
    STATE state = START_STATE;

    char currentChar = '\0';

    while (currentChar != EOF && token->type == NO_TOKEN_TYPE)
    {
        currentChar = (char) getchar();
        switch (state)
        {
            case START_STATE:
                switch (currentChar)
                {
                    case '0'...'9':
                        stringValue[stringValueIndex++] = currentChar;
                        state = INT_STATE;
                        break;
                    case 'A'...'z':
                        stringValue[stringValueIndex++] = currentChar;
                        state = ID_OR_KEYWORD_STATE;
                        break;
                    case '=':
                        token->type = ASSIGNMENT_TOKEN;
                        break;
                    case '(':
                        token->type = LPAREN_TOKEN;
                        break;
                    case ')':
                        token->type = RPAREN_TOKEN;
                        break;
                    case '+':
                    case '-':
                        token->type = ADD_OP_TOKEN;
                        token->val.op = currentChar;
                        break;
                    case '*':
                    case '/':
                    case '%':
                        token->type = MULT_OP_TOKEN;
                        token->val.op = currentChar;
                        break;
                    case ';':
                        token->type = SEMICOLON_TOKEN;
                        break;
                    case EOF:
                        token->type = EOF_TOKEN;
                        break;
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                        break;
                    default:
                        token->type = INVALID_TOKEN;
                        token->val.op = currentChar;
                        break;
                }
                break;
            case ID_OR_KEYWORD_STATE:
                switch (currentChar)
                {
                    case 'A'...'z':
                    case '0'...'9':
                        stringValue[stringValueIndex++] = currentChar;
                        break;
                    default:
                        ungetc(currentChar, stdin);
                        updateKeywordOrId(token, stringValue);
                        break;
                }
                break;
            case INT_STATE:
                switch (currentChar)
                {
                    case '0'...'9':
                        stringValue[stringValueIndex++] = currentChar;
                        break;
                    case '.':
                        stringValue[stringValueIndex++] = currentChar;
                        state = FLOAT_STATE;
                        break;
                    default:
                        ungetc(currentChar, stdin);
                        token->type = INT_TOKEN;
                        token->val.integral = strtol(stringValue, NULL, 10);
                        break;
                }
                break;
            case FLOAT_STATE:
                switch (currentChar)
                {
                    case '0'...'9':
                        stringValue[stringValueIndex++] = currentChar;
                        break;
                    default:
                        ungetc(currentChar, stdin);
                        token->type = FLOAT_TOKEN;
                        token->val.floating_point = strtod(stringValue, NULL);
                        break;
                }
                break;
        }

        // If the buffer is full, double its size
        // I really ought to realloc here and just manually set the null terminator in the switch above...
        // but this lets me move the ugly down here and leave the pretty up there...
        if (stringValueIndex >= bufferSize-2)
        {
            bufferSize *= 2;
            char *temp = calloc(sizeof(char), bufferSize);
            strcpy(temp, stringValue);
            free(stringValue);
            stringValue = temp;
        }
    }

    free(stringValue);
    return token;
}

