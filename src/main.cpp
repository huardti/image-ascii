#include <iostream>
#include <string>

#include "bmp.hpp"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "nombre d'argument incorect" << std::endl;
        return 0;
    }

    const char* file = argv[1];
    bmp::Image I(file);
    I.convert_black_and_white(bmp::LUMINOSITE);

    I.write("luminosite.bmp");
    
    return 0;

}