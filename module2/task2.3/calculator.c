#include <math.h>
#include <stddef.h>
#include "stack.h"
#include "operator.h"
#include "parser.h"

double doOperation(double left, double right, char operator_symbol,
                const Operator* operators, int operators_count)
{
    for (int i = 0; i<operators_count; i++)
    {
        if (operator_symbol==operators[i].symbol)
        {
            return operators[i].func(left,right);
        }
    }
    return nan("");
}
double calculate(const Token* expr, const Operator* operators, int operators_count)
{
    stackNode *calc = NULL;
    for (int i =0; expr[i].type!=EMPTY_TOKEN; i++)
    {
        if (expr[i].type==NUMBER_TOKEN)
        {
            stackPush(&calc, expr[i].value);
        }
        else if (expr[i].type==OPERATOR_TOKEN)
        {
            double right = stackPop(&calc);
            double left  = stackPop(&calc);
            stackPush(&calc, doOperation(left,right,expr[i].data, operators, operators_count));
        }
    }
    return stackPop(&calc);
}