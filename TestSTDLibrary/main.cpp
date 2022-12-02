#include "iCaseInsenitiveCompare.h"

#include <iostream>
#include <set>
#include <string>
#include <string_view>



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

    std::set<   std::string_view,      std::less<>,             std::allocator<std::string_view>    > st_standard;
    std::set<   std::string_view,      CaseInsenitiveCompare,   std::allocator<std::string_view>    > st_custom;

    st_standard.insert("Rob");
    st_standard.insert("Rajib");
    st_standard.insert("Tim");
    st_standard.insert("rob");

    st_custom.insert("Rob");
    st_custom.insert("Rajib");
    st_custom.insert("Tim");
    st_custom.insert("rob");

    std::cout<<"Show values inside: st_standard"<<std::endl;
    for(auto iterator = st_standard.begin(); iterator!= st_standard.end(); iterator++){
        std::cout<<*iterator<<std::endl;
    }

    std::cout<<"Show values inside: st_custom"<<std::endl;
    for(auto iterator = st_custom.begin(); iterator!= st_custom.end(); iterator++){
        std::cout<<*iterator<<std::endl;
    }


    return 0;
}