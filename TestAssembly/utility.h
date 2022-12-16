#ifndef UTILITY_H
#define UTILITY_H
#include <memory>
#include "common.h"

namespace Utility
{
    std::string getRandomString(int len);
    std::string getDeepcopyStringRandomizeCase(const std::string&, unsigned int len);
}

#endif // UTILITY_H
