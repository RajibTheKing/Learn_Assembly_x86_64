#include "assembly_wrapper.h"

#define ASM(...) asm( #__VA_ARGS__ )



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
    unsigned int result = 5;
    //try inline assembly

    ASM(cmp $0, %rcx;
        loop_label:
            cmp $0, %rcx;
            je return_match;
            sub $8, %rcx;
            movq (%rax), %xmm10;
            movq (%rsi), %xmm11;
            add $8, %rax;
            add $8, %rsi;

            pcmpistrm $0x18, %xmm10, %xmm11;
            movq %xmm0, %r8;
            cmp $0x00, %r8;
            je loop_label;
            jne return_mismatch;

        return_mismatch:
            movq $0x01, result;

        return_match:
            movq $0x00, result;
        : // output operands
                "=m" (str1),         // %0
                "=m" (str2),         // %1
                "=r" (result)        // %2
        : // input operands
            "m" (str1Len),          // %3
            "m" (str2Len),          // ...
            "r" (result)
        : // clobber
            "r15", "rcx", "xmm0", "xmm1", "xmm5", "xmm6", "xmm7", "memory"

    );

    //result = compare_string(str1, str2, str1Len, str2Len);

    return result;

}
