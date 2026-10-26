#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "stack.h"
#include "operator.h"
#include "parser.h"

double calculate(const Token* expr, const Operator* operators, int operators_count);
double doOperation(double left, double right, char operator_symbol,
                const Operator* operators, int operators_count);

#endif