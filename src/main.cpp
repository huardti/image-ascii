#include <iostream>
#include <string>

#include "bmp.hpp"
#include "ascii.hpp"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "nombre d'argument incorect" << std::endl;
        return 0;
    }

    const char* file = argv[1];
    bmp::Image I(file);
    I.convert_black_and_white(bmp::LUMINOSITE);
    bmp::Pixel p;
    
    for(size_t y = I.get_height(); y >0; y--){
        for (size_t x = 0; x < I.get_width(); x++) {
            p = (I.get_pixel(x,y));
            char c = grayToChar(p.R);
            std::cout << c;
        }
        std::cout << std::endl;
    }
    /*
    p = (I.get_pixel(1000,1000));
    std::cout << " degre:" << (int)p.R << std::endl;
    char c = grayToChar(p.R);
    std::cout << c;
*/
    return 0;

}