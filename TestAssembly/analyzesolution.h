#ifndef ANALYZESOLUTION_H
#define ANALYZESOLUTION_H

#include "assembly_wrapper.h"
#include "utility.h"
extern int compareCharByCharCaseInsensitive(const char *str1, const char* str2, size_t len);

class AnalyzeSolution
{
public:
    AnalyzeSolution() = default;
    ~AnalyzeSolution() = default;

    void startAnaylizing();

    void analyzeStringCompareCaseinsensitive();


};

#endif // ANALYZESOLUTION_H
