#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>
#include "parser.h"
#include "calculator.h"
#include "stack.h"

#define MAX_OPERATORS 100

int loadPlugins(const char* directory, Operator* operators)
{
    DIR* dir = opendir(directory);

    if (!dir)
    {
        perror("Не удалось открыть каталог");
        return -1;
    }

    struct dirent* entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (!strstr(entry->d_name, ".so"))
            continue;

        char path[PATH_MAX];

        snprintf(path, sizeof(path), "%s/%s",
                 directory, entry->d_name);

        void* handle = dlopen(path, RTLD_NOW);

        if (!handle)
        {
            fprintf(stderr, "%s\n", dlerror());
            continue;
        }

        dlerror();

        double (*func)(double, double) =
            (double (*)(double, double))dlsym(handle, "plugin_func");

        char* error = dlerror();

        if (error)
        {
            fprintf(stderr, "%s\n", error);
            dlclose(handle);
            continue;
        }

        char* op = (char*)dlsym(handle, "plugin_operator");

        error = dlerror();

        if (error)
        {
            fprintf(stderr, "%s\n", error);
            dlclose(handle);
            continue;
        }

        if (count >= MAX_OPERATORS)
        {
            dlclose(handle);
            break;
        }

        operators[count] = (Operator){func, *op};

        count++;

        printf("Загружена библиотека: %s\n", entry->d_name);
    }

    closedir(dir);
 
    return count;
}
int main()
{

    Operator operators[MAX_OPERATORS];

    int operators_count = loadPlugins(
        "plugins",
        operators
    );

    printf("Загружено операций: %d\n", operators_count);


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