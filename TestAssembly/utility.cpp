#include "utility.h"

Utility::Utility()
{
    // Using system time as a seed value
    srand((unsigned) time(NULL));
}

void Utility::printArrayInHex(unsigned char *data, int len)
{
    for(int i=0; i<len; i++){
        printf("0x%02X ", data[i]);
    }
    printf("\n");
}


unsigned char* Utility::getRandomString(int len){
    unsigned char *str = new unsigned char[len + 1];
    int offset;
    for(int i=0; i<len; i++){
        int rand_case = (rand() % 2);
        rand_case ? offset = 'a' : offset = 'A';
        int rand_now = (rand() % 26) + offset;
        str[i] = rand_now;
//        printf("rand_now = %c\n", rand_now);
    }
    str[len] = 0; //null terminator
    return str;
}

unsigned char* Utility::getDeepcopyString(unsigned char * str, unsigned int len){
    unsigned char *deepCopyStr = new unsigned char[len];
    memcpy(deepCopyStr, str, len);
    return deepCopyStr;
}

unsigned char* Utility::getDeepcopyStringRandomizeCase(unsigned char * str, unsigned int len){
    unsigned char *deepCopyStr = new unsigned char[len];
    memcpy(deepCopyStr, str, len);

    for(unsigned int i=0; i<len;i++){
        int rand_case = rand() %2;
        rand_case ? deepCopyStr[i] = toupper(deepCopyStr[i]) : tolower(deepCopyStr[i]);
    }
    return deepCopyStr;
}

unsigned int Utility::compareCharByChar(unsigned char *str1, unsigned char* str2, unsigned int str1len, unsigned int str2len)
{
    //in-efficient and Naive string compare

    for(unsigned int i=0; i<str1len; i++)
    {
        if(str1[i] != str2[i])
            return 1; //mismatch found
    }

    return 0; // no mismatch
}

