// uses the h file provided to gain access to stbi_write_png()
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// needed for sqrt()
#include <math.h>
// General utilities (calloc)
#include <stdlib.h>
// File I/O
#include <stdio.h>

int width = 512;
int height = 512;

int channels = 3;

// Defines a vector 3 type
typedef struct {
    double x,y,z;
} vec3;

// Defines a material type
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

// Applies a scalar to a vector
vec3 scale(vec3 direction, double t) {
    vec3 result;
    result.x = direction.x * t;
    result.y = direction.y * t;
    result.z = direction.z * t;
    return result;
}

// Get the cross product of two vectors
vec3 cross(vec3 e1, vec3 e2) {
    vec3 result;
    result.x = ( (e1.y * e2.z) - (e1.z * e2.y) );
    result.y = ( (e1.z * e2.x) - (e1.x * e2.z) );
    result.z = ( (e1.x * e2.y) - (e1.y * e2.x) );
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

// Builds the camera ray for the pixel its currently selected on
ray getRay(int row, int col) {
    ray result;
    result.origin = (vec3) { 0, 0, 0 };
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

// Tests weather the ray hits a sphere and returns the distance t or -1 on a miss
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
// Triangle Object
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct {
    vec3 v[3];
    material mat;
} triangle;

// Tests weather the ray hits a triangle and returns the distance t or -1 on a miss
double triangleIntersect(ray r1, triangle t1) {
  
    // compute the 12 values A-L, then M as doubles
    double A = (t1.v[0].x - t1.v[1].x);
    double B = (t1.v[0].y - t1.v[1].y);
    double C = (t1.v[0].z - t1.v[1].z);
    double D = (t1.v[0].x - t1.v[2].x);
    double E = (t1.v[0].y - t1.v[2].y);
    double F = (t1.v[0].z - t1.v[2].z);
    double G = r1.dest.x;
    double H = r1.dest.y;
    double I = r1.dest.z;
    double J = (t1.v[0].x - r1.origin.x);
    double K = (t1.v[0].y - r1.origin.y);
    double L = (t1.v[0].z - r1.origin.z);

    // Compute M
    double M = A*(E*I - H*F) + B*(G*F - D*I) + C*(D*H - E*G);
    
    // Compute Beta
    double beta = ( (J*(E*I - H*F) + K*(G*F - D*I) + L*(D*H - E*G) ) / M );

    // Compute Gamma
    double gam = ( (I*(A*K - J*B) + H*(J*C - A*L) + G*(B*L - K*C)) / M );

    // Compute t
    double t = ( (-1 * (F*(A*K - J*B) + E*(J*C - A*L) + D*(B*L - K*C))) / M );
        
    // Checks if the triangle is outside or behind the ray
    if (t < 0) {
        return -1;
    }
    if (gam < 0 || gam > 1) {
        return -1;
    }
    if (beta < 0 || beta > 1 - gam) {
        return -1;
    }
    
    return t;
}

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

triangle triangles[100];
int numTriangles=0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Shadows
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Return 1 if object blocks the light from hitPoint, otherwise return 0.
int inShadow(vec3 hitPoint) {
    vec3 toLight = subtract(light, hitPoint);
    double lightDist = length(toLight);
    ray shadowRay;
    shadowRay.dest = normal(toLight);
    // The 0.001 nudge prevents shadow acne from occuring
    shadowRay.origin = add(hitPoint, scale(shadowRay.dest, 0.001));

    double t = 0;

    for (int i = 0; i < numSpheres; i++) {
        t = sphereIntersect(shadowRay, spheres[i]);

        if (t > 0 && t < lightDist) return 1;
    }

    for (int j = 0; j < numTriangles; j++) {
        t = triangleIntersect(shadowRay, triangles[j]);
        
        if (t > 0 && t < lightDist) return 1;
    }

    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Scene Builder
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

unsigned char *scene() {
    // Calculate total bytes Needed for memory allocation
    int bytes = width * height * channels;

    // Allocate memory to the heap for the image
    unsigned char *image = calloc(bytes, 1);

    // Object Creation
    spheres[0] = (sphere) { .pos = { 0,0,-16 }, .radius = 2, .mat = refl };
    spheres[1] = (sphere) { .pos = { 3,-1,-14 }, .radius = 1, .mat = refl };
    spheres[2] = (sphere) { .pos = { -3,-1,-14 }, .radius = 1, .mat = red };
    numSpheres = 3;
   
    // back wall
    triangles[0] = (triangle) { .v = { { -8,-2,-20 }, {8,-2,-20}, {8,10,-20} }, .mat = blue };
    triangles[1] = (triangle) { .v = { { -8,-2,-20 }, {8,10,-20}, {-8,10,-20} }, .mat = blue };
    // floor
    triangles[2] = (triangle) { .v = { { -8,-2,-20 }, {8,-2,-10}, {8,-2,-20}}, .mat = white };
    triangles[3] = (triangle) { .v = { { -8,-2,-20 }, {-8,-2,-10}, {8,-2,-10}}, .mat = white };
    // right red triangle
    triangles[4] = (triangle) { .v = { { 8,-2,-20 }, {8,-2,-10}, {8,10,-20}}, .mat = red };
    numTriangles = 5;
    
    // Builds the scene pixel by pixel
    for (int y = 0; y < height; y++) {
        
        for (int x = 0; x < width; x++) {
            // Used for defining rgb colors per pixel in the scene
            int index = (y * width + x) * channels;    

            ray r1;
            r1 = getRay(y, x);

            for (int bounce = 0; bounce < 10; bounce++) {

                // Calculate if a ray hits a sphere or a triangle based on the heiarchy of closest to camera
                double closestT = -1;
                int closestIndex = -1;
                int hitTriangle = 0;
                for (int i = 0; i < numSpheres; i++) {
                    double currentS = sphereIntersect(r1, spheres[i]);
                
                    if (currentS > 0 && (closestT < 0 || currentS < closestT)) {
                        closestT = currentS;
                        closestIndex = i;
                        hitTriangle = 0;
                    }
                }
                for (int i = 0; i < numTriangles; i++) {
                    double currentT = triangleIntersect(r1, triangles[i]);
                
                    if (currentT > 0 && (closestT < 0 || currentT < closestT)) {
                        closestT = currentT;
                        closestIndex = i;
                        hitTriangle = 1;
                    }
                }

                if (closestT > 0) {
                    if (hitTriangle == 1 && triangles[closestIndex].mat.reflective == 0) {
                    
                        vec3 hitPoint = add(r1.origin, scale(r1.dest, closestT));

                        // Calculates the norm of current triangle
                        vec3 edge1 = subtract(triangles[closestIndex].v[1], triangles[closestIndex].v[0]);
                        vec3 edge2 = subtract(triangles[closestIndex].v[2], triangles[closestIndex].v[0]);
                        vec3 surfaceNormal = normal(cross(edge1, edge2));
                    
                        // Calculate the vector pointing at the light from the location where the ray hit. 
                        vec3 lightDir = normal(subtract(light, hitPoint));

                        // Take the dot product of the normal vector and the vector pointing toward the light. Store in diffuse (if diffuse < 0.2 then diffuse = 0.2)
                        double diffuse = dot(surfaceNormal, lightDir);
                        if (diffuse < 0.2) diffuse = 0.2;
                        if (inShadow(hitPoint) == 1) diffuse = 0.2;

                        image[index] = triangles[closestIndex].mat.color[0] * diffuse * 255;
                        image[index+1] = triangles[closestIndex].mat.color[1] * diffuse * 255;
                        image[index+2] = triangles[closestIndex].mat.color[2] * diffuse * 255;
                        break;
                    }

                    // When reflective == 0, calculate the norm of the surface the ray hits. (different for spheres vs triangles)
                    if (hitTriangle == 0 && spheres[closestIndex].mat.reflective == 0) {
                    
                        vec3 hitPoint = add(r1.origin, scale(r1.dest, closestT));

                        // Calculates the norm of current sphere
                        vec3 surfaceNormal = normal(subtract(hitPoint, spheres[closestIndex].pos));

                        // Calculate the vector pointing at the light from the location where the ray hit. 
                        vec3 lightDir = normal(subtract(light, hitPoint));

                        // Take the dot product of the normal vector and the vector pointing toward the light. Store in diffuse (if diffuse < 0.2 then diffuse = 0.2)
                        double diffuse = dot(surfaceNormal, lightDir);
                        if (diffuse < 0.2) diffuse = 0.2;
                        if (inShadow(hitPoint) == 1) diffuse = 0.2;
                    
                        image[index] = spheres[closestIndex].mat.color[0] * diffuse * 255;
                        image[index+1] = spheres[closestIndex].mat.color[1] * diffuse * 255;
                        image[index+2] = spheres[closestIndex].mat.color[2] * diffuse * 255;
                        break;
                    }


                    if (hitTriangle == 1 && triangles[closestIndex].mat.reflective == 1) {
              
                         vec3 hitPoint = add(r1.origin, scale(r1.dest, closestT));
    
                        // Calculates the norm of current triangle
                        vec3 edge1 = subtract(triangles[closestIndex].v[1], triangles[closestIndex].v[0]);
                        vec3 edge2 = subtract(triangles[closestIndex].v[2], triangles[closestIndex].v[0]);
                        vec3 surfaceNormal = normal(cross(edge1, edge2));
                    
                         // Calculate the reflection using the formula r = d - 2(d*n)n
                        double dn = dot(r1.dest, surfaceNormal);
                        r1.dest = subtract(r1.dest, scale(surfaceNormal, 2*dn) );
                        r1.origin = add(hitPoint, scale(r1.dest, 0.001));
                    }
                
                    if (hitTriangle == 0 && spheres[closestIndex].mat.reflective == 1) {
                    
                        vec3 hitPoint = add(r1.origin, scale(r1.dest, closestT));

                        // Calculates the norm of current sphere
                        vec3 surfaceNormal = normal(subtract(hitPoint, spheres[closestIndex].pos));

                       
                        // Calculate the reflection using the formula r = d - 2(d*n)n
                        double dn = dot(r1.dest, surfaceNormal);
                        r1.dest = subtract(r1.dest, scale(surfaceNormal, 2*dn) );
                        r1.origin = add(hitPoint, scale(r1.dest, 0.001));
                    }
                }
                else {
                    image[index] = 0;
                    image[index+1] = 0;
                    image[index+2] = 0;
                    break;
                }
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
