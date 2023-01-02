#include "analyzesolution.h"
#include "LessCompare.h"

int compareCharByCharCaseInsensitive(const char *str1, const char* str2, size_t len)
{
    //not efficient implemnentation ... just a Naive solution
    int i=0, j=0;
    while((int)len > 0)
    {
        int diff = tolower(str1[i]) - tolower(str2[i]);
        if(diff != 0)
            return diff; //mismatch found
        i++;
        j++;
        len--;
    }

    return 0; // no mismatch
}

const char* strchrCaseInsensitive(const char *str, int target)
{
    int len = strlen(str);
    for(int i=0; i<len; i++)
    {
        if(tolower(str[i]) == tolower(target))
        {
            return str+i; //< Found, return address of first occurence
        }
    }
    return nullptr; //< Not Found, return nullptr
}

int main()
{
    AnalyzeSolution *analyzeSolution = new AnalyzeSolution();

     analyzeSolution->startAnaylizing();

    // std::string str1 = "";
    // std::string str2 = "";
    // int result = compareCharByCharCaseInsensitive(str1.c_str(), str2.c_str(), 6);
    // printf("%d\n", result);


//    /// Check the behaviors of string_view
//    std::string_view str {"once upon a time"};
//    std::string_view str2 = str.substr(3, 5);

//    for(unsigned int i=0; i<str.length();i++){
//        printf("%X ", str.data()+i);
//    }
//    printf("\n");
//    for(unsigned int i=0; i<str2.length();i++){
//        printf("%X ", str2.data()+i);
//    }
//    printf("\n");

//    unsigned int x = 2541;

//    printf("%X\n", x);




    return 0;
}





