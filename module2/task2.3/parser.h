#ifndef PARSER_H
#define PARSER_H

typedef enum 
{
    EMPTY = -1,
    OPERATOR,
    NUMBER
} TOKEN_TYPE;

typedef struct {
    TOKEN_TYPE type;
    double value;
    char data;
} Token;

typedef struct {
    double (*func) (double, double);
    char symbol;
    int priority;
} Fuction

#endif