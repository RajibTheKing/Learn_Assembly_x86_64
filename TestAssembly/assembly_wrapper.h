#ifndef ASSEMBLY_WRAPPER_H
#define ASSEMBLY_WRAPPER_H

#include <stdio.h>

extern "C"
{
    unsigned int getsum_x86_64(unsigned int &a, unsigned int &b);
    unsigned int getsum_x86_64_GAS(unsigned int &a, unsigned int &b);
}

class AssemblyWrapper{
public:
    unsigned int getSum(unsigned int &a, unsigned int &b);

};

#endif