#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "stack.h"
#include "operator.h"
#include "parser.h"

double calculate(const Token* expr, const Operator* operators);
double calculate(double left, double right, char operator_symbol,
                const Operator* operators, int operators_count);

#endif