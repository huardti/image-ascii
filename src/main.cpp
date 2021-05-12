#include <iostream>
#include <string>

#include "image/bmp/bmp.hpp"
//#include "ascii.hpp"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " [file_name.bmp]" << std::endl;
        return 0;
    }
#ifdef _DEBUG
    std::cout << "alive" << std::endl;
#endif // _DEBUG

    const char* file = argv[1];
    // Img::image i ;
    //Image::bmp I(file);
    //I.convert_black_and_white(bmp::LUMINOSITE);
    //toAscii(I);
    return 0;

}