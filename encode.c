#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"



// check the validity of the file and return 1 if valid
int validate_file(BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header){

    // ensure that we are working with a supported file type
    if (file_header.magic_number != 0x4D42 || file_header.offset!=54 || info_header.compression!=0 || info_header.bit_depth!=24 || info_header.header_size!=40 ){
        printf("file type not supported!\n");

        return 22;
    }
    return 1;

}

// get the main file header provided the file pointer
BITMAPFILEHEADER get_file_header(FILE * inputFile){
    // read the file header
    BITMAPFILEHEADER file_header;
    fread(&file_header, sizeof(BITMAPFILEHEADER), 1, inputFile);
    return file_header;
}

// ge tthe info file header provided the file pointer
BITMAPINFOHEADER get_info_header(FILE * inputFile){
    // read the file info header
    BITMAPINFOHEADER info_header;
    fread(&info_header, sizeof(BITMAPINFOHEADER), 1, inputFile);
    return info_header;
}

// the main function for encoding the image
int encode(char inputFileName[], char outputFileName[]){
    // open the input file
    FILE * inputFile = fopen(inputFileName, "r");
    // get the main header
    BITMAPFILEHEADER file_header = get_file_header(inputFile);
    // get the info header
    BITMAPINFOHEADER info_header = get_info_header(inputFile);
    // validate the file
    int valid = validate_file(file_header, info_header);
    // if the file is not valid then exit
    if (valid !=1){
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
    PIXEL (*image)[width]= calloc(height, sizeof(PIXEL)* width);

    // check if the memory is allocated properly
    if (image==NULL){
        printf("memory allocation failed!\n");
        fclose(inputFile);
        return 12;
    }
    // iterate over memory and write each row into the memory allocated
    for (int i =0 ; i < height; i++){
        fread(image[i], sizeof(PIXEL), width, inputFile);
    }





    // output file
    FILE * outputFile = fopen(outputFileName, "w");
    fwrite(&file_header, sizeof(BITMAPFILEHEADER), 1, outputFile);
    fwrite(&info_header, sizeof(BITMAPINFOHEADER), 1, outputFile);
    for (int i =0; i < height; i++){
        fwrite(image[i], sizeof(PIXEL), width, outputFile);
    }


    free(image);
    fclose(inputFile);



    fclose(outputFile);
    return 0;
    
}


// main function where the program starts
int main(int argc, char * argv[]){
    // if the user doesn't supply the correct amount of command line arguments print an error message and prompt the user with the correct use
    // then exit
    if (argc != 3){
        printf("Usage: ./encode <input file> <output file>\n");
        return 22;

    }   
// if the user supplies the correct amount of command line arguments, then print the following message:
// and start the encoding process
    else {

            char *inputFileName;
            strcpy(inputFileName, argv[1]);
            char *outputFileName;
            strcpy(outputFileName, argv[2]);
            return encode(inputFileName, outputFileName);

    }
}
