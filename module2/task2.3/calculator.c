#include <math.h>
#include "stack.h"
#include "operator.h"
#include "parser.h"

double calculate(double left, double right, char operator_symbol,
                const Operator* operators, int operators_count)
{
    for (int i = 0; i<operators_count, i++)
    {
        if (operator_symbol==operators[i].symbol)
        {
            return operators.func(left,right);
        }
    }
    return nan("")
}
double calculate(const Token* expr, const Operator* operators, int operator_count)
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
            stackPush(calculate(left,right,expr[i].data, operators, operators_count))
        }
    }
    return stackPop();
}