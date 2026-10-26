#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <locale.h>
#include "stack.h"
#include "parser.h"

#define MAX_EXPR_SIZE 1000
#define MAX_NUMBER_SIZE 100
#define SUPPORTED_OPERATORS "*-/+"

int precedence(char operator) { 
    switch (operator) {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '^':
        return 3;
    default:
        return -1;
    }
}

int isOperator(char ch) { //проверка является ли символ оператором 
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^');
}

static void addNum(Token *arr, int i, double value)
{
    arr[i].type = NUMBER;
    arr[i].value=value;
    arr[i].data='\0';
}

static void addOp(Token *arr, int i, char operator)
{
    arr[i].type = OPERATOR;
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
    postfix[j].type = EMPTY;
    return postfix;
}

double calculate(Token* expr)
{
    int i=0;
    stackNode calc = NULL;
    while (expr[i].type!=EMPTY)
    {
        if (expr[i].type==NUMBER)
        {
            stackPush(calc, expr[i].value);
        }
        else if (expr[i].type==OPERATOR)
        {
            double right = stackPop(calc);
            double left  = stackPop(calc);
            stackPush()
        }
    }
}
int main() {
    stackNode* head = NULL;
    char infix[MAX_EXPR_SIZE] = {'\0'};
    printf("Enter String:");
    fgets(infix,MAX_EXPR_SIZE,stdin);
    for (int i = 0; i < MAX_EXPR_SIZE; i++)
    {
        if (infix[i] == '\0' && infix[i-1] == '\n')
            infix[i-1] = '\0';
    }
    fflush(stdin);
    Token* postfix = infixToPostfix(infix, &head);
    if (postfix==NULL)
    {
        printf("String is empty!\n"); 
    }
  //  else if (strcmp(infixToPostfix(infix, &head), "error")==0)
   // {
    //    printf("Error!\n");
  //  }
    else 
    {
        printf("Infix   : %s\n", infix);
        printf("Postfix : ");
        for (int i =0; i<MAX_EXPR_SIZE && postfix[i].type!=EMPTY; i++)
        {
            if (postfix[i].type==NUMBER)
            {
                printf("%g ", postfix[i].value);
            }
            else if (postfix[i].type==OPERATOR)
            {
                printf("%c ", postfix[i].data);
            }
        }   
    }
    free(postfix);
    return 0;
}