#include "assembly_wrapper.h"

#define ASM(...) asm( #__VA_ARGS__




unsigned int AssemblyWrapper::getSum(unsigned int &a, unsigned int &b)
{
    //return getsum_x86_64(a, b);
    //return getsum_x86_64_GAS(a, b);
    unsigned int sum;
    ASM(
      movl %1, %%eax;
      add %2, %%eax;
      movl %%eax, %0;
      )
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
    //return compare_string(str1, str2, str1Len, str2Len);

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


}

int AssemblyWrapper::compareStringCaseinsensitive(const char *lhs, const char *rhs, unsigned int len)
{
    // return compare_string_case_insensitive(str1, str2, len);

    ///< Assembly Implementation with SSE instructions
    int result;

    ASM(    ///< Prepare constant CHARACTER_RANGE
            movq        $0x5A41,        %%r12;                  // A: 0x41, Z: 0x5A --> defining Range
            movq        %%r12,          %%xmm12;                // CHARACTER_RANGE: xmm12, move quad word(64 bit) r12 to xmm12 to perform SSE instructions

            ///< Prepare constant CHARACTER_DIFF
            movq        $0x20202020,    %%r12;                  // Diff = 'a' - 'A' = 32 = x020, Prepare a doubleword value
            movq        %%r12,          %%xmm13;                // moving diff to SSE register
            pshufd      $0,             %%xmm13,    %%xmm13;    // CHARACTER_DIFF: xmm13, double word (32 bit) will be broadcasts to 128 bit like 0x20202020 20202020 20202020 20202020

        head_loop:
            cmp         $16,            %3;                     // check if all characters are compared
            jl          tail_loop;                              // all characters were matched so far. i.e. len = 37, 32 bytes are checked inside head_loop, 5 bytes will be checked inside tail_loop
            sub         $16,            %3;                     // substruct len 16, meaning 16 bytes are going to be checked inside this head_loop
            movdqu      (%1),           %%xmm10;                // move double quad word (128 bit) of lhs to xmm10 register(SSE)
            movdqu      (%2),           %%xmm11;                // move double quad word (128 bit) of rhs to xmm11 register(SSE)
            add         $16,            %1;                     // skip the pointer by 16 bytes
            add         $16,            %2;                     // skip the pointer by 16 bytes

            ///< Converting lhs to Lower
            pcmpistrm   $0x44,          %%xmm10,    %%xmm12;    // MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
            pand        %%xmm13,        %%xmm0;                 // TO_ADD: CHARACTER_DIFF & MASK
            movdqu      %%xmm10,        %%xmm14;                // COPY: xmm10 --> xmm14
            paddb       %%xmm0,         %%xmm14;                // LOWER: TO_ADD + COPY --> xmm14

            ///< Converting rhs to Lower
            pcmpistrm   $0x44,          %%xmm11,    %%xmm12;    // MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
            pand        %%xmm13,        %%xmm0;                 // TO_ADD: CHARACTER_DIFF & MASK
            movdqu      %%xmm11,        %%xmm15;                // COPY: xmm11 --> xmm15
            paddb       %%xmm0,         %%xmm15;                // LOWER: TO_ADD + COPY --> xmm15


            ///< Check lhs and rhs are identical or not
            pcmpistrm   $0x18,          %%xmm14,    %%xmm15;    // compare two sse register completely equal or not
            movq        %%xmm0,         %%r8;                   // move the result of xmm0 register to r8 register (temp) to perform cmp instruction
            cmp         $0x00,          %%r8;                   // check the output after pcmpistrm compare
            je          head_loop;
            jne         prepare_intermediate_mismatch;          // Here we already found a mismatch inside head_loop, need to check one by one again to return the result


        prepare_intermediate_mismatch:
            sub         $16,            %1;
            sub         $16,            %2;
            add         $16,            %3;
            jmp tail_loop;

        tail_loop:
            cmp         $0,             %3;
            je return_result_match;
            movq        (%1),           %%r13;
            movq        (%2),           %%r14;
            sub         $1,             %3;
            add         $1,             %1;
            add         $1,             %2;

            ///< convert single byte from str1 to lower
        str1:
            cmp $0x41, %%r13b;
            jl str2;
            cmp $0x5A, %%r13b;
            jg str2;
            add $32, %%r13b;

            ///< convert single byte from str2 to lower
        str2:
            cmp $0x41, %%r14b;
            jl compare;
            cmp $0x5A, %%r14b;
            jg compare;
            add $32, %%r14b;

        compare:
            sub %%r14b, %%r13b;
            cmp $0, %%r13b;
            jne return_result_mismatch;
            je tail_loop;

        return_result_mismatch:
            and $0xFF, %%r13;
            movl %%r13d, %0;
            jmp end_func;

        return_result_match:
            movl $0, %0;
        end_func:
        )
        : ///< output operands
            "=r" (result)                                       // %0
        : ///< input operands
            "r" (lhs),                                          // %1
            "r" (rhs),                                          // %2
            "r" (len)                                           // %3
        : ///< clobber
            "%xmm0", "%xmm10", "%xmm11", "%xmm12", "%xmm13","%xmm14", "%xmm15", "%r8", "%r12", "%r14", "%r13", "memory"

    );
    ///< Signed byte Value Range: -128...0....+127, following statement is going to convert unsigned value to signed value
    return result > 127 ? result - 256 : result;

}
