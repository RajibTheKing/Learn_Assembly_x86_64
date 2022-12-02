
#ifndef CASE_INSENSITIVE_COMPARE_H
#define CASE_INSENSITIVE_COMPARE_H
#include <string>
#include <string_view>
#include <cstring>
#include <stdio.h>
class CaseInsenitiveCompare
{
public:
    using is_transparent = void;
    using A = const char*; 
    using B = const std::string&;
    using C = const std::string_view&;
  

    template<typename T, typename U> bool operator()(T lhs, U rhs) const { return isLess(lhs)(rhs); }

    struct isLess
    {
        const char* lhs_str;
        std::size_t lhs_len;

        isLess(const char* lhs)                                 : lhs_str(lhs), lhs_len(std::strlen(lhs)) {}
        isLess(const std::string& lhs)                          : lhs_str(lhs.data()), lhs_len(lhs.length()) {}
        isLess(const std::string_view& lhs)                     : lhs_str(lhs.data()), lhs_len(lhs.length()) {}

        bool operator()(const char* rhs)
        { 
            return compareCharByCharCaseInsensitive(lhs_str, rhs, std::strlen(rhs)) < 0; 
        }

        bool operator()(const std::string& rhs)
        { 
            return compareCharByCharCaseInsensitive(lhs_str, rhs.data(), rhs.length()) < 0; 
        }

        bool operator()(const std::string_view& rhs)
        { 
            return compareCharByCharCaseInsensitive(lhs_str, rhs.data(), rhs.length()) < 0; 
        }

        int compareCharByCharCaseInsensitive(const char *lhs, const char* rhs, std::size_t len)
        {
            //printf("inside compareCharByCharCaseInsensitive with, %s and %s\n", lhs, rhs);
            //not efficient implemnentation ... just a Naive solution
            int i=0, j=0;
            while((int)len > 0)
            {
                int diff = tolower(lhs[i]) - tolower(rhs[i]);
                if(diff != 0)
                    return diff; //mismatch found
                i++;
                j++;
                len--;
            }

            return 0; // no mismatch
        }
    };
};

#endif