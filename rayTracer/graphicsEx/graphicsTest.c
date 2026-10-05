#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int DEBUG=0;

int size = 256;
int center = 128;


void processPixel(int x, int y, unsigned char *color) {
   

    if(y < size/10) {

	if(drand48() < .001)
	    color[0] = color[1] = color[2] = 255; 
	else
	    color[0] = color[1] = color[2] = 0;
	return;

    }

    float zoom = y/(float) center;
    x = (x-center)/zoom + center;
    y = (y-center)/zoom + center;

    float distance = sqrt((x-center)*(x-center) + (y-center)*(y-center));
    float brightness = (sin(distance/10)+1)/2;

    color[0] = 0;
    color[1] =  (1-brightness)*255 * .5;
    color[2] = brightness*255 * .5;
}

int showStats() {
    return 1;
}


int main() {
    unsigned char color[3];
    FILE *f = fopen("output.ppm", "wb");
    fprintf(f, "P6\n%d %d\n255\n", size, size);

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            processPixel(x, y, color);
            fwrite(color, 1, 3, f);
        }
    }

    fclose(f);
    return 0;
}
