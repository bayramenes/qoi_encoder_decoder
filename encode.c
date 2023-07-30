#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "qoi.h"

// define the structure of the pixel in an image so that it can be used in the encoder and decoder
typedef struct
{
    BYTE red;
    BYTE green;
    BYTE blue;
} PIXEL;

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


// check the validity of the file and return 1 if valid
// it will be taking the convert variable to that we can check for the correct byte order depending
// if the machine is little or big endian
int validate_file(BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header)
{

    // ensure that we are working with a supported file type

        if (
            file_header.magic_number != 0x4D42 
            || file_header.offset != 54 
            || info_header.compression != 0 
            || info_header.bit_depth != 24 
            || info_header.header_size != 40
            ) 
        {
            printf("file type not supported!\n");

            return 22;
        }
        return 1;
}

// get the main file header provided the file pointer

// i decided not to convert the gathered data even if it is little endian because it is not necessary
// i just going to reverse the magic number so that we can check it
BITMAPFILEHEADER get_file_header(FILE * inputFile)
{
    // read the file header
    BITMAPFILEHEADER file_header;
    fread(&file_header, sizeof(BITMAPFILEHEADER), 1, inputFile);

    return file_header;
}

// ge tthe info file header provided the file pointer
BITMAPINFOHEADER get_info_header(FILE * inputFile)
{
    // read the file info header
    BITMAPINFOHEADER info_header;
    fread(&info_header, sizeof(BITMAPINFOHEADER), 1, inputFile);
    return info_header;
}

// get the hash of a given pixel
// useful for index and creating a hashmap of the pixels
BYTE hash_pixel(PIXEL pixel)
{
    return (pixel.red * 3 + pixel.green * 5 + pixel.blue * 7) % 64;
}
// this code basically checks for the endiannes of the underlying machine according to how the number 1 is stored in the machine
// this is done by extracting the first byte of the integer and comparing it with the number 1
// if the number 1 is stored in the first byte then it is a little endian machine
// if the number 1 is stored in the last byte then it is a big endian machine
// this work because c int is a 4 byte value

