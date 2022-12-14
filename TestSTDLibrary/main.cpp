#include "iCaseInsenitiveCompare.h"
#include "iLessCompare.h"

#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <cstring>
using namespace std::literals;




template<typename A, typename B>
void showSetValues(std::string name, std::set<A, B> current_set){
    std::cout<<"Show values inside: "<<name<<std::endl;
    std::cout<<"{";
    for(auto iterator = current_set.begin(); iterator!= current_set.end(); iterator++){
        if(iterator == current_set.begin()){
           std::cout<<"\""<<*iterator<<"\"";
        }else{
           std::cout<<", \""<<*iterator<<"\"";
        }

    }
    std::cout<<"}\n\n";
}

int main()
{
    /*
        Task from Rob: 
        essentially we should build a thing to be thrown in std::set<key, !!here!!, default> (see https://en.cppreference.com/w/cpp/container/set 
            and similar for all the other container) which should
        a) be transparent
        b) allow the container (if it stores text data) to order that like in a dictionary by ignoring casing
    */

    /*
        template<
            class Key,
            class Compare = std::less<Key>,
            class Allocator = std::allocator<Key>
        > class set;
    */

    using custom_set = std::set<std::string_view, ANG::string::LessCompare>;
    custom_set st_length{"BeetLe", "Cat", "aPe", "aaa", "bb", "ba", "Ab", "aA", "A", "B"};
    std::set<   std::string_view,      ANG::string::LessCompareLengthCaseInsensitive  > st_length_insensitive{"BeetLe", "Cat", "aPe", "aaa", "bb", "ba", "Ab", "aA", "A", "B"};
//    std::set<   std::string_view,      ANG::string::LessCompareAlphabetCaseInsensitive> st_alpha{"BeetLe", "Cat", "aPe", "aaa", "bb", "ba", "Ab", "aA", "A", "B"};



    showSetValues("st_length", st_length);
    showSetValues("st_length_insensitive", st_length_insensitive);
//    showSetValues("st_alpha", st_alpha);

    std::string str("some texts");
    std::string_view hello_sv(str.c_str());

    std::cout<<"hello_sv : "<<hello_sv<<std::endl;

    std::cout<<std::string_view("hello_sv")<<std::endl;




    return 0;
}
