#include "ascii.hpp"

#include <algorithm>

char grayToChar(uint8_t gray){
    char CHAR_LIST[32] = " .-,:+~;(%x1*nT3J5$S4FGE8AD@B#0";
    int num_chars = 31;
    return CHAR_LIST[std::min(int(gray * num_chars / 255),num_chars - 1)];
}

void toAscii(bmp::Image I){
    bmp::Pixel p;
    for(size_t y = I.get_height(); y >0; y--){
        for (size_t x = 0; x < I.get_width(); x++) {
            p = (I.get_pixel(x,y));
            char c = grayToChar(p.R);
            std::cout << c;
        }
        std::cout << std::endl;
    }
}