from PIL import Image
import PIL
import numpy as np
import sys
import os
try:
    # get the file name and prompt the user in case it wasn't supplied
    filename=sys.argv[1]
    # if more than one argument is given throw and error
    if len(sys.argv)>2:
        print('Usage python3 exract_pixels.py <filename>')
        sys.exit()
    # if only one argument is given, then we want to open the image in the same way as PIL does
    # and if the user forgets to add the -r flag, while passing a raw image file the program will give and error and exit ...
    else:
        # load image with PIL this is done so that we can get the raw pixels without the compression of jpg or whatever...
        image = Image.open(filename,'r')
        # get the raw pixels from the image
        pixels = np.array(image)
        # convert the raw pixels to a PIL image
        result = Image.fromarray(pixels)
        # save the image as a bmp image so that it is only pixels and not the compression of the image so that we can compress it 
        # using QOI image format
        result.save((f'{filename}.bmp'))
        # inform the user with some information about the size of the image before and after extracting the pixels and the file 
        # name of the bmp file that was created
        print(f'image size before extracting pixels: {os.path.getsize(filename)/1000} kilo  bytes')
        print(f'image size after extracting pixels: {os.path.getsize(f"{filename}.bmp")/1000} kilo bytes')
        print(f'extracted pixels to : {filename}.bmp')

except IndexError:
    print('Usage python3 exract_pixels.py <filename>')
    sys.exit()

# if the file is not found, the program will give an error and exit ...
except FileNotFoundError:
    print('File not found')
    sys.exit()
# if a wrong image format or a file other than an image is passed, the program will give an error and exit ...
except PIL.UnidentifiedImageError:
    print('File not recognized')
    print('note: if you are using a dng or raw image, please convert it to png to preserve full quality')
    print('here is a website that should be working : https://cloudconvert.com/dng-to-png')
    sys.exit()

