#ifndef PARSER_H
#define PARSER_H

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

#endif