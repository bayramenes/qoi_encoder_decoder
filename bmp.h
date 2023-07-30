// this is a file that contains the main structure for the bmp file so that they can be used both by the encoder and decoder
#include <stdint.h>






// define some data types to be more user friendly

typedef uint8_t BYTE;
typedef int32_t LONG;
typedef uint32_t DWORD;
typedef uint16_t WORD;


// define the structure of the bmp file HEADER
// note that we had to use the __attribute__((__packed__)) to make sure that the compiler will not add any padding to the structure
// hence misalligning the data that we want and misinterpreting the data
typedef struct  
{
    WORD magic_number;
    DWORD file_size;
    WORD reserved1;
    WORD reserved2;
    DWORD offset;

    
}__attribute__((__packed__))
BITMAPFILEHEADER;

// define the structure of the bmp file INFO

typedef struct {
    DWORD header_size;
    DWORD width;
    DWORD height;
    WORD color_planes;
    WORD bit_depth;
    DWORD compression;
    DWORD image_size;
    DWORD horizontal_resolution;
    DWORD vertical_resolution;
    DWORD colors;
    DWORD important_colors;
}__attribute__((__packed__))
BITMAPINFOHEADER;





