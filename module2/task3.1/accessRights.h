#ifndef ACCESS_RIGHTS_H
#define ACCESS_RIGHTS_H

#define BitmapType unsigned short int

void printBinaryMask(BitmapType n);
BitmapType stringToBits(const char* string, int* err);
BitmapType octStringToBits(const char* string, int* err);
int bitsToOctString(BitmapType bits, char* result);
int bitsToString(BitmapType bits, char* result);

#endif