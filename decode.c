#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "qoi.h"

// ---------------------------
// ---------------------------
// ------------NOTE-----------
// i have assumed that the system running this code is a little endian system
// since this is the most common in INTEL and AMD also APPLE SILICON is this 
// way and also because i am too lazy to implement that as well so yeah
// ---------------------------
// ---------------------------
// ---------------------------



// functions to reverse endiannes of the data

// this code basically ands each byte of the data so that we can get the individual bytes then applies a shift to them so that that the order is reversed and
// then it OR's the reversed bytes to get the reversed data
// note that i will only be using this function for the header so that the data is little endian but for the header it has to be normal so i will
// be reversing the header in order to maintaint the correct order but since most processeers and computers use little endian so i will be using the same
// and will not bother with big endian for the data


u_int32_t Reverse32(u_int32_t value)
{
	u_int32_t result;
	u_int32_t leftmost_byte = (value & 0x000000FF) << 24;
	u_int32_t left_middle_byle = (value & 0x0000FF00) << 8;
	u_int32_t right_middle_byte = (value & 0x00FF0000) >> 8;
	u_int32_t rightmost_byte = (value & 0xFF000000) >> 24;
	result = (leftmost_byte | left_middle_byle
			| right_middle_byte | rightmost_byte);

	return result;
}


u_int16_t Reverse16(u_int16_t value)
{
	u_int16_t result;
	u_int16_t leftmost_byte = (value & 0x00FF) << 8;
	u_int16_t rightmost_byte = (value & 0xFF00) >> 8;
	result = (leftmost_byte | rightmost_byte);
	return result;
}

// define the structure of the pixel in an image so that it can be used in the encoder and decoder
typedef struct
{
    BYTE red;
    BYTE green;
    BYTE blue;
} PIXEL;


const BYTE RGB_byte_flag=254; // 11111110
const BYTE run_bit_flag=192; // 11000000
const BYTE index_bit_flag=0; // 00000000 
const BYTE small_difference_bit_flag=64; // 01000000
const BYTE bigger_difference_bit_flag=128; // 10000000 

// a function that return the byte of which is supposted to be the bit flag that were defined above
// this function is useful when reading bytes so that what type of byte is this
// e.g. is it a run flag or an index or a difference etc...
BYTE get_flag(BYTE byte){
    return byte & 192;   
    // the number 192 seems arbitrary but it is 11000000 in binary 
    // the idea being that when anding 11000000 with the byte we get the flag becuase only the last two bits will be 
    // turned on according to the flag other ones are for sure turned off
}



// a function that return the data from the byte 
BYTE get_byte_data(BYTE byte){
    return byte & 63 ; // because the number 63 is 00111111 in binary
}

// get the file header of the input file
QOI_HEADER get_file_header(FILE *inputFile){

    QOI_HEADER file_header;
    // read the file header from the memory as one chunk
    fread(&file_header, sizeof(QOI_HEADER), 1, inputFile);
    return file_header;
}


int validate_file(QOI_HEADER file_header){
    if (
        file_header.magic != 0x66696f71
        || file_header.width < 0
        || file_header.height < 0
        || file_header.channels != 3 // i might support alpha channel later but too lazy now
        )
        {
            printf("file type not supported!\n");

            return 22;
        }
        return 1; // 1 indicates that the file is valid
}

// get the hash of a given pixel
// useful for index and creating a hashmap of the pixels
BYTE hash_pixel(PIXEL pixel)
{
    return (pixel.red * 3 + pixel.green * 5 + pixel.blue * 7) % 64;
}



