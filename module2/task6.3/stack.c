#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stack.h"

int stackPush(stackNode** head, double data) {
    stackNode* tmp = malloc(sizeof(stackNode));
    if (tmp == NULL) {
        return -1;
    }
    tmp->next = *head;
    tmp->data = data;
    *head = tmp;
    return 0;
}

double stackPop(stackNode** head) {
    stackNode* out;
    double value;
    if (*head == NULL) {
        return nan("");
    }
    out = *head;
    *head = (*head)->next;
    value = out->data;
    free(out);
    return value;
}

void stackPrint(stackNode* head)
{
    if (head==NULL)
    {
        printf("Stack is empty\n");
        return;
    }

    stackNode* tmp = head;
    while (tmp!=NULL)
    {
        printf("%d ", tmp->data);
        tmp = tmp->next;
    }
    printf("\n");
}