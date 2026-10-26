#ifndef OPERATORS_H
#define OPERATORS_H

typedef struct {
    double (*func) (double, double);
    char symbol;
} Operator;

int getPriorityBySym(char sym_of_operator)
{
    switch (sym_of_operator) {
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
int getPriority(Operator operator)
{
    return getPriorityBySym(operator.symbol);
}
#endif