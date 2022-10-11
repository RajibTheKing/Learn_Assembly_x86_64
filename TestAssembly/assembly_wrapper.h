#ifndef ASSEMBLY_WRAPPER_H
#define ASSEMBLY_WRAPPER_H

extern "C"
{
    unsigned int getSum_x86_64(unsigned int &a, unsigned int &b);
}

class AssemblyWrapper{
public:
    unsigned int getSum(unsigned int &a, unsigned int &b);

};

#endif