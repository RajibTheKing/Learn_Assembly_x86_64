#include "analyzesolution.h"

AnalyzeSolution::AnalyzeSolution()
{
    assemblyWrapper = new AssemblyWrapper();
    utility = new Utility();
}

AnalyzeSolution::~AnalyzeSolution()
{
    delete assemblyWrapper;
    delete utility;
}

void AnalyzeSolution::startAnaylizing()
{
    //this->analyzeAdd();

    //this->analyzeSaturation();

    //this->analyzeSubstringSearch();

    //this->analyzeStringCompare();

    this->analyzeStringCompareCaseinsensitive();

}

/* TEST: ADD
*  Syntex: add         %rdx,     %rax
*/
void AnalyzeSolution::analyzeAdd()
{
    printf("\nTesting: ADD\n");
    unsigned int a = 20;
    unsigned int b = 15;
    //expected result = 35

    printf("a = %d\n", a);
    printf("b = %d\n", b);
    unsigned int result = assemblyWrapper->getSum(a, b);
    printf("Sum = %d\n", result);
}


/*  TEST: PACKSSWB/PACKSSDW
*   Syntex: packsswb    %xmm9,      %xmm10
*/
void AnalyzeSolution::analyzeSaturation()
{

    printf("\nTesting: PACKSSWB/PACKSSDW\n");

    //128 bits
    //unsigned char inData[]  = {0xFF, 0x00, 0xDA, 0x00, 0x37, 0x00, 0x54, 0x00, 0x45, 0x00, 0xEA, 0x00, 0x23, 0x00, 0xFA, 0x00};

    //128 bits
    //unsigned char outData[] = {0x56, 0x00, 0x23, 0x00, 0x67, 0x00, 0xAB, 0x00, 0x47, 0x00, 0x2A, 0x00, 0xE2, 0x00, 0x88, 0x00};

    //unsigned char outData[] = {0x56, 0x23, 0x67, 0xAB, 0x47, 0x2A, 0xE2, 0x88,     0xFF, 0xDA, 0x37, 0x54, 0x45, 0xEA, 0x23, 0xFA};

    //doesn't work probably because of the overflows

    //128 bits
    unsigned char inData[]  = {0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00};

    //128 bits
    unsigned char outData[] = {0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00};



    //expected
    //unsigned char outData[] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};


    printf("inData = "); utility->printArrayInHex(inData, 16);
    printf("outData = "); utility->printArrayInHex(outData, 16);
    assemblyWrapper->packedSaturationTest(inData, outData);
    printf("outData = "); utility->printArrayInHex(outData, 16);
}


/*  TEST: PCMPISTRI/PCMPISTRM
*   Syntex: PCMPISTRM  $0x58,  %xmm9,     %xmm10
*   Let's try to find the location by given substring
*/

void AnalyzeSolution::analyzeSubstringSearch()
{

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
}

/*  TEST: string compare (case Sensitive)
*   Syntex: PCMPISTRM  $0x58,  %xmm9,     %xmm10
*   Let's try to check if two strings are equal or not
*/
void AnalyzeSolution::analyzeStringCompare()
{
    printf("\nTesting: string comparison\n");
    unsigned int str1Len = 8 * 10000;
    unsigned int str2Len = 8 * 10000;
    unsigned char *str1 = utility->getRandomString(str1Len);
    unsigned char *str2 = utility->getDeepcopyString(str1, str1Len);
    //str2[str2Len-5] = '$'; //changing single byte
    printf("Address of str1  = %p\n", str1);
    printf("Address of str2  = %p\n", str2);
    //unsigned char *str2 = getRandomString(str2Len);
    //printf("str1 = %s\n", str1);
    //printf("str2 = %s\n", str2);

    // Start measuring time
    auto begin = std::chrono::high_resolution_clock::now();
    //auto cmpRes = std::strcmp(reinterpret_cast<const char*>(str1), reinterpret_cast<const char*>(str2));
    auto cmpRes = assemblyWrapper->compareString(str1, str2, str1Len, str2Len);
    //auto cmpRes = compareCharByChar(str1, str2, str1Len, str2Len);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("compare(str1, str2) = %d\n", cmpRes);
    std::cout<<"Execution time measured: " << elapsed.count() << " Nanoseconds" << std::endl;
    //Fact: 10 million characters(both str1 and str2 same length) std::strcmp takes only 850000 Nanoseconds(avg.) to compare
    delete[] str1;
    delete[] str2;
}


/*  TEST: string compare (case Insensitive)
*   Let's try to check if two strings are identical or not
*/
void AnalyzeSolution::analyzeStringCompareCaseinsensitive()
{
    printf("\nTesting: string comparison (Case-Insensitive)\n");
    //while(true)
    {
        unsigned int len = rand() % (8*100000);
        printf("String Len selected = %d\n", len);
        unsigned char *str1 = utility->getRandomString(len);
        unsigned char *str2 = utility->getDeepcopyStringRandomizeCase(str1, len);
        unsigned int randomByte = rand() % len;
        str2[randomByte] = '$'; //changing single byte
        //printf("Address of str1  = %p\n", str1);
        //printf("Address of str2  = %p\n", str2);
        //unsigned char *str2 = getRandomString(str2Len);
        if(len <=100)
        {
            printf("str1 = %s\n", str1);
            printf("str2 = %s\n", str2);
        }

        // Start measuring time
        auto begin1 = std::chrono::high_resolution_clock::now();
        //Source: https://linux.die.net/man/3/strcasecmp
        auto cmpRes1 = strncasecmp(reinterpret_cast<const char*>(str1), reinterpret_cast<const char*>(str2), len);
        auto end1 = std::chrono::high_resolution_clock::now();
        auto elapsed1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end1 - begin1);


        auto begin2 = std::chrono::high_resolution_clock::now();
        auto cmpRes2 = utility->compareCharByCharCaseInsensitive(str1, str2, len);

        auto end2 = std::chrono::high_resolution_clock::now();
        auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end2 - begin2);

        auto begin3 = std::chrono::high_resolution_clock::now();
        auto cmpRes3 = assemblyWrapper->compareStringCaseinsensitive(str1, str2, len);
        auto end3 = std::chrono::high_resolution_clock::now();
        auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end3 - begin3);


        printf("strcasecmp = %d\n", cmpRes1);
        printf("Naive      = %d\n", cmpRes2);
        printf("assembly   = %d\n", cmpRes3);
        std::cout<<"Execution time measured: " << elapsed1.count() << " Nanoseconds" << std::endl;
        std::cout<<"Execution time measured: " << elapsed2.count() << " Nanoseconds" << std::endl;
        std::cout<<"Execution time measured: " << elapsed3.count() << " Nanoseconds" << std::endl;

        delete[] str1;
        delete[] str2;

        std::string estStr;
        getline(std::cin, estStr);
        getchar();
    }
}



