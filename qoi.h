// this file container different types of integer types so that we can define byte and long and word etc.
#include <stdint.h>

// define some data types to be more user friendly

typedef uint8_t BYTE;
typedef int32_t LONG;
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef int8_t U_BYTE;



// define the structure of the QOI file header
typedef struct
{
    DWORD magic;
    DWORD width;
    DWORD height;
    BYTE channels;
    BYTE color_space;

}__attribute__((packed)) 
QOI_HEADER;



