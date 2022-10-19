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
}

unsigned int AssemblyWrapper::compareString(unsigned char *str1, unsigned char *str2, unsigned int str1Len, unsigned int str2Len)
{
    return compare_string(str1, str2, str1Len, str2Len);
}
