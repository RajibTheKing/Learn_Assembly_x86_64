#ifndef ASSEMBLY_WRAPPER_H
#define ASSEMBLY_WRAPPER_H

#include <stdio.h>

extern "C"
{
    unsigned int getsum_x86_64(unsigned int &a, unsigned int &b);
    unsigned int getsum_x86_64_GAS(unsigned int &a, unsigned int &b);
    void packed_suturation_test(unsigned char *inData, unsigned char *outData);
}

class AssemblyWrapper{
public:
    unsigned int getSum(unsigned int &a, unsigned int &b);
    void packedSaturationTest(unsigned char *inData, unsigned char *outData);

};

#endif
