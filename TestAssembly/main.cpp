#include "analyzesolution.h"


int compareCharByCharCaseInsensitive(const char *str1, const char* str2, size_t len)
{
    //not efficient implemnentation ... just a Naive solution
    int i=0, j=0;
    while(len)
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


int main()
{
    AnalyzeSolution *analyzeSolution = new AnalyzeSolution();

    analyzeSolution->startAnaylizing();


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





