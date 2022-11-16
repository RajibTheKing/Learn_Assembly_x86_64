#include "analyzesolution.h"
#include "cType.inl"

AnalyzeSolution::AnalyzeSolution()
{
    utility = new Utility();
}

AnalyzeSolution::~AnalyzeSolution()
{
    delete utility;
}

void AnalyzeSolution::startAnaylizing()
{
    //this->analyzeAdd();

    //this->analyzeSaturation();

    //this->analyzeSubstringSearch();

    //this->analyzeStringCompare();

    this->analyzeStringCompareCaseinsensitive();

    //this->analyzeMovdqa();

}

void AnalyzeSolution::showProgressBar(int totalElements, int alreadyCompleted)
{
    /// show progress bar
    std::cout << "[";
    int numberOfBarsToShow = (int)(alreadyCompleted * 100.0 / totalElements);
    for(int i=0; i<numberOfBarsToShow; i++){
        std::cout<<"=";
    }
    for(int i=0; i<(100-numberOfBarsToShow); i++){
        std::cout<<" ";
    }
    std::cout<<"] ";
    std::cout<<numberOfBarsToShow<<" %\r";
    std::cout.flush();

}

std::string AnalyzeSolution::getNameBySolution(int n){
    switch (n) {
    case 0:
        return "strcasecmp";
        break;
    case 1:
        return "C++ Naive Solution";
        break;
    case 2:
        return "assembly ";
        break;
    case 3:
        return "assembly without ordering";
        break;
    case 4:
        return "assembly without ordering v2";
        break;
    default:
        return "No Solution by this number";
        break;
    }
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

    int number_of_solutions = 5;

    //Initialize Function Pointer for all solutions
    int (*solutions[number_of_solutions])(const char *, const char *, size_t);
    solutions[0] = strncasecmp;
    solutions[1] = compareCharByCharCaseInsensitive;
    solutions[2] = compare_string_case_insensitive;
    solutions[3] = i_case_compare;
    solutions[4] = i_case_compare_v2;


    long long totalTime[number_of_solutions];
    for(int i=0; i<number_of_solutions; i++){
        totalTime[i]  = 0;
    }
    long long totalLength = 0;

    int results[number_of_solutions];

    int testCase = 1000;
    int kase = 0;



    while(kase++ < testCase)
    {
        /// Generate two strings with random characters
        unsigned int len = 1000 + rand() % 16;
        totalLength+=len;
//        printf("String Len selected = %d\n", len);
        unsigned char *str1 = utility->getRandomString(len);
        unsigned char *str2 = utility->getDeepcopyStringRandomizeCase(str1, len);

        /// change a random byte to make expected equal false
        unsigned int randomByte = rand() % len;
        int offset = 5 - (rand() % 10);
        printf("Index: %d, Changing Single byte from %c to %c\n", randomByte, str2[randomByte], str2[randomByte] + offset);
        str2[randomByte] += offset;

        /// print the starting address of both strings
        printf("Address of str1  = %p\n", str1);
        printf("Address of str2  = %p\n", str2);

        /// if the length is less then 100 then show the strings on console output
        if(len <= 100)
        {
            printf("str1 = %s\n", str1);
            printf("str2 = %s\n", str2);
        }

        /// Start Checking the Results and Measuring Execution time for each solution
        for(int i=0; i<number_of_solutions; i++)
        {
            auto begin = std::chrono::high_resolution_clock::now();
            auto cmpRes = solutions[i](reinterpret_cast<const char*>(str1), reinterpret_cast<const char*>(str2), len);
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
            std::cout<<"Execution time measured: " << elapsed.count() << " Nanoseconds ("<<getNameBySolution(i)<<") --> "<<cmpRes<< std::endl;

            totalTime[i] += elapsed.count();
            results[i] = cmpRes;
            results[i] = results[i] > 127 ? results[i] - 256 : results[i];

        }

        if(results[0] == results[1] && results[1] == results[2] &&
                (results[0] == 0? results[3] == 0: results[3] != 0) && (results[3] == results[4]) ){
            printf("Case: %d --> Result MATCH!!\n", kase);
        }else{
            printf("Case: %d --> MISMATCH FOUND!!!!\n", kase);
            break;
        }


//        auto begin1 = std::chrono::high_resolution_clock::now();
//        // Source: https://linux.die.net/man/3/strcasecmp
//        auto cmpRes1 = strncasecmp(reinterpret_cast<const char*>(str1), reinterpret_cast<const char*>(str2), len);
//        auto end1 = std::chrono::high_resolution_clock::now();
//        auto elapsed1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end1 - begin1);


        /// show the strings again to check if the characters are modified after any funciton calls
        if(len <=100)
        {
            printf("str1 = %s\n", str1);
            printf("str2 = %s\n", str2);
        }

        // de-allocate memories
        delete[] str1;
        delete[] str2;

//        /// show progress bar
//        showProgressBar(testCase, kase);
    }

    printf("\nTotal Char Length checked: %lld\n", totalLength);
    for(int i=0; i<number_of_solutions; i++){
        printf("Total time needed %lld --> (%s)\n", totalTime[i], getNameBySolution(i).c_str());
    }


}

void AnalyzeSolution::analyzeMovdqa()
{
    std::string_view lhs{"once upon a time in germany"}; ///< lhs length = 27
    std::string_view rhs = lhs;//lhs.substr(20, 5);
    unsigned int len = 5;
    auto result = assemblyWrapper->testMovdqa(lhs.data(), rhs.data(), len);
    printf("Result = %d\n", result);

//    const char *a = lhs.data();
//    const char *b = rhs.data();

//    std::cout<<&*a<<std::endl;
//    std::cout<<&*b<<std::endl;

//    unsigned char *lhs = new unsigned char[16];
//    unsigned char *rhs = lhs + 10;
//    int len = 5;

//    int len = 5;
//    void *lhs = mmap ( NULL, 16*sizeof(unsigned char), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
//    void *rhs = lhs;

//    std::string str1 = "Once Upon A Time in Germany";
//    memcpy(lhs, str1.c_str(), 16);
//    printf("%s\n", (unsigned char *)lhs);
//    auto result = assemblyWrapper->testMovdqa(reinterpret_cast<const char*>(lhs), (reinterpret_cast<const char*>(rhs) + 10), len);

//    int testCases = 10000;
//    int kase = 0;

//    while(kase < testCases)
//    {
//        kase++;

//        int len = 5;
//        unsigned char *lhs = new unsigned char[1];
//        unsigned char *rhs = lhs + 16;
//        auto result = assemblyWrapper->testMovdqa(reinterpret_cast<const char*>(lhs), reinterpret_cast<const char*>(rhs), len);
//        printf("Case: %d --> Result = %d\n", kase, result);
//        //delete[] lhs;
//    }

//    int testCases = 1000;
//    int kase = 0;
//    long address = 100; //< It's just a value
//    long *p = &address;

//    while(kase < testCases)
//    {
//        kase++;
//        int len = 5;
//        p  = p + rand() % 1000;
//        printf("Specify Address: %p\n", p);
//        void *lhs = mmap ( p, 1*sizeof(unsigned char), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
//        void *rhs = lhs;
//        rhs = static_cast<char*>(rhs) + 20;

//        auto result = assemblyWrapper->testMovdqa(reinterpret_cast<const char*>(lhs), reinterpret_cast<const char*>(rhs), len);
//        printf("Case: %d --> Result = %d\n", kase, result);
//        //delete[] lhs;
//    }
}


