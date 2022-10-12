#include "assembly_wrapper.h"
#include "common.h"

int main()
{
    AssemblyWrapper *assemblyWrapper = new AssemblyWrapper();

    
    unsigned int a = 20;
    unsigned int b = 15;

    unsigned int result = assemblyWrapper->getSum(a, b);


    std::cout << "Sum = " << result << std::endl;

    return 0;
}