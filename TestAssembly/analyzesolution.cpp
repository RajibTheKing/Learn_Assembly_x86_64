#include "analyzesolution.h"
#include "cType.inl"
#include "custom_random.h"
#include "assembly_wrapper.h"

namespace
{
    void printValues(std::string_view prefix, const std::string& str1, const std::string& str2)
    {
        std::cout
            << prefix << std::endl
            << "\tstr1:" << str1 << std::endl
            << "\tstr2:" << str2 << std::endl
            << std::endl << std::endl;
    }
}

void AnalyzeSolution::startAnaylizing()
{
    analyzeStringCompareCaseinsensitive();
}

/*  TEST: string compare (case Insensitive)
*   Let's try to check if two strings are identical or not
*/
void AnalyzeSolution::analyzeStringCompareCaseinsensitive()
{
    auto* lalla1 = "zOLhmlfPPhzuDbJhDeGDJnXmIvupZWBsA";
    auto* lalla2 = "zOLhMlFPPHzuDbJhDEGDJNXmIvUpZWBSA";
    printValues("lalla1 & lalla2: ", lalla1, lalla2);
    auto firstRet = ___i_case_compare_ordering(lalla1, lalla2, std::strlen(lalla1));
    std::cout << "firstRet: "<<firstRet << std::endl;
    printValues("lalla1 & lalla2 (after):", lalla1, lalla2);

    auto* eee = "zOLhmlfPPhzuDbJhDeGDJnXmIvupZWBsA";
    auto* ddd = "zOLhMlFPPHzuDbJhDEGDJNXmIvUpZWBSA";
    printValues("eee & ddd: ", eee, ddd);
    auto secondRet = ___i_case_compare_ordering(eee, ddd, std::strlen(eee));
    printValues("eee & ddd (after): ", eee, ddd);
    std::cout<<"secondRet: "<<secondRet<<std::endl;

    std::cout << "Testing: string comparison (Case-Insensitive)" << std::endl;

    //Initialize Function Pointer for all solutions
    std::vector<std::function<int(const char *, const char *, size_t)>> solutions;

    solutions.emplace_back(compareCharByCharCaseInsensitive);
    solutions.emplace_back(___i_case_compare_ordering);
//    solutions.emplace_back(___i_case_compare);
//    solutions.emplace_back(i_case_compare_v3);
//    solutions.emplace_back(i_case_compare_v4);

    int number_of_solutions = solutions.size();

    std::vector<long long> totalTime;
    totalTime.reserve(number_of_solutions);


    for(int i=0; i<number_of_solutions; i++){
        totalTime[i]  = 0;
    }
    int testCase = 2000;

    for(auto kase = 1; kase <= testCase; ++kase)
    {
        std::cout << "TEST CASE: " << kase << std::endl;

         /// Generate two strings with random characters
        unsigned int len = 1 + ANG::tools::random::next() % 33;

        std::cout << "String Len selected = " << len << std::endl;
        auto str1 = Utility::getRandomString(len);
        auto str2 = Utility::getDeepcopyStringRandomizeCase(str1, len);

//        /// change a random byte to make expected equal false
//        unsigned int randomByte = ANG::tools::random::next() % len;
//        int offset = 5 - (ANG::tools::random::next() % 10);
//        std::cout << "Index: " << randomByte << ", Changing Single byte from " << str2[randomByte] << " to " << ( str2[randomByte] + offset) << std::endl;
//        str2[randomByte] += offset;

        /// print the starting address of both strings
        printValues("print the starting address", str1, str2);

        /// Start Checking the Results and Measuring Execution time for each solution
        for(const auto& function_to_test : solutions)
        {
            std::cout << "CALL COMPARER" << std::endl;
            auto cmpRes = function_to_test(str1.c_str(), str2.c_str(), len);
            std::cout << "RESULT: " << (cmpRes == 0 ? "success" : "FAILURE") << std::endl;
        }

        /// show the strings again to check if the characters are modified after any funciton calls
        printValues("show the strings again", str1, str2);

    }

}



