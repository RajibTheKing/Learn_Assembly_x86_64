#include "assembly_wrapper.h"
#include "common.h"


void printArrayInHex(unsigned char *data, int len)
{
    for(int i=0; i<len; i++){
        printf("0x%02X ", data[i]);
    }
    printf("\n");
}

int main()
{
    AssemblyWrapper *assemblyWrapper = new AssemblyWrapper();

    /* TEST: ADD
    *  Syntex: add         %rdx,     %rax
    */

    printf("Testing: ADD\n");
    unsigned int a = 20;
    unsigned int b = 15;
    //expected result = 35

    printf("a = %d\n", a);
    printf("b = %d\n", b);
    unsigned int result = assemblyWrapper->getSum(a, b);
    printf("Sum = %d\n", result);


    /*  TEST: PACKSSWB/PACKSSDW
    *   Syntex: packsswb    %xmm9,      %xmm10
    */
    printf("Testing: PACKSSWB/PACKSSDW\n");
//    //128 bits
    unsigned char inData[]  = {0xFF, 0x00, 0xDA, 0x00, 0x37, 0x00, 0x54, 0x00, 0x45, 0x00, 0xEA, 0x00, 0x23, 0x00, 0xFA, 0x00};

//    //128 bits
    unsigned char outData[] = {0x56, 0x00, 0x23, 0x00, 0x67, 0x00, 0xAB, 0x00, 0x47, 0x00, 0x2A, 0x00, 0xE2, 0x00, 0x88, 0x00};

//    //unsigned char outData[] = {0x56, 0x23, 0x67, 0xAB, 0x47, 0x2A, 0xE2, 0x88,     0xFF, 0xDA, 0x37, 0x54, 0x45, 0xEA, 0x23, 0xFA};

//     doesn't work probably because of the overflows

    //128 bits
    //unsigned char inData[]  = {0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00};

    //128 bits
    //unsigned char outData[] = {0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00};



    //expected
    //unsigned char outData[] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};


    printf("inData = "); printArrayInHex(inData, 16);
    printf("outData = "); printArrayInHex(outData, 16);
    assemblyWrapper->packedSaturationTest(inData, outData);
    printf("outData = "); printArrayInHex(outData, 16);


    /*  TEST: PCMPISTRI/PCMPISTRM
    *   Syntex: PCMPISTRM  $0x58,  %xmm9,     %xmm10
    *   Let's try to find the location by given substring
    */

    printf("Testing: PCMPISTRI\n");
    unsigned char inStr[] = "this is a joke!!";
    unsigned char subStr[] = "is";
    printf("inStr = %s\n", inStr);
    printf("subStr = %s\n", subStr);
    unsigned int resStr = assemblyWrapper->searchSubstr(inStr, subStr);
    //  imm8 = 0x4c
    //  output: 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00
    //  imm8 = 0x0c
    //  output: 0000000000100100
    printf("resStr = %02X\n", resStr);

    return 0;
}





