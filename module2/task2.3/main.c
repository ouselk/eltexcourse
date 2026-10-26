#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "calculator.h"
#include "stack.h"

double mySumFunc(double left, double right)
{
    return left+right;
}
double myDiffFunc(double left, double right)
{
    return left-right;
}
double myMultFunc(double left, double right)
{
    return left*right;
}
double myDivFunc(double left, double right)
{
    return left/right;
}
double myStrangeFunc(double left, double right)
{
    return (left-right)*3;
}

int main()
{

    Operator operators[] = {
        {mySumFunc,  '+'},
        {myDiffFunc, '-'},
        {myMultFunc, '*'},
        {myDivFunc,  '/'},
        {myStrangeFunc, '@'}
    };
    const int operators_count = 5;

    stackNode* head = NULL;
    char infix[MAX_EXPR_SIZE] = {'\0'};
    double result = nan("");
    while (1)
    {
    printf("Введите выражение:");
        fgets(infix,MAX_EXPR_SIZE,stdin);
        if (strlen(infix)==1)
        {
            printf("Вы ничего не ввели. Извинитесь.\n");
            continue;
        }
        infix[strcspn(infix, "\n")] = '\0';
        fflush(stdin);

        Token* postfix = infixToPostfix(infix, &head);
        result = calculate(postfix, operators, operators_count);
        if (isnan(result))
        {
            printf("Проверьте корректность выражения.\n");
        }
        else if (isinf(result))
        {
            printf("Деление на ноль запрещено.\n");
        }
        else
        {
            printf("Ответ: %f\n", result);
        }
    }
    return 0;

}