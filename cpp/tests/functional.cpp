#include "wrapper.hpp"
#include "iostream"


class multiplier {
    public:
        int operator()(const int a, const int b) const {
            return a * b;
        }
};

/*
    Maybe need to implement a const_cast?
    Many of the variables are not calling the lvalue ctor
    unless they are explicitly defined as const variables.
*/

int main() {
    std::cout << "Now creating adder" << std::endl;
    dev_std::function<int(const int, const int)> f1(
        [](int a, int b) -> int{
            return a + b;
        });

    std::cout << "Now creating multiplier" << std::endl;
    const multiplier* m = new multiplier();
    dev_std::function<int(const int, const int)> f2(*m);


    std::cout << "Now creating sub" << std::endl;
    const auto sub = [](const int a, const int b) -> int{
            return a - b;
        };
    dev_std::function<int(const int, const int)> f3(sub);

    std::cout << "Now creating empty" << std::endl;
    dev_std::function<int(const int, const int)> f4;

    std::cout << "Checking truth values: " << std::endl;
    f1 ? std::cout << "f1 :true\n" : std::cout<< "f1 :false\n";
    f2 ? std::cout << "f2 :true\n" : std::cout<< "f2 :false\n";
    f3 ? std::cout << "f3 :true\n" : std::cout<< "f3 :false\n";
    f4 ? std::cout << "f4 :true\n" : std::cout<< "f4 :false\n";

    std::cout << "Checking truth results for 3 and 5 " << std::endl;
    std::cout << "f1: " << f1(3, 5) << std::endl;
    std::cout << "f2: " << f2(3, 5) << std::endl;
    std::cout << "f3: " << f3(3, 5) << std::endl;
    std::cout << "f4: " << f4(3, 5) << std::endl;
    
    
    
    
    



    return 0;
} // end
