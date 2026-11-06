#ifndef OPERATORS_H
#define OPERATORS_H

typedef struct {
    double (*func) (double, double);
    char symbol;
} Operator;

int getPriorityBySym(char sym_of_operator);
int getPriority(Operator operator);

#endif