#include <memory>
#include "utility.h"

std::string Utility::getRandomString(int len)
{
    std::string buffer;

    int offset;
    for(int i=0; i<len; i++)
    {
        int rand_case = (rand() % 2);
        rand_case ? offset = 'a' : offset = 'A';
        int rand_now = (rand() % 26) + offset;
        buffer += rand_now;
    }
    return buffer;
}

std::string  Utility::getDeepcopyStringRandomizeCase(const std::string& str, unsigned int len)
{
    std::string deepCopyStr = str;
    (void) len;

    for(unsigned int i=0; i<len;i++)
    {
        int rand_case = rand() %2;
        rand_case ? deepCopyStr[i] = toupper(deepCopyStr[i]) : tolower(deepCopyStr[i]);
    }
    return deepCopyStr;
}