int decode(char *inputFileName, char *outputFileName)
{
    

    // open the input file
    FILE * inputFile = fopen(inputFileName, "r");
    // get the file header for validation purposes and also to be able to get the width and height
    // these are important for the decoder to be able to decode the pixels according to the width and height
    QOI_HEADER file_header =  get_file_header(inputFile);
    
    // validate the file
    int valid = validate_file(file_header);
    // if the file is not valid then return the response from valid
    if (valid != 1){
        return valid;
    }

    printf("file is valid and ready to be decoded!!\n");


    // first we get the width and height so that we can allocate memory for the pixels
    // and decode the pixels according to that

    int width = file_header.width;
    int height = file_header.height;

    printf("image width : %d\n", width);
    printf("image height  : %d\n", height);
    
    // ------------------------------------------------
    // initialize an index list to save pixels at
    // ------------------------------------------------

    // note that i have added static so that the values are initialized to 0
    static PIXEL index_list[64];
    // initialize the previous pixel to 0
    PIXEL previous_pixel;
    previous_pixel.red = 0;
    previous_pixel.green = 0;
    previous_pixel.blue = 0;
    PIXEL current_pixel;

    // open the output file 
    FILE * outputFile = fopen(outputFileName, "w");
    // the header to the output file
    BITMAPFILEHEADER output_file_header;
    output_file_header.magic_number=0x4d42; // this is in reverse assuming little endian
    // note that for file size we hav the offset of 54 bytes 
    // which are reserved for the headers
    // and then after that we have the raw data assuming no compression
    // and since we have only channels of color (might implement alpha later)
    // this means that each pixel is 3 bytes so we have to multiply the width
    // by the height to get the pixel count
    // and then we have to multiply the pixel count by 3 to get the byte count
    // and add to 54 to get the final file size
    output_file_header.file_size=54+(width*height*3);
    output_file_header.reserved1=0;
    output_file_header.reserved1=0;
    output_file_header.offset=54;



    // write file header to the output file
    fwrite(&output_file_header, sizeof(BITMAPFILEHEADER), 1, outputFile);

    BITMAPINFOHEADER output_info_header;
    // this is the most common standart for BMP info headers which is
    // i think used by windows and mac and so i am using it here
    output_info_header.header_size=40;
    output_info_header.width=width;
    output_info_header.height=height;
    output_info_header.color_planes=1;
    output_info_header.bit_depth=24;
    output_info_header.compression=0;
    output_info_header.image_size=width*height*output_info_header.bit_depth;
    // i will be setting the horizontal and vertical resolution to zero
    // this is because it has to do with screen size and so on 
    // and it doesn't impact the images and the image still opens so yeah
    output_info_header.horizontal_resolution=0;
    output_info_header.vertical_resolution=0;
    output_info_header.colors=0;
    output_info_header.important_colors=0;

    // write info file header to the output file
    fwrite(&output_info_header, sizeof(BITMAPINFOHEADER), 1, outputFile);

int x =0 ; 
    // now we decode the pixels
    BYTE current_byte;


int run_times=0;
int first_timer=0;
int small_difference_times=0;
int big_difference_times=0;
int index_times=0;



    while ( !feof(inputFile) ){


        fread(&current_byte, sizeof(BYTE), 1, inputFile);
        
        // check if the flag is an RGB flag 
        // note that i have to check if the flag is an RGB flag because
        // the RGB flag is an 8-bit flag not a 2 bit one
        if ( current_byte == RGB_byte_flag){

            // get the RGB pixel
            PIXEL pixel;
            // << order matters >>
            BYTE red_channel_byte;
            BYTE green_channel_byte;
            BYTE blue_channel_byte;


            fread(&red_channel_byte, sizeof(BYTE), 1, inputFile);
            fread(&green_channel_byte, sizeof(BYTE), 1, inputFile);
            fread(&blue_channel_byte, sizeof(BYTE), 1, inputFile);


            pixel.red = red_channel_byte;
            pixel.green = green_channel_byte;
            pixel.blue = blue_channel_byte;

            // write the pixel to the output file
            fwrite(&pixel, sizeof(PIXEL), 1, outputFile);
            // set this is the previous pixel
            previous_pixel = pixel;
            // update the index list with the new pixel
            BYTE pixel_hash = hash_pixel(pixel);
            // place this into the pixel index list
            index_list[pixel_hash]=pixel;
            printf("full\n");
        }
        else
        {
            
            // get the flag of the current byte
            BYTE flag = get_flag(current_byte);
            // get the data of the current byte
            BYTE data = get_byte_data(current_byte);


            if ( flag == run_bit_flag){


                // keep writing the previous pixel to the output file
                // the number of times the run length is
                for (int i = 0 ; i < ( data + 1 ) ; i++){
                    fwrite(&previous_pixel, sizeof(PIXEL), 1, outputFile);
                    printf("run\n");
                }
                // previous pixel stays the same

            }
            else if ( flag == index_bit_flag){

                // get the pixel from the index list
                PIXEL pixel = index_list[data];
                // write the pixel to the output file
                fwrite(&pixel, sizeof(PIXEL), 1, outputFile);
                // update the previous pixel
                previous_pixel=pixel;
                printf("index\n");
            }
            else if ( flag == small_difference_bit_flag){

                // get the difference
                // bits will then be shifted so that we can compare the numbers in the correct way
                // note that also we are storing differences with a bias of 2 so -2 => 0 etc.
                // that is why are going to subract 2 from the difference to get the real value
                // i think QOI implements it this way so that we don't have to deal with signed numbers
                // which are a little bit more confusing to work with given this context
                U_BYTE red_difference = ( ( data & 48 ) >> 4 ) - 2; // 48 is 00110000 which lets us get the red difference
                U_BYTE green_difference = ( ( data & 12 ) >> 2 ) -2; // 12 is 00001100 which lets us get the green difference
                U_BYTE blue_difference = ( ( data & 3 ) ) -2; // 3 is 00000011 which lets us get the blue difference
                // create the pixel
                PIXEL pixel;
                pixel.red = previous_pixel.red + red_difference;
                pixel.green = previous_pixel.green + green_difference;
                pixel.blue = previous_pixel.blue + blue_difference;
                // write the pixel to the output file
                fwrite(&pixel, sizeof(PIXEL), 1, outputFile);
                // set this is the previous pixel
                previous_pixel = pixel;
                printf("small\n");
            }
            
            else if ( flag == bigger_difference_bit_flag){
                // note that we are have a bias of 32 for the green channel 
                // and 8 for the red and blue channels

                // since this type of encoded data is stored with two bytes rather than one 
                // first we have to get the green channel difference and then read the second
                // byte where we will find the values for red and blue
                U_BYTE green_difference = data - 32 ; // subtracted because we have a bias of 32 e.g. -32 => 0
                // read the second byte
                BYTE second_byte;

                fread(&second_byte, sizeof(BYTE), 1, inputFile);
                // get the red and blue differences
                U_BYTE red_to_green_difference = ( ( second_byte & 240 ) >> 4 ) - 8;
                U_BYTE blue_to_green_difference = ( ( second_byte & 15 ) ) - 8;
                // note that these values are of the difference between (difference of red) - (difference of green)
                // and the same applies for the blue one
            
                // create the pixel
                // first we get the current green value because we need it to cal
                // note that the formula for the red and blue seems arbitrary but 
                // you can work it out and it turns out to be this one so yeah...
                PIXEL pixel;
                pixel.red = red_to_green_difference + previous_pixel.red + green_difference ;
                pixel.green = previous_pixel.green + green_difference ;
                pixel.blue = blue_to_green_difference + previous_pixel.blue + green_difference;
                // write the pixel to the output file
                fwrite(&pixel, sizeof(PIXEL), 1, outputFile);
                // update previous pixel
                previous_pixel = pixel;
                printf("big\n");
            }
            else{
                printf("Error while decoding byte flag isn't recognized...");
                return 84;
            }
        }
    }
    fclose(inputFile);
    fclose(outputFile);
    return 0;

}



int main(int argc, char *argv[])
{
    // check whether the user provided all the arguments needed and if not so 
    // prompt the user to provide them
    if (argc != 3)
    {
        printf("Usage: ./encode <input file> <output file>\n");
        return 22;
    }
    else
    {
        char *inputFileName;
        strcpy(inputFileName, argv[1]);
        char *outputFileName;
        strcpy(outputFileName, argv[2]);
        return decode(inputFileName, outputFileName);
    }



}


