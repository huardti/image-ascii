#include "bmp.hpp"
 
namespace bmp
{

    /*============================*/
    Image::Image(int32_t width, int32_t height, bool has_alpha){
        if (width <= 0 || height <= 0) {
            throw std::runtime_error("The image width and height must be positive numbers.");
        }
        std::cout << width << " " << height << std::endl;
        
        bmp_info_header.width = width;
        bmp_info_header.height = height;
        if (has_alpha) {
            bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);

            bmp_info_header.bit_count = 32;
            bmp_info_header.compression = 3;
            row_stride = width * 4;
            data.resize(row_stride * height);
            file_header.file_size = file_header.offset_data + data.size();
        }
        else {
            bmp_info_header.size = sizeof(BMPInfoHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

            bmp_info_header.bit_count = 24;
            bmp_info_header.compression = 0;
            row_stride = width * 3;
            data.resize(row_stride * height);

            uint32_t new_stride = make_stride_aligned(4);
            file_header.file_size = file_header.offset_data + static_cast<uint32_t>(data.size()) + bmp_info_header.height * (new_stride - row_stride);
        }
    }

    Image::Image(const char* fname){
        read(fname);
    }

    Image::~Image() {
    }

    void Image::read(const char* fname){
        std::ifstream inp{ fname, std::ios_base::binary };
        if (!inp) {
            throw std::runtime_error("Unable to open the input image file.");
        }
        inp.read((char*)&file_header, sizeof(file_header));
        if(file_header.file_type != 0x4D42) {
            throw std::runtime_error("Error! Unrecognized file format.");
        }
        inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

        // The BMPColorHeader is used only for transparent images
        if(bmp_info_header.bit_count == 32) {
            // Check if the file has bit mask color information
            if(bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
                inp.read((char*)&bmp_color_header, sizeof(bmp_color_header));
                // Check if the pixel data is stored as BGRA and if the color space type is sRGB
                check_color_header(bmp_color_header);
            } else {
                std::cerr << "Error! The file \"" << fname << "\" does not seem to contain bit mask information\n";
                throw std::runtime_error("Error! Unrecognized file format.");
            }
        }

        // Jump to the pixel data location
        inp.seekg(file_header.offset_data, inp.beg);

        // Adjust the header fields for output.
        // Some editors will put extra info in the image file, we only save the headers and the data.
        if(bmp_info_header.bit_count == 32) {
            bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        } else {
            bmp_info_header.size = sizeof(BMPInfoHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        }
        file_header.file_size = file_header.offset_data;

        if (bmp_info_header.height < 0) {
            throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
        }

        data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

        // Here we check if we need to take into account row padding
        if (bmp_info_header.width % 4 == 0) {
            inp.read((char*)data.data(), data.size());
            file_header.file_size += static_cast<uint32_t>(data.size());
        }
        else {
            row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
            uint32_t new_stride = make_stride_aligned(4);
            std::vector<uint8_t> padding_row(new_stride - row_stride);

            for (int y = 0; y < bmp_info_header.height; ++y) {
                inp.read((char*)(data.data() + row_stride * y), row_stride);
                inp.read((char*)padding_row.data(), padding_row.size());
            }
            file_header.file_size += static_cast<uint32_t>(data.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());
        }
    }

    void Image::write(const char *fname) {
        std::ofstream of{ fname, std::ios_base::binary };
        if (of) {
            if (bmp_info_header.bit_count == 32) {
                write_headers_and_data(of);
            }
            else if (bmp_info_header.bit_count == 24) {
                if (bmp_info_header.width % 4 == 0) {
                    write_headers_and_data(of);
                }
                else {
                    uint32_t new_stride = make_stride_aligned(4);
                    std::vector<uint8_t> padding_row(new_stride - row_stride);

                    write_headers(of);

                    for (int32_t y = 0; y < bmp_info_header.height; ++y) {
                        of.write((const char*)(data.data() + row_stride * y), row_stride);
                        of.write((const char*)padding_row.data(), padding_row.size());
                    }
                }
            }
            else {
                throw std::runtime_error("The program can treat only 24 or 32 bits per pixel BMP files");
            }
        }
        else {
            throw std::runtime_error("Unable to open the output image file.");
        }
    }

    void Image::set_pixel(uint32_t x0, uint32_t y0, uint8_t B, uint8_t G, uint8_t R, uint8_t A) {
        if (x0 > (uint32_t)bmp_info_header.width || y0 > (uint32_t)bmp_info_header.height) {
            std::cout << x0 << "," << y0 << std::endl;
            throw std::runtime_error("The point is outside the image boundaries!");
        }

        uint32_t channels = bmp_info_header.bit_count / 8;
        data[channels * (y0 * bmp_info_header.width + x0) + 0] = B;
        data[channels * (y0 * bmp_info_header.width + x0) + 1] = G;
        data[channels * (y0 * bmp_info_header.width + x0) + 2] = R;
        if (channels == 4) {
            data[channels * (y0 * bmp_info_header.width + x0) + 3] = A;
        }
    }

    void Image::set_pixel(uint32_t x0, uint32_t y0, Pixel p){
        set_pixel(x0, y0, p.B, p.G, p.R, p.A);
    }

    Pixel Image::get_pixel(uint32_t x0, uint32_t y0){
        if (x0 > (uint32_t)bmp_info_header.width || y0 > (uint32_t)bmp_info_header.height) {
            std::cout << x0 << "," << y0 << std::endl;
            throw std::runtime_error("The point is outside the image boundaries!");
        }

        Pixel p;
        uint32_t channels = bmp_info_header.bit_count / 8;
        p.B = data[channels * (y0 * bmp_info_header.width + x0) + 0];
        p.G = data[channels * (y0 * bmp_info_header.width + x0) + 1];
        p.R = data[channels * (y0 * bmp_info_header.width + x0) + 2];
        if (channels == 4) {
            p.A = data[channels * (y0 * bmp_info_header.width + x0) + 3];
        }
        return p;
    }

    void Image::write_headers(std::ofstream &of) {
        of.write((const char*)&file_header, sizeof(file_header));
        of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));
        if(bmp_info_header.bit_count == 32) {
            of.write((const char*)&bmp_color_header, sizeof(bmp_color_header));
        }
    }

    void Image::convert_black_and_white(ModeNoirBlanc M){
        uint32_t channels = bmp_info_header.bit_count / 8;

        double moyenne;
        uint8_t R,G,B,min,max;
        for (int y = 0; y < bmp_info_header.height; y++) {
            for (int x = 0; x < bmp_info_header.width; x++) {
                moyenne = 0;
                if (M == LUMINOSITE) {
                    moyenne += 0.07*data[channels * (y * bmp_info_header.width + x) + 0];
                    moyenne += 0.71*data[channels * (y * bmp_info_header.width + x) + 1];
                    moyenne += 0.23*data[channels * (y * bmp_info_header.width + x) + 2];
                }
                else if (M == CLARTE) {
                    B = data[channels * (y * bmp_info_header.width + x) + 0];
                    G = data[channels * (y * bmp_info_header.width + x) + 1];
                    R = data[channels * (y * bmp_info_header.width + x) + 2];

                    min = std::min(B,G);
                    moyenne += std::min(min,R);
                    max = std::max(B,G);
                    moyenne += std::max(max, R);
                    moyenne = moyenne/2;          
                }
                else if(M == MOYENNE){
                    moyenne += data[channels * (y * bmp_info_header.width + x) + 0];
                    moyenne += data[channels * (y * bmp_info_header.width + x) + 1];
                    moyenne += data[channels * (y * bmp_info_header.width + x) + 2];
                    moyenne = moyenne/3;       
                }
                    set_pixel(x,y,Pixel{(uint8_t)moyenne,(uint8_t)moyenne,(uint8_t)moyenne,(uint8_t)255});
            }   
        }

    }

//================Private===============//

    void Image::write_headers_and_data(std::ofstream &of) {
        write_headers(of);
        of.write((const char*)data.data(), data.size());
    }

    // Add 1 to the row_stride until it is divisible with align_stride
    uint32_t Image::make_stride_aligned(uint32_t align_stride) {
        uint32_t new_stride = row_stride;
        while (new_stride % align_stride != 0) {
            new_stride++;
        }
        return new_stride;
    }

    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
    void Image::check_color_header(BMPColorHeader &bmp_color_header) {
        BMPColorHeader expected_color_header;
        if(expected_color_header.red_mask != bmp_color_header.red_mask ||
            expected_color_header.blue_mask != bmp_color_header.blue_mask ||
            expected_color_header.green_mask != bmp_color_header.green_mask ||
            expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {
            throw std::runtime_error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
        }
        if(expected_color_header.color_space_type != bmp_color_header.color_space_type) {
            throw std::runtime_error("Unexpected color space type! The program expects sRGB values");
        }
    }

}
