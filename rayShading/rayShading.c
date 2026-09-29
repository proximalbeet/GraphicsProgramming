// uses the h file provided to gain access to stbi_write_png()
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// needed for sqrt() and sin()
#include <math.h>
// General utilities (malloc,drand48)
#include <stdlib.h>
// File I/O
#include <stdio.h>

// TODO Add these new features
// Diffuse shading
// Multiple Objects
// Support for triangles
// Reflections
// Shadows

int width = 512;
int height = 512;

int channels = 3;

// Defines a vector 3 type
typedef struct {
    double x,y,z;
} vec3;

// Definea a material type
typedef struct {
     float color[3];
     int reflective;
} material;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Equations
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

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

// TODO create a scalar function
vec3 scale(vec3 direction, double t) {
    vec3 result;
    result.x = direction.x * t;
    result.y = direction.y * t;
    result.z = direction.z * t;
    return result;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Ray
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

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
// Sphere Object
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct {

    vec3 pos;
    double radius;
    material mat;

} sphere;

double sphereIntersect(ray r1, sphere s1) {
    vec3 oc = subtract(r1.origin, s1.pos);
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
// TODO Triangle Object
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Object Calling
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// Define all materials that can be used
material refl = { .color = {0,0,0}, .reflective = 1 };
material blue = { .color = {0,0,1}, .reflective = 0 };
material red =  { .color = {1,0,0}, .reflective = 0 };
material white= { .color = {1,1,1}, .reflective = 0 };

// Global Light
vec3 light = {3, 5, -15};

sphere spheres[100];
int numSpheres = 0;

//TODO Uncomment when introduced
//triangle triangles[100];
//int numTriangles=0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Scene Builder
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

unsigned char *scene() {
    // Calculate total bytes Needed for memory allocation
    int bytes = width * height * channels;
    // Allocate memory to the heap for the image
    unsigned char *image = malloc(bytes);

    // Object Creation
    spheres[0] = (sphere) { .pos = { 0,0,-16 }, .radius = 2, .mat = blue };
    spheres[1] = (sphere) { .pos = { 3,-1,-14 }, .radius = 1, .mat = white };
    spheres[2] = (sphere) { .pos = { -3,-1,-14 }, .radius = 1, .mat = red };
    numSpheres = 3;
   
    // Builds the scene pixel by pixel
    for (int y = 0; y < height; y++) {
        
        for (int x = 0; x < width; x++) {
            // Used for defining rgb colors per pixel in the scene
            int index = (y * width + x) * channels;    

            ray r1;
            r1 = getRay(y, x);

            // Calculate if a ray hits a sphere based on the heiarchy of closest to camera
            double closestT = -1;
            int closestIndex = -1;
            for (int i = 0; i < numSpheres; i++) {
                double currentT = sphereIntersect(r1, spheres[i]);
                
                if (currentT > 0 && (closestT < 0 || currentT < closestT)) {
                    closestT = currentT;
                    closestIndex = i;
                }
            }
        
            if (closestT > 0) {

                // When reflective == 0, calculate the norm of the surface the ray hits. (different for spheres vs triangles)
                if (spheres[closestIndex].mat.reflective == 0) {
                    
                    vec3 hitPoint = add(r1.origin, scale(r1.dest, closestT));

                    // Calculates the norm of current sphere
                    vec3 surfaceNormal = normal(subtract(hitPoint, spheres[closestIndex].pos));

                    //TODO Calculates the norm of current triangle
                    
                    // Calculate the vector pointing at the light from the location where the ray hit. hard code this as global variable
                    vec3 lightDir = normal(subtract(light, hitPoint));

                    // Take the dot product of the normal vector and the vector pointing toward the light. Store in diffuse (if diffuse < 0.2 then diffuse = 0.2)
                    double diffuse = dot(surfaceNormal, lightDir);
                    if (diffuse < 0.2) diffuse = 0.2;

                    // TODO Set the color of the current pixel to material * diffuse * 255
                    image[index] = spheres[closestIndex].mat.color[0] * diffuse * 255;
                    image[index+1] = spheres[closestIndex].mat.color[1] * diffuse * 255;
                    image[index+2] = spheres[closestIndex].mat.color[2] * diffuse * 255;

                }

            }
            else {
                image[index] = 0;
                image[index+1] = 0;
                image[index+2] = 0;
            }
        }
    }
       return image;
}

// Builds the scene and writes it as an image
int main() {
    
    unsigned char *sceneImg = scene();
       
    stbi_write_png("reference.png", width, height, channels, sceneImg, width * channels);

    free(sceneImg);

    return 0;
}
