#include "qtree.h"
#include <math.h> // to perform calculations

// function prototypes because i don't like writing helper functions before the actual function
QTNode *create_quadtree_helper(Image *image, unsigned int r, unsigned int c, unsigned int h, unsigned int w, double max_rmse);
void save_qtree_as_ppm_helper(QTNode *root, unsigned int **pixels);
QTNode *load_preorder_qt_helper(FILE *fp, int k);

// part 2
QTNode *create_quadtree(Image *image, double max_rmse) {
    return create_quadtree_helper(image, 0, 0, image->height, image->width, max_rmse);
}

QTNode *create_quadtree_helper(Image *image, unsigned int r, unsigned int c, unsigned int h, unsigned int w, double max_rmse) { 
    // calculate RMSE first and then initialize root
    double sum = 0;
    double average, RMSE;
    // calculate average
    for (unsigned int i = r; i < (r + h); i++) {
        for (unsigned int j = c; j < (c + w); j++) {
            sum += image->pixels[i][j];
        }
    }
    average = sum / (h * w);

    // calculate using RMSE formula
    sum = 0; // reusing sum variable
    for (unsigned int i = r; i < (r + h); i++) { // iterate through every pixel to find sum of difference squared
        for (unsigned int j = c; j < (c + w); j++) {
            sum += pow((image->pixels[i][j] - average), 2);
        }
    }
    RMSE = sqrt(sum / (h * w));

    // initialize root
    QTNode *root = malloc(sizeof(QTNode)); // malloc first
    root->intensity = (unsigned char)average;
    root->row = r;
    root->col = c;
    root->height = h;
    root->width = w;
    //printf("row: %u, col: %u, height: %u, width: %u\n", r, c, h, w);
    //printf("Average: %f, RMSE: %f\n", average, RMSE);
    
    if (RMSE <= max_rmse || (h == 1 && w == 1)) { // base case -> leaf node
        //printf("leaf node!\n");
        for (int i = 0; i < 4; i++) {
            root->children[i] = NULL;
        }
    } else if (h == 1) { // single row -> child 3 and 4 NULL
        unsigned int half_w = w / 2; // single horizontal row -> split width in half, keep height of 1
        (root->children[0]) = create_quadtree_helper(image, r, c, h, half_w, max_rmse);
        (root->children[1]) = create_quadtree_helper(image, r, c + half_w, h, w - half_w, max_rmse);
        (root->children[2]) = NULL;
        (root->children[3]) = NULL;
    } else if (w == 1) { // single column -> child 2 and 4 NULL
        unsigned int half_h = h / 2;
        (root->children[0]) = create_quadtree_helper(image, r, c, half_h, w, max_rmse);
        (root->children[1]) = NULL;
        (root->children[2]) = create_quadtree_helper(image, r + half_h, c, h - half_h, w, max_rmse);
        (root->children[3]) = NULL;
    } else { // error is greater than necessary -> bad -> split the node -> normal 4 children 
        // variables just to make life easier
        unsigned int half_h = h / 2;
        unsigned int half_w = w / 2;
        (root->children[0]) = create_quadtree_helper(image, r, c, half_h, half_w, max_rmse);
        (root->children[1]) = create_quadtree_helper(image, r, c + half_w, half_h, w - half_w, max_rmse);
        (root->children[2]) = create_quadtree_helper(image, r + half_h, c, h - half_h, half_w, max_rmse);
        (root->children[3]) = create_quadtree_helper(image, r + half_h, c + half_w, h - half_h, w - half_w, max_rmse);
    }
    return root;
}

QTNode *get_child1(QTNode *node) {
    return node->children[0];
}

QTNode *get_child2(QTNode *node) {
    return node->children[1];
}

QTNode *get_child3(QTNode *node) {
    return node->children[2];
}

QTNode *get_child4(QTNode *node) {
    return node->children[3];
}

unsigned char get_node_intensity(QTNode *node) {
    return node->intensity;
}

