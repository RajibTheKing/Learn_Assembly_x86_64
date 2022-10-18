#ifndef ASSEMBLY_WRAPPER_H
#define ASSEMBLY_WRAPPER_H

#include "common.h"

extern "C"
{
    unsigned int getsum_x86_64(unsigned int &a, unsigned int &b);
    unsigned int getsum_x86_64_GAS(unsigned int &a, unsigned int &b);
    void packed_suturation_test(unsigned char *inData, unsigned char *outData);
    unsigned int search_substr(unsigned char *inStr, unsigned char *subStr);
}

class AssemblyWrapper{
public:
    unsigned int getSum(unsigned int &a, unsigned int &b);
    void packedSaturationTest(unsigned char *inData, unsigned char *outData);
    unsigned int searchSubstr(unsigned char *inStr, unsigned char* subStr);

};

#endif
