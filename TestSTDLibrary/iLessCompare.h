#ifndef ANG_STRING_LESSCOMPARE_H
#define ANG_STRING_LESSCOMPARE_H
#include <cstring>
#include <string_view>
#include <string>

namespace ANG::string {

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



/// <typename Compare> for use with associative containers
/// enables them to use std::string as key and still be able to search via std::string_view and vice versa
/// \attention std::less<void> will use some implicit conversion between std::string and std::string_view thus be less efficient
/// even though std::less<void> uses perfect forwarding but std lacks operator<(...) between std::string and std::string_view
/// (therefore I guess the std::string would create an intermediate for type std::string_view)
/// \attention this further doesn't apply an alphabetical ordering in order to gain additional speed ups
template<int (*CompareFunction)(const char*, const char*, std::size_t)> class LessCompareBase
{
public:
    using is_transparent = void; ///< as required by \see https://en.cppreference.com/w/cpp/container/xyz/find or anything that looks up values

    /// Cartesian product as required by \see https://en.cppreference.com/w/cpp/named_req/Compare
    template<typename T, typename U> bool operator()(T lhs, U rhs) const { return isLess(lhs)(rhs); }

    /// does the actual comparison but avoids Cartesian product for actual implementation
    struct isLess
    {
        const char* lhs_str;
        std::size_t lhs_len;

//        isLess(const char* lhs)                                 : lhs_str(lhs), lhs_len(std::strlen(lhs)) {} //< because each time a comparison is made \see std::strlen() will be called, which most likely eats up all the benefit, therefore its much better to transform it into \see std::string_view and then possibly still do an asymmetric key type lookup
        isLess(const std::string& lhs)                          : lhs_str(lhs.data()), lhs_len(lhs.length()) {}
        isLess(const std::string_view& lhs)                     : lhs_str(lhs.data()), lhs_len(lhs.length()) {}

//        bool operator()(const char* rhs) const                  { return execute(rhs, std::strlen(rhs)); }
        bool operator()(const std::string& rhs) const           { return execute(rhs.data(), rhs.length()); }
        bool operator()(const std::string_view& rhs) const      { return execute(rhs.data(), rhs.length()); }

        bool execute(const char* rhs_str, std::size_t rhs_len) const
        {
            auto val = lhs_len - rhs_len;
            if (val)
                return lhs_len < val;
            else
                return CompareFunction(lhs_str, rhs_str, rhs_len) < 0;
        }
    };
};

using LessCompare = LessCompareBase<&std::strncmp>;
using LessCompareLengthCaseInsensitive = LessCompareBase<&compareCharByCharCaseInsensitive>;

}

#endif
