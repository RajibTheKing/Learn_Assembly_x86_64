#ifndef ANALYZESOLUTION_H
#define ANALYZESOLUTION_H

#include "assembly_wrapper.h"
#include "utility.h"

class AnalyzeSolution
{
public:
    AssemblyWrapper *assemblyWrapper = nullptr;
    Utility *utility = nullptr;

    AnalyzeSolution();
    ~AnalyzeSolution();

    void startAnaylizing();

    void analyzeAdd();
    void analyzeSaturation();
    void analyzeSubstringSearch();
    void analyzeStringCompare();


};

#endif // ANALYZESOLUTION_H
