#ifndef ASSEMBLY_WRAPPER_H
#define ASSEMBLY_WRAPPER_H

#include "common.h"

extern "C"
{
    void sub_check_x86_64();
    unsigned int getsum_x86_64(unsigned int &a, unsigned int &b);
    unsigned int getsum_x86_64_GAS(unsigned int &a, unsigned int &b);
    void packed_suturation_test(unsigned char *inData, unsigned char *outData);
    unsigned int search_substr(unsigned char *inStr, unsigned char *subStr);
    unsigned int compare_string(unsigned char *str1, unsigned char *str2, unsigned int str1len, unsigned int str2len);

    int test_movdqa_x86_64(const char *lhs, const char *rhs, unsigned int len);

    int ___i_case_compare_ordering(const char *str1, const char *str2, size_t len);
    int ___i_case_compare(const char *lhs, const char *rhs, size_t len);
    int i_case_compare_v3(const char *lhs, const char *rhs, size_t len);
    int i_case_compare_v4(const char *lhs, const char *rhs, size_t len);
}

class AssemblyWrapper{
public:
    static AssemblyWrapper* getInstance(){
        static AssemblyWrapper *instance = new AssemblyWrapper();
        return instance;
    }
    unsigned int getSum(unsigned int &a, unsigned int &b);
    void packedSaturationTest(unsigned char *inData, unsigned char *outData);
    unsigned int searchSubstr(unsigned char *inStr, unsigned char* subStr);
    unsigned int compareString(unsigned char *str1, unsigned char *str2, unsigned int str1Len, unsigned int str2Len);
    int compareStringCaseinsensitive(const char *str1, const char *str2, unsigned int len);
    int testMovdqa(const char *lhs, const char *rhs, unsigned int len);
private:
    AssemblyWrapper(){}
};

#endif
