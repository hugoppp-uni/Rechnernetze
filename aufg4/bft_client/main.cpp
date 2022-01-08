#include <iostream>
#include "options.hpp"

int main(int argc, char** args){
    Options options{argc, args};
    std::cout << "Hello Client" << std::endl;
    return 0;
}
