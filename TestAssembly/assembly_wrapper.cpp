#include "assembly_wrapper.h"



unsigned int AssemblyWrapper::getSum(unsigned int &a, unsigned int &b)
{
    return getsum_x86_64_GAS(a, b); 
}