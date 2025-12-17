#include "dec64.hpp"
#include <iostream>
#include <string>

int main() {
    // Example 1: Constructing dec64 objects
    dec64 a(123); // Integer constructor
    dec64 b(45.67); // Double constructor
    dec64 c("789.01"); // String constructor
    std::string_view d_ = "345.67";
    dec64 d(d_); // String view constructor

    std::cout << "a: " << a << std::endl;
    std::cout << "b: " << b << std::endl;
    std::cout << "c: " << c << std::endl;
    std::cout << "d: " << d << std::endl;

    // Example 2: Arithmetic operations
    dec64 sum = a + b;
    dec64 diff = a - c;
    dec64 prod = b * c;
    dec64 quot = c / b;

    std::cout << "a + b: " << sum << std::endl;
    std::cout << "a - c: " << diff << std::endl;
    std::cout << "b * c: " << prod << std::endl;
    std::cout << "c / b: " << quot << std::endl;

    // Example 3: Comparison operations
    std::cout << "a == b: " << (a == b) << std::endl;
    std::cout << "a < c: " << (a < c) << std::endl;
    std::cout << "b > c: " << (b > c) << std::endl;

    // Example 4: Rescaling
    dec64 rescaled = c.rescale(3);
    std::cout << "c rescaled to 3 decimal places: " << rescaled << std::endl;

    rescaled = c.rescale(1);
    std::cout << "c rescaled to 1 decimal places: " << rescaled << std::endl;

    // Example 5: Hashing
    std::cout << "Hash of a: " << std::hash<dec64>{}(a) << std::endl;

    return 0;
}