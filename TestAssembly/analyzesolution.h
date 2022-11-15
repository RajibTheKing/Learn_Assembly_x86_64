#ifndef ANALYZESOLUTION_H
#define ANALYZESOLUTION_H

#include "assembly_wrapper.h"
#include "utility.h"
extern int compareCharByCharCaseInsensitive(const char *str1, const char* str2, size_t len);

class AnalyzeSolution
{
public:
    AssemblyWrapper *assemblyWrapper = AssemblyWrapper::getInstance();
    Utility *utility = nullptr;

    AnalyzeSolution();
    ~AnalyzeSolution();

    void startAnaylizing();

    void analyzeAdd();
    void analyzeSaturation();
    void analyzeSubstringSearch();
    void analyzeStringCompare();
    void analyzeStringCompareCaseinsensitive();
    void analyzeMovdqa();
    std::string getNameBySolution(int n);
    void showProgressBar(int totalElements, int alreadyCompleted);


};

#endif // ANALYZESOLUTION_H
