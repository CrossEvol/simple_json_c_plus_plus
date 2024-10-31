#include <any>
#include<iostream>
#include <vector>


int main() {
    std::vector<std::any> vec;

    // Add any type
    vec.push_back(42);
    vec.push_back(3.14);
    vec.push_back(std::string("hello"));
    vec.push_back(std::vector<int>{1, 2, 3});

    // Access with type checking
    try {
        int i = std::any_cast<int>(vec[0]);
        double d = std::any_cast<double>(vec[1]);
        std::cout << i << std::endl;
        std::cout << d << std::endl;
    } catch(const std::bad_any_cast& e) {
        std::cout << "Wrong type!" << std::endl;
    }
    std::cout << std::any_cast<std::string>(vec[2]) << std::endl;
    std::cout << &vec[3] << std::endl;
    return 0;
}
