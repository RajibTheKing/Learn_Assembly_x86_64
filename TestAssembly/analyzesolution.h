#ifndef ANALYZESOLUTION_H
#define ANALYZESOLUTION_H

#include "assembly_wrapper.h"
#include "utility.h"
extern int compareCharByCharCaseInsensitive(const char *str1, const char* str2, size_t len);
extern const char* strchrCaseInsensitive(const char* str, int target);

class AnalyzeSolution
{
public:
    AssemblyWrapper *assemblyWrapper = AssemblyWrapper::getInstance();
    Utility *utility = nullptr;
    std::ofstream myfile;

    AnalyzeSolution();
    ~AnalyzeSolution();

    void startAnaylizing();

    void analyzeAdd();
    void analyzeSaturation();
    void analyzeSubstringSearch();
    void analyzeStringCompare();
    void analyzeStringCompareCaseinsensitive();
    void analyzeMovdqa();
    void analyze_strichr();
    std::string getNameBySolution(int n);
    std::string getNameBySolution_strichr(int n);
    void showProgressBar(int totalElements, int alreadyCompleted);


};

#endif // ANALYZESOLUTION_H