// the main function for encoding the image
int encode(char inputFileName[], char outputFileName[])
{
    // open the input file
    FILE *inputFile = fopen(inputFileName, "r");
    // get the main header
    BITMAPFILEHEADER file_header = get_file_header(inputFile);
    // get the info header
    BITMAPINFOHEADER info_header = get_info_header(inputFile);
    // validate the file
    int valid = validate_file(file_header, info_header);
    // if the file is not valid then exit
    if (valid != 1)
    {
        return valid;
    }
    // get pixel data to

    // 1. get the width and height of the image
    int width = info_header.width;
    int height = info_header.height;

    // 2. allocate memory

    // first i have used calloc instead of malloc because it is easier to deal with memory leaks and also it intializes the memory to zero
    // the first argumetn for calloc is the number of elements that we want to allocate in this case the height of the image because if we think about the image is basically
    // a 2d array then the height is the number of rows and the width is the number of columns in our case we want
    // the second thing is that i have to multiply width with the size of the pixel because each pixel is represented by 3 bytes so we multiply it by 3
    // so that we have enough room for each pixel in array because the width by default is the count of the pixel and since each pixel is 3 bytes hence we multiply it by 3

    // this syntax has to be used because if we think about it we want to get a 2d array so we have to define image as the pointer to first array or in other words the first row
    // that why we have to both set this as a pointer so that it is an array and also we have to specify a size which is width in this case because this is the number of columns
    // for each array inside the bigger array
    // so that the program moves the pointer row by row because we have specified a width
    // allocate size for the current image
    PIXEL(*image)
    [width] = calloc(height, sizeof(PIXEL) * width);

    // check if the memory is allocated properly
    if (image == NULL)
    {
        printf("memory allocation failed!\n");
        fclose(inputFile);
        return 12;
    }
    // iterate over memory and write each row into the memory allocated
    for (int i = 0; i < height; i++)
    {
        fread(image[i], sizeof(PIXEL), width, inputFile);
    }

    // -----------------------------------------------------------------------------------------------
    // Start Processing the pixels and encoding them and writing them to the output file
    // -----------------------------------------------------------------------------------------------

    // ------------------------------------------------
    // write the file header for the output file
    // ------------------------------------------------
    FILE *outputFile = fopen(outputFileName, "w");
    QOI_HEADER qoi_header;
    // if is in reverse assuming little endian
    qoi_header.magic = 0x66696f71;
    qoi_header.width = width;
    qoi_header.height = height;
    qoi_header.channels = 3;
    qoi_header.color_space = 1;
    fwrite(&qoi_header, sizeof(QOI_HEADER), 1, outputFile);

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



    // declare some variables before the encoding process
    // this is solely for a cleaner code and ease of troubleshooting




    const BYTE RGB_byte_flag=254; // 11111110
    const BYTE run_bit_flag=192; // 11000000
    const BYTE index_bit_flag=0; // 00000000 
    const BYTE small_difference_bit_flag=64; // 01000000
    const BYTE bigger_difference_bit_flag=128; // 10000000 


    // a run integer variable indicating the run value that will be written
    // for example if we have 8 consecutive bits that are the same 
    // we just store them in 3 bytes
    // two bytes will be the actuall pixel which will be the previous pixel
    // and the third one will be the run byte
    // first two bits will be 11 indicating the run and the last 6 bits
    // will be the run value
    // note that the run value should between 1 and 62 because 63 and 64 are 
    // reserved for other stuff
    int run =0;



    for ( int row =0 ; row < height ; row++ ){

        for ( int col = 0 ; col<width ; col++ ){
            // get the pixel
            current_pixel  = image[row][col];
            // difference from the previous pixel;
            int pixel_hash=hash_pixel(current_pixel);


            // when storing the data we have to make sure that we the difference values are
            // stored as unsigned integers with a bias of 2
            // e.g. 00 will indicate -2 ,01 will indicate -1, 10 will indicate 0, 11 will indicate 1
            // this is why we are going to add 2 to each value
            // but we are not going to add this 2 here because we use these variables in other places
            // but rather we will be adding this when we check for the condition
            U_BYTE diff_red = (current_pixel.red - previous_pixel.red);
            U_BYTE diff_green = (current_pixel.green - previous_pixel.green);
            U_BYTE diff_blue = (current_pixel.blue - previous_pixel.blue) ;

            // check for a run sequence
            if (
                current_pixel.red == previous_pixel.red 
            && current_pixel.green == previous_pixel.green 
            && current_pixel.blue == previous_pixel.blue
            && run < 64
            ){
                // if the pixel is the same as the previous pixel then we can skip it
                run++;                    
            }
            else if ( run > 0 ) 
            {
                // the bit flag is 11
                // if the pixel is not the same as the previous pixel then we can write the run
                // and there was a run sqeunce before we want to save that run sequence
                BYTE run_byte;
                // in order to create this run byte we will have to xor 11000000 and the run value
                // e.g. 00101001 this will create the byte 11101001 which is the run byte
                // please note the run length will be stored with a bias of -1 
                // i.e. we will subtracting 1 from the run value when we store this is mostly to include run length of 63 
                // but 64 is still not going to work becuase the 11111110 bit tag should be reserved for the rgb and with 64 we will get 11111111
                // after subtracting 1 it become 11111110 and it will be stored this way which will confuse the decoder

                run_byte = run_bit_flag ^ (run - 1);
                fwrite(&run_byte,sizeof(BYTE),1,outputFile);
            }

            // if there is no run sequence then check for small difference from the previous pixel

            else if (
                    -2 <= diff_red && diff_red <= 1
                    && -2<=diff_green && diff_green <= 1
                    && -2<=diff_blue && diff_blue <= 1
                ) {
                    // bit flag is 01
                    // when storing the data we have to make sure that we the difference values are
                    // stored as unsigned integers with a bias of 2
                    // e.g. 00 will indicate -2 ,01 will indicate -1, 10 will indicate 0, 11 will indicate 1
                    BYTE difference_byte;
                    difference_byte = ( small_difference_bit_flag ) ^ ( ( diff_red + 2 ) << 4 ) ^ ( ( diff_green + 2 ) << 2 ) ^ ( ( diff_blue +2 ) );
                    fwrite(&difference_byte,sizeof(BYTE),1,outputFile);
                }


            // before checking for bigger differences where pixels will be stored in two bytes rather than
            // one i will be checking for index variables because the index is stored in only one byte
            // hence should be more efficient
            else if (
                index_list[pixel_hash].red == current_pixel.red
                && index_list[pixel_hash].green == current_pixel.green
                && index_list[pixel_hash].blue == current_pixel.blue
            ){
                // the bit flag 00
                // if the pixel is the same as pixel in the index list then
                // encode this pixel as an index to that one 
                BYTE index_byte = index_bit_flag ^ pixel_hash;
                fwrite(&index_byte,sizeof(BYTE),1,outputFile);
            }

            // check for bigger differences from the previous pix
            // if so we will be encoding as a two byte pixel rather than the original 3
            else if (
                // the bit flag 10
                -32<=diff_green && diff_green <= 31
                && -8<=(diff_red-diff_green) && (diff_red-diff_green) <= 7
                && -8<=(diff_blue-diff_green) && (diff_blue-diff_green) <= 7
            )
            {
                // differences will be stored with a bias of 32 for the green channel
                // and of 8 for the red and blue channels
                // i.e. add 32 for the green difference and add 8 for the red and blue difference
                BYTE flag_and_green_difference_byte = bigger_difference_bit_flag ^ ( diff_green + 32 );
                BYTE red_and_blue_difference_to_green = (( diff_red-diff_green + 8 ) << 4 ) ^ ( diff_blue-diff_green + 8 );
                fwrite(&flag_and_green_difference_byte,sizeof(BYTE),1,outputFile);
                fwrite(&red_and_blue_difference_to_green,sizeof(BYTE),1,outputFile);
            }
            // if none of these conditions are met then we will store the pixel as a 4 byte pixel
            // this should seem a little bit intimidating and counterintuitive since we are storing
            // one extra byte but the hope here is that there will not be too many cases of these 
            else {
                // first write the RGB flag which in this case is the extra flag
                fwrite(&RGB_byte_flag,sizeof(BYTE),1,outputFile);
                // then write the red, green and blue values
                fwrite(&current_pixel,sizeof(PIXEL),1,outputFile);
            }
        }


    // }
    // // output file
    // FILE * outputFile = fopen(outputFileName, "w");
    // fwrite(&file_header, sizeof(BITMAPFILEHEADER), 1, outputFile);
    // fwrite(&info_header, sizeof(BITMAPINFOHEADER), 1, outputFile);
    // for (int i =0; i < height; i++){
    //     fwrite(image[i], sizeof(PIXEL), width, outputFile);
    // }
    }
    free(image);
    fclose(inputFile);
    fclose(outputFile);
    return 0;
}

// main function where the program starts
int main(int argc, char *argv[])
{
    // if the user doesn't supply the correct amount of command line arguments print an error message and prompt the user with the correct use
    // then exit
    if (argc != 3)
    {
        printf("Usage: ./encode <input file> <output file>\n");
        return 22;
    }
    // if the user supplies the correct amount of command line arguments, then print the following message:
    // and start the encoding process
    else
    {

        char *inputFileName;
        strcpy(inputFileName, argv[1]);
        char *outputFileName;
        strcpy(outputFileName, argv[2]);
        return encode(inputFileName, outputFileName);
    }
}

