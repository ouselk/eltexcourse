#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <locale.h>
#include "stack.h"
#include "operator.h"
#include "parser.h"

#define MAX_EXPR_SIZE 1000
#define MAX_NUMBER_SIZE 100
#define SUPPORTED_OPERATORS "*-/+"

int precedence(char operator) { 
    return getPriority(operator)
}

int isOperator(char ch) { //проверка является ли символ оператором 
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^');
}

static void addNum(Token *arr, int i, double value)
{
    arr[i].type = NUMBER_TOKEN;
    arr[i].value=value;
    arr[i].data='\0';
}

static void addOp(Token *arr, int i, char operator)
{
    arr[i].type = OPERATOR_TOKEN;
    arr[i].value=0;
    arr[i].data=operator;
}
Token* infixToPostfix(char* infix, stackNode** head) {
    int i, j;
    int len = strlen(infix); //инициализируем переменную размером входной строки
    if (len==0)
    {
        return NULL;
    }
    Token* postfix = (Token*)malloc(sizeof(Token) * (len + 2)); //выделяем память под постфиксную строку

    for (i = 0, j = 0; i < len; i++) {
        if (isalnum(infix[i])) { //Если символ - буква или цифра, то добавляем его в постфиксную строку postfix.
            char buf[MAX_NUMBER_SIZE+1];
            int k = 0;
            while (isalnum(infix[i]))
            {
                buf[k++]=infix[i++];
                if (k>MAX_NUMBER_SIZE)
                {
                    exit(-67);
                }
            }
            i--;
            buf[k]='\0';
            addNum(postfix, j++, strtod(buf, NULL));
        } else if (infix[i] == '(') {
            stackPush(head, infix[i]);
        } else if (infix[i] == ')') { //Если символ - закрывающая скобка, проверяем стек 
        //и переносим операторы из стека в постфиксную строку, пока не встретим открывающую скобку.
            while ((*head) != NULL && (*head)->data != '(')
                addOp(postfix, j++, stackPop(head));
            if (*head != NULL) {
                stackPop(head); 
            } else {
                exit(222);
            }
        } else if (isOperator(infix[i])) { //Если символ - оператор, переносим операторы из стека в постфиксную строку в соответствии с их приоритетом.
            while ((*head) != NULL && precedence((*head)->data) >= precedence(infix[i]))
                addOp(postfix, j++, stackPop(head));
            stackPush(head, infix[i]);
        }
    }
    while ((*head) != NULL) {
        if ((*head)->data == '(' || (*head)->data == ')') {
            exit(222); 
        }
        addOp(postfix, j++, stackPop(head));
    }
    postfix[j].type = EMPTY_TOKEN;
    return postfix;
}