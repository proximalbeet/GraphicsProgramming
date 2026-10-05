// uses the h file provided to gain access to stbi_write_png()
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// needed for sqrt() and sin()
#include <math.h>
// General utilities (malloc,drand48)
#include <stdlib.h>
// File I/O
#include <stdio.h>


int width = 512;
int height = 512;


int channels = 3;


// create a vector 3 data type
typedef struct {
    double x,y,z;
} vec3;

// create a function for each modification of vector 3's

// Add
vec3 add(vec3 a, vec3 b) {
    vec3 result;
    result.x = (a.x + b.x);
    result.y = (a.y + b.y);
    result.z = (a.z + b.z);
    
    return result;
}
// Subtract
vec3 subtract(vec3 a, vec3 b) {
    vec3 result;
    result.x = (a.x - b.x);
    result.y = (a.y - b.y);
    result.z = (a.z - b.z);

    return result;
}

// Dot Product
double dot(vec3 a, vec3 b) {
    double result;
    double x = (a.x * b.x);
    double y = (a.y * b.y);
    double z = (a.z * b.z);
    result = x + y + z;
    
    return result;
}
// Length
double length(vec3 vector) {
    double result;
    double x = vector.x;
    double y = vector.y;
    double z = vector.z;
    result = sqrt((x * x) + (y * y) + (z * z)); 
    return result;
}

// Normalize
vec3 normal(vec3 vector) {
    vec3 result;
    double len = length(vector);
    result.x = vector.x / len;
    result.y = vector.y / len;
    result.z = vector.z / len;

    return result;
}

typedef struct {

    // Starting vector position (0,0,0)
    vec3 origin;

    // Normlized vector pointing from camera position
    vec3 dest; 
} ray;

ray getRay(int row, int col) {
    ray result;
    result.origin = (vec3) { 0, 0, 0 };
    //TODO Find the ray destinzation Z=-2
    double pixelSize = 2.0 / width;
    double worldX = (-1 + (col + 0.5) * pixelSize);
    double worldY = (1 - (row + 0.5) * pixelSize);
    result.dest = (vec3) { worldX, worldY, -2 };
    result.dest = normal(result.dest);
    return result;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

typedef struct {

    vec3 center;
    double radius;

} sphere;

double sphereIntersect(ray r1, sphere s1) {
    vec3 oc = subtract(r1.origin, s1.center);
    double a = dot(r1.dest, r1.dest);
    double b = 2 * dot(oc, r1.dest);
    double c = dot(oc, oc) - s1.radius * s1.radius;
    double discriminant = b*b - 4*a*c; 

    if (discriminant < 0) {
        return -1;
    }
    else{
        return ((-b - sqrt(discriminant)) / (2*a));
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

unsigned char *scene() {
    // Calculate total bytes Needed for memory allocation
    int bytes = width * height * channels;

    // Create a sphere object
    sphere s1;
    s1.center = (vec3) { 2, 2, -16 };
    s1.radius = 5.3547;

    // Allocate memory to the heap for the image
    unsigned char *image = malloc(bytes);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int index = (y * width + x) * channels;
            
            ray r1;
            r1 = getRay(y, x);

            double t = sphereIntersect(r1, s1);

            if (t > 0) {
            image[index] = 255;
            image[index+1] = 255;
            image[index+2] = 255;

            }
            else {
            image[index] = 128;
            image[index+1] = 0;
            image[index+2] = 0;

            }
        }
    }
       return image;
}


int main() {
    
    unsigned char *sceneImg = scene();


    //Get the bottom left pixel
    ray r1 = getRay(511, 0);
    // Get the top right pixel
    ray r2 = getRay(0, 511);
    // Get the Middle pixel
    ray r3 = getRay(256, 255);

    //Print the 3 values
    printf("Bottom left pixel\n");
    printf("RayPosition %f %f %f\n", r1.origin.x, r1.origin.y, r1.origin.z);
    printf("RayDirection %f %f %f\n", r1.dest.x, r1.dest.y, r1.dest.z);
    printf("\n");
    printf("Top right pixel\n");
    printf("RayPosition %f %f %f\n", r2.origin.x, r2.origin.y, r2.origin.z);
    printf("RayDirection %f %f %f\n", r2.dest.x, r2.dest.y, r2.dest.z);
    printf("\n");
    printf("Middle pixel\n");
    printf("RayPosition %f %f %f\n", r3.origin.x, r3.origin.y, r3.origin.z);
    printf("RayDirection %f %f %f\n", r3.dest.x, r3.dest.y, r3.dest.z);
    
    stbi_write_png("sphere.png", width, height, channels, sceneImg, width * channels);

    free(sceneImg);

    return 0;
}
