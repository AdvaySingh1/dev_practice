#include <pair.hpp>
#include <string>
#include <iostream>
int main(){
    int a, b;
    a = b = 10;
    dev_std::pair p1(2, 4);
    dev_std::pair p2 = dev_std::make_pair<int, std::string>(3, "Hello");
    dev_std::pair p3(a, b);
    dev_std::pair<int, int> p4, p5;
    p4 = p1;
    p5 = dev_std::pair(1, 1);

    

    std::cout << "Pair 1| First: " 
        << p1.first() << " | Second: "
        << p1.second() << std::endl;

    std::cout << "Pair 2| First: " 
        << p2.first() << " | Second: "
        << p2.second() << std::endl;
    
    std::cout << "Pair 3| First: " 
        << p3.first() << " | Second: "
        << p3.second() << std::endl;

    std::cout << "Pair 4| First: " 
        << p4.first() << " | Second: "
        << p4.second() << std::endl;

    std::cout << "Pair 5| First: " 
        << p5.first() << " | Second: "
        << p5.second() << std::endl;


    std::cout << " == operatior | p1 & p3: " << (p1 == p3) << std::endl;
    std::cout << " == operatior | p1 & p4: " << (p1 == p4) << std::endl;
    
    std::cout << " > operatior | p1 & p3: " << (p1 > p3) << std::endl;
    std::cout << " > operatior | p1 & p4: " << (p1 > p4) << std::endl;

    std::cout << " <= operatior | p1 & p3: " << (p1 <= p3) << std::endl;
    std::cout << " <= operatior | p1 & p4: " << (p1 <= p4) << std::endl;

    dev_std::swap(p1, p5);

    std::cout << "Pair 1| First: " 
        << p1.first() << " | Second: "
        << p1.second() << std::endl;

    std::cout << "Pair 5| First: " 
        << p5.first() << " | Second: "
        << p5.second() << std::endl;
    
}