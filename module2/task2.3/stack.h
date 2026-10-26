#ifndef STACK_H
#define STACK_H


typedef struct stackNode
{
    double data;
    struct stackNode* next;
} stackNode;

int stackPush(stackNode** head, double data);
double stackPop(stackNode** head);
void stackPrint(stackNode* head);

#endif