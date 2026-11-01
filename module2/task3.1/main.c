#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include "accessRights.h"


typedef enum
{
    EPHEMERAL =-1,
    IN_MAIN_MENU,
    GETING_RIGHTS_FROM_USER,
    GETTING_FILENAME_FROM_USER,
    CHANGING_RIGHTS
} ProgramStatus;

ProgramStatus PROGRAM_STATUS = EPHEMERAL;

int readLine(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL)
        return -1;

    char *nl = strchr(buf, '\n');
    if (nl != NULL)
    {
        *nl = '\0';
    }
    else
    {
        char c = 1;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    return 0;
}

void getRightsFromUserAndPrintBinary()
{
    PROGRAM_STATUS = GETING_RIGHTS_FROM_USER;
    char userinput[10] = {'\0'};
    int err = 0;
    do
    {
        if (err==-1)
        {
            printf("Вы ввели ерунду, исправьтесь.\n");
        }
        printf("Введите права доступа в буквенном или числовом формате: ");

//        char c = 1;
 //       for (int i =0; i<=10 && c!='\0' && c!='\n'; i++)
    //    {
  //          c = getchar();
     //       userinput[i]=c;
       // }
        readLine(userinput, 10);
        BitmapType result = 0;
        if (strlen(userinput)==4)
        {
            result = octStringToBits(userinput, &err);
        }
        else if (strlen(userinput)==9)
        {
            result = stringToBits(userinput, &err);
        }
        else { err = -1;}
        
        if (err==0)
        {
            printf("Двоичное представление: ");
            printBinaryMask(result);
        }
    } while (PROGRAM_STATUS==GETING_RIGHTS_FROM_USER);
}

void changeRightsAndPrint(BitmapType bitmap)
{
    PROGRAM_STATUS = CHANGING_RIGHTS;
    char input[5] = {'\0'};
    int err = 0;

    printf("Хотите изменить права доступа? (y/n): ");
    readLine(input, sizeof(input));

    if (strcmp(input, "y") != 0 &&
        strcmp(input, "Y") != 0 &&
        strcmp(input, "да") != 0)
    {
        return;
    }
    do 
    {
        printf("Введите новые права (например, 0755) "
            "или изменение (например, u+x, go-w, a=r): ");

        readLine(input, sizeof(input));

        // Числовая запись: заменяем права целиком
        if (input[0] >= '0' && input[0] <= '7')
        {
            bitmap = octStringToBits(input, &err);

            if (err != 0)
            {
                printf("Некорректная восьмеричная запись.\n");
                return;
            }
        }
        else
        {
            // Символьная запись в стиле chmod
            char *p = input;
            char who[4] = {'\0'};
            int whoCount = 0;

            while (*p != '\0' &&
                *p != '+' && *p != '-' && *p != '=')
            {
                if (*p == 'u' || *p == 'g' ||
                    *p == 'o' || *p == 'a')
                {
                    who[whoCount++] = *p;
                }
                else
                {
                    printf("Некорректный класс доступа.\n");
                    return;
                }

                if (whoCount >= 3)
                {
                    printf("Некорректный класс доступа.\n");
                    return;
                }

                p++;
            }

            if (whoCount == 0)
            {
                printf("Не указан класс доступа.\n");
                return;
            }

            char operation = *p;

            if (operation != '+' &&
                operation != '-' &&
                operation != '=')
            {
                printf("Некорректная операция.\n");
                return;
            }

            p++;

            if (*p == '\0')
            {
                printf("Не указаны права.\n");
                return;
            }

            BitmapType mask = 0;
            BitmapType specialMask = 0;

            for (; *p != '\0'; p++)
            {
                if (*p == 'r')
                    mask |= 4;
                else if (*p == 'w')
                    mask |= 2;
                else if (*p == 'x')
                    mask |= 1;
                else if (*p == 's')
                    specialMask |= 1;
                else if (*p == 't')
                    specialMask |= 1;
                else
                {
                    printf("Некорректное право доступа.\n");
                    return;
                }
            }

            int user = 0, group = 0, other = 0;

            for (int i = 0; i < whoCount; i++)
            {
                if (who[i] == 'a')
                    user = group = other = 1;
                else if (who[i] == 'u')
                    user = 1;
                else if (who[i] == 'g')
                    group = 1;
                else if (who[i] == 'o')
                    other = 1;
            }

            BitmapType positions[3] = {6, 3, 0};
            int selected[3] = {user, group, other};

            for (int i = 0; i < 3; i++)
            {
                if (!selected[i])
                    continue;

                BitmapType currentMask = mask << positions[i];

                if (operation == '+')
                    bitmap |= currentMask;
                else if (operation == '-')
                    bitmap &= ~currentMask;
                else
                {
                    bitmap &= ~(7 << positions[i]);
                    bitmap |= currentMask;
                }
            }

            // Специальные биты setuid, setgid и sticky
            if (specialMask)
            {
                if (user)
                    bitmap |= (1 << 11);

                if (group)
                    bitmap |= (1 << 10);

                if (other)
                    bitmap |= (1 << 9);
            }
        }

        printf("\nНовые права доступа:\n");

        printf("Двоичное представление: ");
        printBinaryMask(bitmap);

        char oct[5] = {'\0'};
        bitsToOctString(bitmap, oct);
        printf("Восьмеричное представление: %s\n", oct);

        char str[10] = {'\0'};
        bitsToString(bitmap, str);
        printf("Строковое представление: %s\n", str);
    } while (PROGRAM_STATUS==CHANGING_RIGHTS);
}

