#ifndef UTILITY_H
#define UTILITY_H
#include "common.h"

class Utility
{
public:
    Utility();
    void printArrayInHex(unsigned char *data, int len);
    unsigned char* getRandomString(int len);
    unsigned char* getDeepcopyString(unsigned char * str, unsigned int len);
    unsigned int compareCharByChar(unsigned char *str1, unsigned char* str2, unsigned int str1len, unsigned int str2len);
};

#endif // UTILITY_H
