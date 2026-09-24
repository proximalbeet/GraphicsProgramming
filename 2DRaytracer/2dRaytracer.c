// uses the h file provided to gain access to stbi_write_png()
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// needed for sqrt() and sin()
#include <math.h>
// General utilities (malloc,drand48)
#include <stdlib.h>
// File I/O
#include <stdio.h>

// TODO write a 512x512 white img to disk
int width = 512;
int height = 512;
// Represents how many bytes describe one pixel's color. (RGB)
int channels = 3;

unsigned char *checkerboard() {
    // Calculate total bytes Needed for memory allocation
    int bytes = width * height * channels;
    
    // Allocate memory to the heap for the image
    unsigned char *image = malloc(bytes);

      // Loop over every row (y) and column (x) as an array for the image
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Holds the position of a pixel in the array (base = 0)
            int index = (y * width + x) * channels;
         
                   
            int tileRow = y / 64;
            int tileCol = x / 64;

            
            if ((tileRow + tileCol) % 2 == 0 ) {
                image[index] = 255;
                image[index+1] = 0;
                image[index+2] = 0;
            }
            else {
                image[index] = 0;
                image[index+1] = 0;
                image[index+2] = 255;
            }
        }
    }  
    return image;
}

unsigned char *gradient() {
     // Calculate total bytes Needed for memory allocation
    int bytes = width * height * channels;
    
    // Allocate memory to the heap for the image
    unsigned char *image = malloc(bytes);

      // Loop over every row (y) and column (x) as an array for the image
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Holds the position of a pixel in the array (base = 0)
            int index = (y * width + x) * channels;
         
            //Red channel is off on the left side of the image and on on the right side
            image[index] = ((double)x / width) * 255;   
            //Green channel is off on the top of the image and on on the bottom
            image[index+1] = ((double)y / height) * 255;
            //Blue channel will be set to 128 (middle) across the entire image.
            image[index+2] = 128;
            //top left pixel (0,0,128)
            //bottom right pixel (255,255,128)
        }
    }  
    return image;
  }

int main()
{
    unsigned char *checkerboardImg = checkerboard();
    unsigned char *gradientImg = gradient();

    // Write the array to the disk as a png
    //stbi_write_png("checkerboard.png", width, height, channels, image, width * channels);
    stbi_write_png("checkerboard.png", width, height, channels, checkerboardImg, width * channels);
    stbi_write_png("gradient.png", width, height, channels, gradientImg, width * channels);

    free(checkerboardImg);
    free(gradientImg);

    return 0;
}
