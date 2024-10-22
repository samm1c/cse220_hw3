#include "image.h"

// part 1
Image *load_image(char *filename) {

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) { // no file found
        return NULL;
    }

    char format[10]; // buffer for P3, +1 for null char
    fscanf(fp, "%2s ", format); // P3
    
    // skip comment(s) -> store width and height
    unsigned int width = 0, height = 0;
    while (1) { // loop will break by itself
        char line[256]; // char array buffer to store second line
        fgets(line, sizeof(line), fp); // entire scanner skips over line by storing it in buffer called line
        if (line[0] == '#') {
            continue; // keep skipping comment lines
        } else { 
            if (sscanf(line, "%u %u ", &width, &height) == 2) { // successfully stored 2 items; sscanf takes a string and reads it
                break;
            }
        }
    }
    
    // store max intensity
    unsigned char intensity = 0;
    int temp = 0; // scan as int, convert to char
    fscanf(fp, "%d ", &temp);
    intensity = (char)temp;
    //printf("width: %u \t height: %u \t intensity: %d \n", width, height, intensity);

    unsigned int **pixels = malloc(height * sizeof(unsigned int *)); // create dynamic array by malloc'ing
    for (unsigned int i = 0; i < height; i++) { // malloc per row
        pixels[i] = malloc(width * sizeof(unsigned int));
    }
    unsigned int r = 0, g = 0, b = 0; // stores R G B 
    for (unsigned int p = 0; p < height; p++) { // p -> pixels array index
        for (unsigned int q = 0; q < width; q++) {
            fscanf(fp, "%u %u %u ", &r, &g, &b);
            //printf("%u ", r);
            pixels[p][q] = r; // still dereferences pixels; same as *(pixels + p) but syntactic sugar
        }
        //printf("\n");
    }

    fclose(fp);

    Image *img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->intensity = intensity;
    img->pixels = pixels;

    return img;
}

void delete_image(Image *image) {
    if (image != NULL) { // make sure image actually exists first
        for (unsigned int i = 0; i < (image->height); i++) {// deallocate each row
            free(image->pixels[i]);
        }
        free(image->pixels); // deallocate 2D reference
        image->pixels = NULL; // pixels officially point to nothing
        free(image); // free object
    }
}

unsigned char get_image_intensity(Image *image, unsigned int row, unsigned int col) {
    return image->pixels[row][col];
}

unsigned short get_image_width(Image *image) {
    return image->width;
}

unsigned short get_image_height(Image *image) {
    return image->height;
}



// part 2
unsigned int hide_message(char *message, char *input_filename, char *output_filename) {
    (void)message;
    (void)input_filename;
    (void)output_filename;
    return 0;
}

char *reveal_message(char *input_filename) {
    (void)input_filename;
    return NULL;
}

unsigned int hide_image(char *secret_image_filename, char *input_filename, char *output_filename) {
    (void)secret_image_filename;
    (void)input_filename;
    (void)output_filename;
    return 10;
}

void reveal_image(char *input_filename, char *output_filename) {
    (void)input_filename;
    (void)output_filename;
}
