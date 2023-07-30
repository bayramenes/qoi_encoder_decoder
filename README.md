# QOI (Quite Ok Image) Format !

this is and image format that was created by Dominic Szablewski
in Novermber of 2021...

It is a lossless image format which means that it will retain all the data of the image even after compression...

for comparison the widely used JEPG/JPG is a lossy image format that sacrifices a little bit of the data in order to have better compression...

On the other end PNG which is also widely used is a lossless image compression algorithm just like the one here called : QOI

## Who Cares ??
Well. Even though PNG is fine and widely used it is a little bit on the complicated side and is known for its slow speed...
This is why Dominic have though of another algorithm and came up with QOI 

For comparison QOI is much much faster than PNG though it might some times not achieve same compression rations it is sometimes 30x the speed


### This Project
It all started for me when i watched [this](https://www.youtube.com/watch?v=EFUYNoFRHQI) amazing video from a youtube channel called Reducible...

This all got me thinking why not try and implement QOI since i felt like it was simpler...

Now full disclaimer i am by no means a C developer and i had to learn C for the sole purpose of this project though i had some interaction with the language in the past...

So if you have any suggestions or found any bugs with the code please notify me and/or help me...

# Setup

To get this to work we have to convert the image that we have into the BMP file format since it is easier to work with and it doesn't have any compression so that we can get the full pixel data...

you can convert the image on your own to BMP and use it or you can use the python script in the project to setup the enviroment and then convert to BMP...


### clone the repo
```
git clone https://github.com/bayramenes/qoi_encoder_decoder.git
```

### Setup the enviroment 


#### for windows
```
python setup.py
```

#### for MacOs/Linux
```
python3 setup.py
```

follow the prompts from the script and wait until done...

##### finally activate the enviroment

### Convert to BMP

make sure that the virtual env is activated

```
python extract_pixels.py <input image>
```


### Compile the code

#### Encoder
you can use any compiler you want

#### GCC
```
gcc encode.c -o encode
```

#### CLANG 
```
clang encode.c -o encode
```

