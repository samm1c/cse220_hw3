#include "image.h"
#include <string.h>

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



// part 3

// message -> last bit of each pixel
unsigned int hide_message(char *message, char *input_filename, char *output_filename) {

    // file handler -> read the input file
    FILE *f_input = fopen(input_filename, "r");
    char line[256]; // buffer

    // file handler -> write to the output file
    FILE *f_output = fopen(output_filename, "w");

    // figure out how long string is -> N 
    unsigned int N = strlen(message);

    // figure out how many pixels is in input -> M
    // skip first line (+ any comments)
    for (int i = 0; i < 1; i++) {
        fgets(line, sizeof(line), f_input);
        if (line[0] == '#') {
            i--; // decrement because it's a comment so you want to iterate one MORE time
        }
    }

    // count M
    unsigned int M, h, w;
    fscanf(f_input, "%u %u", &h, &w);
    M = h * w; // -> number of total pixels is height * width
    printf("M: %d\n", M);

    // put back at beginning of file
    rewind(f_input);

    // to skip over first 3 lines (assume comments ommitted)
    for (int i = 0; i < 3; i++) {
        fgets(line, sizeof(line), f_input); // put the line into the char array line
        fprintf(f_output, "%s", line);
        if (line[0] == '#') {
            i--; // decrement because it's a comment so you want to iterate one MORE time
        }
    }

    unsigned int r, g, b; // placeholders for intensity
    int lim = 0; // determines how many characters to encode

    if (M < (8 * (N + 1))) { // not enough pixels for the message!; N+1 for null character
        //printf("M - 1 : %d\n", M - 1);
        //printf("M / 8 : %d\n", M/8);
        M -= 8; // reserve for the null character
        lim = (M / 8);
    } else { 
        lim = N;
    }

    printf("lim: %d\n", lim);

    for (int a = 0; a < lim; a++) { // a doesn't matter, just to count however many times up to lim
        char c = *message;
        for (int i = 7; i >= 0; i--) { // 1 ASCII character -> 8 bits -> 8 pixels
            // take in the set of 3 rgb (grayscale) values for 1 single pixel and change them
            fscanf(f_input, "%u %u %u ", &r, &g, &b);
            
            unsigned int k = (c >> i) & 1; // bit to insert into rgb; & 1 is so that you only take the last bit and ignore the rest
            
            // set/modify last bit -> r is changed
            r &= ~(1); // clear the bit first
            r |= k; // add what we need (0 or 1)
            
            printf("r: %d \t k: %u \t c: %c\n", r, k, c);
            fprintf(f_output, "%u %u %u\n", r, r, r);
        }
        //printf("r: %d \t c: %c\n", r, c);
        message++; // next character
    }

    // include null character!!!! \0 -> 0000 in ASCII
    for (int i = 0; i < 8; i++) {
        fscanf(f_input, "%u %u %u ", &r, &g, &b);
        printf("r: %u\t", r);
        r &= ~(1); // just zero out the last bit
        printf("r: %u\n", r);
        fprintf(f_output, "%u %u %u\n", r, r, r);
    }

    // write the rest of the input file into output -> leave the rest of the rgb/intensity/values alone!!!! no more secrete message!!!
    while (fscanf(f_input, "%u %u %u ", &r, &g, &b) == 3) {
        fprintf(f_output, "%u %u %u\n", r, g, b);
        printf("r: %u\n", r);
    }

    fclose(f_input);
    fclose(f_output);
    
    return 0; // success!!!
}

char *reveal_message(char *input_filename) {
    
    FILE *fp = fopen(input_filename, "r");

    // skip first 3 lines
    char line[256]; // buffer
    for (int i = 0; i < 3; i++) {
        fgets(line, sizeof(line), fp);
        if (line[0] == '#') { // comment! continue loop
            i--;
        }
    }

    int end = 0; // flag to signal null character
    int index = 0; // also used to count length 
    int cap = 10;
    // i did NOT want to count how long msg was and THEN allocate mem, so am using realloc instead
    char *message = malloc(cap * sizeof(char)); // we are handling msg dynamically and reallocating memory as necessary;
    unsigned int r, g, b;
    while (!end) {

        char c = 0; // incoming character; NOT '0' which is 48 in ASCII

        if (cap <= (index + 1)) { // not enough memory!
            cap *= 2;
            message = realloc(message, cap * sizeof(char));
        }
        
        for (int i = 7; i >= 0; i--) { // one character -> 8 ASCII -> 8 bits -> 8 pixels
            fscanf(fp, "%u %u %u ", &r, &g, &b);
            int last_bit = r & 1; // only want last bit
            c |= last_bit << i; // shift it into position
        }

        message[index++] = c;

        if (c == '\0') {
            end = 1; // null character!
        }
        
    }

    fclose(fp);

    return message;
}

// image ->
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
