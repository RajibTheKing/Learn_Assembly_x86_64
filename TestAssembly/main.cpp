#include "assembly_wrapper.h"
#include "common.h"


void printArrayInHex(unsigned char *data, int len)
{
    for(int i=0; i<len; i++){
        printf("0x%02X ", data[i]);
    }
    printf("\n");
}

unsigned char* getRandomString(int len){
    unsigned char *str = new unsigned char[len];
    for(int i=0; i<len; i++){
        int rand_now = (rand() % 26) + 'a';
        str[i] = rand_now;
    }
    return str;
}

unsigned char* getDeepcopyString(unsigned char * str, unsigned int len){
    unsigned char *deepCopyStr = new unsigned char[len];
    memcpy(deepCopyStr, str, len);
    return deepCopyStr;
}

unsigned int compareCharByChar(unsigned char *str1, unsigned char* str2, unsigned int str1len, unsigned int str2len)
{
    //in-efficient and Naive string compare

    for(unsigned int i=0; i<str1len; i++)
    {
        if(str1[i] != str2[i])
            return 1; //mismatch found
    }

    return 0; // no mismatch
}

int main()
{
    // Using system time as a seed value
    srand((unsigned) time(NULL));

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
    printf("\nTesting: PACKSSWB/PACKSSDW\n");
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

    printf("\nTesting: PCMPISTRI\n");
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


    printf("\nTesting: string comparison\n");
    unsigned int str1Len = 8 * 10000;
    unsigned int str2Len = 8 * 10000;
    unsigned char *str1 = getRandomString(str1Len);
    unsigned char *str2 = getDeepcopyString(str1, str2Len);
    //str2[str2Len-5] = '$'; //changing single byte
    printf("Address of str1  = %p\n", str1);
    printf("Address of str2  = %p\n", str2);
    //unsigned char *str2 = getRandomString(str2Len);
    //printf("str1 = %s\n", str1);
    //printf("str2 = %s\n", str2);

    // Start measuring time
    auto begin = std::chrono::high_resolution_clock::now();
    auto cmpRes = std::strcmp(reinterpret_cast<const char*>(str1), reinterpret_cast<const char*>(str2));
    //auto cmpRes = assemblyWrapper->compareString(str1, str2, str1Len, str2Len);
    //auto cmpRes = compareCharByChar(str1, str2, str1Len, str2Len);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("compare(str1, str2) = %d\n", cmpRes);
    std::cout<<"Execution time measured: " << elapsed.count() << " Nanoseconds" << std::endl;
    //Fact: 10 million characters(both str1 and str2 same length) std::strcmp takes only 850000 Nanoseconds(avg.) to compare
    delete[] str1;
    delete[] str2;



    return 0;
}





