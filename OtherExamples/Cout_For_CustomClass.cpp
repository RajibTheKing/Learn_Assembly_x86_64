#include <iostream>
#include <string>

class Entity{
private:
    std::string name;
    int age;

public:
    Entity(std::string n, int a) //constructor
    {
        std::cout<<"Constructor is Executed!!"<<std::endl;
        name = n;
        age = a;
    }

    ~Entity() //Destructor
    {
        std::cout<<"Now...Destructor is executed"<<std::endl;
    }

    std::string getName(){
        return name;
    }

    int getAge(){
        return age;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Entity& entity);
};

/// This code portion is needed, to write cout(Standard Console Output) for Custom Class
std::ostream& operator<<(std::ostream& stream, const Entity& entity){
    stream<<entity.name;
    stream<<", ";
    stream<<std::to_string(entity.age);
    return stream;
}

int main()
{
    Entity entity("Rajib", 32);
    std::cout<<entity<<std::endl;

    return 0;
}

/// Compile by following command (Found from : https://coliru.stacked-crooked.com/) that enables some basic warnings
/// g++ -std=c++20 -O2 -Wall -pedantic -pthread -march=x86-64 Cout_For_CustomClass.cpp && ./a.out