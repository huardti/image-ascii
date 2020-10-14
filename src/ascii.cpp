#include "ascii.hpp"

#include <algorithm>

char grayToChar(uint8_t gray){
    char CHAR_LIST[32] = " .-,:+~;(%x1*nT3J5$S4FGE8AD@B#0";
    int num_chars = 31;
    return CHAR_LIST[std::min(int(gray * num_chars / 255),num_chars - 1)];
}