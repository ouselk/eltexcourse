#ifndef PARSER_H
#define PARSER_H
#define MAX_EXPR_SIZE 1000
#define MAX_NUMBER_SIZE 100

#include "stack.h"
typedef enum 
{
    EMPTY_TOKEN = -1,
    OPERATOR_TOKEN,
    NUMBER_TOKEN
} TOKEN_TYPE;

typedef struct {
    TOKEN_TYPE type;
    double value;
    char data;
} Token;

Token* infixToPostfix(char* infix, stackNode** head);
#endif