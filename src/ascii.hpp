#ifndef _ASCII_H_
#define _ASCII_H_

#include <cstdint>
#include "image/bmp/bmp.hpp"

char grayToChar(uint8_t p);
void toAscii(bmp::Image i);

#endif // !_BMP_