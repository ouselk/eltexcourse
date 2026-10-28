#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "accessRights.h"

void printBinaryMask(BitmapType n)
{
    for (int i =11; i >= 0; i--)
    {
        printf("%d", (n >> i) & 1);
    }
    printf("\n");
}

static int putBitInPosition (BitmapType *bitmap, bool bit, int pos)
{

    if (bit==1)
    {
        BitmapType mask = 1;  // 0000.0001
        mask = mask << pos;          // 0000.0100
        *bitmap = (*bitmap) | mask;
    }
    else if (bit==0)
    {
        BitmapType mask = 1;  // 0000.0001
        mask = mask << pos;          // 0000.0100
        mask = ~mask;         // 1111.1011
        *bitmap = (*bitmap)&mask;
    }
    else 
    {
        return -1;
    }
 //   printf("after %d: ", pos);
  //  printBinaryMask(*bitmap);
    return 1;
}

//принимает строку вида rwxrwxrwx
BitmapType stringToBits(const char* string, int* err)
{
    BitmapType result = 0;
    switch (string[8])      // rwxrwxrw?
    {
        case 'x':
            putBitInPosition(&result, 1, 0);
            break;
        case 't':
            putBitInPosition(&result, 1, 0);
            putBitInPosition(&result, 1, 9);
            break;
        case 'T':
            putBitInPosition(&result, 1, 9);
            break;
        case '-':
            break;
        default:
            *err=-1;
            return 0;
            break;
    }
    switch (string[5]) // rwxrw?rwx 000 000 000 000
    {
        case 'x':
            putBitInPosition(&result, 1, 3);
            break;
        case 's':
            putBitInPosition(&result, 1, 3);
            putBitInPosition(&result, 1, 10);
            break;
        case 'S':
            putBitInPosition(&result, 1, 10);
            break;
        case '-':
            break;
        default:
            *err=-1;
            return 0;
            break;
    }
    switch (string[2]) // rw?rwxrwx
    {
        case 'x':
            putBitInPosition(&result, 1, 6);
            break;
        case 's':
            putBitInPosition(&result, 1, 6);
            putBitInPosition(&result, 1, 11);
            break;
        case 'S':
            putBitInPosition(&result, 1, 11);
            break;
        case '-':
            break;
        default:
            *err=-1;
            return 0;
            break;
    }

    if      (string[0]=='r') { putBitInPosition(&result, 1, 8);}
    else if (string[0]=='-') { ; }
    else                     { *err=-1; return 0;}

    if      (string[1]=='w') { putBitInPosition(&result, 1, 7);}
    else if (string[1]=='-') { ; }
    else                     { *err=-1;  return 0;}

    if      (string[3]=='r') { putBitInPosition(&result, 1, 5);}
    else if (string[3]=='-') { ; }
    else                     { *err=-1;  return 0;}

    if      (string[4]=='w') { putBitInPosition(&result, 1, 4);}
    else if (string[4]=='-') { ; }
    else                     { *err=-1;  return 0;}

    if (     string[6]=='r') { putBitInPosition(&result, 1, 2);}
    else if (string[6]=='-') { ; }
    else                     { *err=-1;  return 0;}

    if      (string[7]=='w') { putBitInPosition(&result, 1, 1);}
    else if (string[7]=='-') { ; }
    else                     { *err=-1;  return 0;}

    return result;
}

//принимает  строку вида 7777
BitmapType octStringToBits(const char* string, int* err)
{
    BitmapType result = 0;
    for (int i = 0; string[i]!='\0'; i++)
    {
        if (string[i] != '0' && string[i] != '1' && string[i] != '2' && 
            string[i] != '3' && string[i] != '4' && string[i] != '5' && 
            string[i] != '6' && string[i] != '7')
        {
            *err=-1;
            return 0;
        }

        char dig = string[i]-'0';
        result= (result<<3) | dig;
    }
    return result;
}

int bitsToOctString(BitmapType bits, char* result)
{
        char temp[5] = {'\0'};
        int idx=0;
        while (bits > 0 && idx < (int)sizeof(temp))
        {
            int digit = bits & 0x7;
            temp[idx++] = '0' + digit;
            bits >>= 3;
        }
        for (int i=0; i < idx; i++)
        {
            result[i] = temp[idx-1-i];
        }
        return 0;
}

int bitsToString(BitmapType bits, char* result)
{
    result[0] = (bits & (1 << 8)) ? 'r' : '-';
    result[1] = (bits & (1 << 7)) ? 'w' : '-';
    result[2] = (bits & (1 << 11)) ? 's' :
                (bits & (1 << 6))  ? 'x' : '-';

    result[3] = (bits & (1 << 5)) ? 'r' : '-';
    result[4] = (bits & (1 << 4)) ? 'w' : '-';
    result[5] = (bits & (1 << 10)) ? 's' :
                (bits & (1 << 3))  ? 'x' : '-';

    result[6] = (bits & (1 << 2)) ? 'r' : '-';
    result[7] = (bits & (1 << 1)) ? 'w' : '-';
    result[8] = (bits & (1 << 9)) ? 't' :
                (bits & (1 << 0)) ? 'x' : '-';

    if (!(bits & (1 << 6)) && (bits & (1 << 11)))
        result[2] = 'S';

    if (!(bits & (1 << 3)) && (bits & (1 << 10)))
        result[5] = 'S';

    if (!(bits & (1 << 0)) && (bits & (1 << 9)))
        result[8] = 'T';

    result[9] = '\0';

    return 0;
}