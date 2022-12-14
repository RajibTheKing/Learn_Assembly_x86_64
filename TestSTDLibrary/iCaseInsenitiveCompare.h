
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

    template<typename T, typename U> bool operator()(T lhs, U rhs) const
    {
        return isLess(lhs)(rhs);
    }

    struct isLess
    {
        const char* lhs_str;
        std::size_t lhs_len;

        isLess(const std::string& lhs)      : lhs_str(lhs.data()), lhs_len(lhs.length()) {}
        isLess(const std::string_view& lhs) : lhs_str(lhs.data()), lhs_len(lhs.length()) {}

        bool operator()(const std::string& rhs)
        { 
            return compareCharByCharCaseInsensitive(lhs_str, rhs.data(), rhs.length()) < 0; 
        }

        bool operator()(const std::string_view& rhs)
        { 
            return compareCharByCharCaseInsensitive(lhs_str, rhs.data(), rhs.length()) < 0; 
        }

        /// N.B: this function can be replaced by assembly implementations
        int compareCharByCharCaseInsensitive(const char *lhs, const char* rhs, std::size_t len)
        {
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
