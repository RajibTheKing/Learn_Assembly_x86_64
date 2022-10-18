#include "assembly_wrapper.h"



unsigned int AssemblyWrapper::getSum(unsigned int &a, unsigned int &b)
{
    //return getsum_x86_64(a, b);
    return getsum_x86_64_GAS(a, b);
}

void AssemblyWrapper::packedSaturationTest(unsigned char *inData, unsigned char *outData)
{
    packed_suturation_test(inData, outData);
}


unsigned int AssemblyWrapper::searchSubstr(unsigned char *inStr, unsigned char *subStr)
{
    return search_substr(inStr, subStr);
    //return 0;
}
