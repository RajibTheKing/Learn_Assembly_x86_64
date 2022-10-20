#include "assembly_wrapper.h"

#define ASM(...) asm( #__VA_ARGS__ )



unsigned int AssemblyWrapper::getSum(unsigned int &a, unsigned int &b)
{
    //return getsum_x86_64(a, b);
    //return getsum_x86_64_GAS(a, b);
    unsigned int sum;
    asm ("movl %1, %%eax;"
         "add %2, %%eax;"
         "movl %%eax, %0;"
         :"=r"(sum)             /* output */
         :"r"(a),"r"(b)         /* input */
         :"%eax"                /* clobbered register */
         );

    return sum;
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
    /*
    unsigned int result=10;
    //try inline assembly

    asm("movl %3, %%ecx;"
        "loop_label:\n"
        "cmp $0, %%rcx;"
        "je return_result;"
        "sub $16, %%rcx;"
        "movdqu (%1), %%xmm10;"
        "movdqu (%2), %%xmm11;"
        "add $16, %1;"
        "add $16, %2;"
        "pcmpistrm $0x18, %%xmm10, %%xmm11;"
        "movq %%xmm0, %%rdx;"
        "cmpq $0, %%rdx;"
        "jne return_result;"
        "je loop_label;"
        "return_result:\n"
        "movl %%edx, %0;"
        : // output operands
            "=r" (result)        // %0
        : // input operands
            "r" (str1),          // %1
            "r" (str2),          // %2
            "r" (str1Len),       // %3
            "r" (str2Len)        // %4
        : // clobber
            "%xmm0", "%xmm10", "%xmm11", "%rcx", "%rdx", "memory"

    );

    return result;
    */

}