void delete_quadtree(QTNode *root) {
    if (root == NULL) { // base case
        return;
    } else {
        for (int i = 0; i < 4; i++) { // must recursively free up children b/c you don't know how big the tree is
            delete_quadtree(root->children[i]);
        }
        free(root);
    }
}

void save_qtree_as_ppm(QTNode *root, char *filename) {
    
    // open file and initialize file handler (fp)
    FILE *fp = fopen(filename, "w");

    // ppm header (first 3 lines)
    fprintf(fp, "P3\n%u %u\n255\n", root->width, root->height); // 255 is always assumed as MAX INTENSITY; root->intensity is the AVERAGE INTENSITY
    
    // create a 2D array to hold all of the pixels, which we will iterate over and write to file that way b/c it's easier
    // MALLOC 2D ARRAY FIRST
    unsigned int **pixels = malloc(root->height * sizeof(unsigned int *)); // create dynamic array by malloc'ing all rows first
    for (unsigned int i = 0; i < root->height; i++) { // malloc columns per row
        pixels[i] = malloc(root->width * sizeof(unsigned int));
    }
    
    // initialize said array using quadtree
    save_qtree_as_ppm_helper(root, pixels);

    // using new 2D array (pixels), write to the file in ROW-MAJOR order by just iterating over it 
    for (unsigned int i = 0; i < root->height; i++) {
            for (unsigned int j = 0; j < root->width; j++) {
                fprintf(fp, "%u %u %u\n", pixels[i][j], pixels[i][j], pixels[i][j]);
            }
    }

    // FREE 2D ARRAY 
    for (unsigned int i = 0; i < (root->height); i++) { // deallocate each row
            free(pixels[i]);
        }
    free(pixels); // deallocate 2D reference

    // close file handler
    fclose(fp);

}

// helper function because PPM file header must be written by main function separately; recursively iterates over quadtree and stores in 2D array (pixels) in the correct indices
void save_qtree_as_ppm_helper(QTNode *root, unsigned int **pixels) {

    if (root == NULL) { // base case -> do nothing
        return;
    } else if ((root->children[0] == NULL) && (root->children[1] == NULL) && (root->children[2] == NULL) && (root->children[3] == NULL)) { // leaf node! store inside pixels
        // put the intensity into the according region (square) inside the 2D array
        for (unsigned int i = root->row; i < root->row + root->height; i++) {
            for (unsigned int j = root->col; j < root->col + root->width; j++) {
                pixels[i][j] = root->intensity;
            }
        }
    } else { // recursively call because it's an internal node -> not compressed enough yet
        for (int i = 0; i < 4; i++) {
            save_qtree_as_ppm_helper(root->children[i], pixels);
        }
    }
}

QTNode *load_preorder_qt(char *filename) {

    // open the file and initialize file handler (fp)
    FILE *fp = fopen(filename, "r");

    //QTNode *root = malloc(sizeof(QTNode));
    //root = load_preorder_qt_helper(fp);
    QTNode *root = load_preorder_qt_helper(fp, 0);
    printf("size: %ld\n", sizeof(root));
    fclose(fp);

    return root;
}

// performs recursion
QTNode *load_preorder_qt_helper(FILE *fp, int k) {

    // variables
    char type; // type of node
    unsigned int i, r, h, c, w;
    QTNode *root = malloc(sizeof(QTNode));

    if (fscanf(fp, "%c %u %u %u %u %u ", &type, &i, &r, &h, &c, &w) != 6) { // base case -> stores variables while checking if EOF
        printf("EOF or leaf?!");
        return NULL;
    }

    // initialize QTNode
    root->intensity = i;
    root->row = r;
    root->height = h;
    root->col = c;
    root->width = w;
    for (int j = 0 ; j < 4; j++) { // children are explicitly NULL because malloc may lead to unexplained behavior
       root->children[j] = NULL;
    }

    if (type == 'N') { // internal node -> continue recursion
        printf("%dN!\t", k);
        for (int j = 0; j < 4; j++) {
            root->children[j] = load_preorder_qt_helper(fp, ++k);
        }
    }

    return root;
}

void save_preorder_qt(QTNode *root, char *filename) {
    (void)root;
    (void)filename;
}

