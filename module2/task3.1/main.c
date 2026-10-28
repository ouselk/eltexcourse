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
    GETTING_FILENAME_FROM_USER
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