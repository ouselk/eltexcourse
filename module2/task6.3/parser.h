#ifndef PARSER_H
#define PARSER_H
#define MAX_EXPR_SIZE 1000
#define MAX_NUMBER_SIZE 100

#include "stack.h"
#include "operator.h"
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

int precedence(char operator);
int isOperator(char ch);
static void addNum(Token *arr, int i, double value);
static void addOp(Token *arr, int i, char operator);
Token* infixToPostfix(char* infix, stackNode** head);

#endif