#include <iostream>
#include "options.hpp"

int main(int argc, char **argv) {

    Options opt{argc, argv};
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