#define MAX_FILENAME_SIZE 255
void getFilenameAndPrint()
{
    PROGRAM_STATUS=GETTING_FILENAME_FROM_USER;
    char userinput [MAX_FILENAME_SIZE] = {'\0'};
    int err = 0;
    do
    {
        if (err!=0)
        {
            printf("Не получилось. попробуйте еще раз.\n");
        }
        printf("Введите имя файла: ");
        readLine(userinput, MAX_FILENAME_SIZE);

        struct stat sb;
        err = stat(userinput, &sb);
        BitmapType bitmap = sb.st_mode & 07777;
        if (err==0)
        {
            printf("Двоичное представление: ");
            printBinaryMask(bitmap);

            char oct[5] = {'\0'};
            bitsToOctString(bitmap, oct);
            printf("Восьмиричное представление: %s\n", oct);
            
            char str[9] = {'\0'};
            bitsToString(bitmap, str);
            printf("Строковое представление: %s\n", str);
            changeRightsAndPrint(bitmap);
        }

    } while (PROGRAM_STATUS==GETTING_FILENAME_FROM_USER);

}

void interrupt_handler(int signal)
{
    puts("");
    switch (PROGRAM_STATUS)
    {
        case IN_MAIN_MENU:
            puts("Завершаем работу.");
            exit(0);
            break;
        case GETING_RIGHTS_FROM_USER:
        case GETTING_FILENAME_FROM_USER:
        case CHANGING_RIGHTS:
            PROGRAM_STATUS=EPHEMERAL;
            puts("Для выхода в главное меню нажмите Enter.");
            break;
    }
    
}

int main()
{
    signal(SIGINT, interrupt_handler);
    int userChange = 0;
    while (1)
    {
        PROGRAM_STATUS=IN_MAIN_MENU;
        puts("Список действий");
        puts("----------------");
        puts("1) Преобразобать буквенную/численную запись прав в бинарную.");
        puts("2) Посмотреть права доступа к файлу.");
        printf("Выберите действие: ");

        scanf("%d", &userChange);
        char c = 1;
        while ((c=getchar()) != '\n' && c!=EOF);


        if (userChange==1)
        {
            getRightsFromUserAndPrintBinary();
        }
        else if (userChange==2)
        {
            getFilenameAndPrint();
        }
    }
//    printf("%d", sizeof(int));
}