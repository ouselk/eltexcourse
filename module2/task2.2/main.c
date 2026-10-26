#include <math.h>
#include <stdio.h>
#include <stdbool.h>

double sum(double n1, double n2)
{
    return n1+n2;
}

double multiple(double n1, double n2)
{
    return n1*n2;
}

double devide(double n1, double n2)
{
    if (n2==0)
    {
        return nan("");
    }
    return n1/n2;
}
double diff (double n1, double n2)
{
    return n1-n2;
} 


void main_menu()
{
    while (true)
    {
        int operation_code = 0;
        puts("Введите код операции:");
        puts("1) Сложение");
        puts("2) Вычитание");
        puts("3) Умножение");
        puts("4) Деление");

        scanf("%d", &operation_code);
        fflush(stdin);

        if (operation_code<1 || operation_code>4)
        {
            puts("Так не поступают");
            continue;
        }

        double n1 =0;
        double n2 =0;
        printf("Введите первое число: ");
        scanf("%lf", &n1);
        fflush(stdin);
        printf("Введите второе число: ");
        scanf("%lf", &n2);
        fflush(stdin);

        double result = nan("");
        switch (operation_code)
        {
            case 1:
                result = sum(n1,n2);
                break;
            case 2:
                result = diff(n1,n2);
                break;
            case 3:
                result = multiple(n1, n2);
                break;
            case 4:
                result = devide(n1,n2);
                break;
            }
      //  printf("%f, %f, %f", n1, n2, result);
        
        if (isnan(result))
        {
            puts("Некорректный результат операции. Проверьте введенные данные.");
        }
        else
        {
            printf("Результат операции: %lf\n", result);
        }
    }
}

int main()
{
    main_menu();
}