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


int main()
{
    stackNode* head = NULL;
    char infix[MAX_EXPR_SIZE] = {'\0'};
    printf("Введите выражение:");
    fgets(infix,MAX_EXPR_SIZE,stdin);
    infix[strcspn(infix, "\n")] = '\0';
    fflush(stdin);
    Token* postfix = infixToPostfix(infix, &head);


    Operator* operators = [
        {mySumFunc,  '+'},
        {myDiffFunc, '-'},
        {myMultFunc, '*'},
        {myDivFunc,  '/'}
    ]

    calculate(postfix, operators, 4);
    return 0;

}