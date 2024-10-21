#include "qtree.h"
#include <math.h> // to perform calculations

// function prototypes because i don't like writing helper functions before the actual function
QTNode *create_quadtree_helper(Image *image, unsigned int r, unsigned int c, unsigned int h, unsigned int w, double max_rmse);

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
    if (RMSE <= max_rmse) { // base case -> leaf node
        //*root = (QTNode){image->intensity, r, c, image->height, image->width, NULL}; // technically nulls are not necessary b/c even if you don't initialize they'll be set to 0 and NULl anyways};
        root->intensity = (unsigned char)average;
        root->row = r;
        root->col = c;
        root->height = h;
        root->width = w;
        for (int i = 0; i < 4; i++) {
            root->children[i] = NULL;
        }
    } else if (RMSE > max_rmse) { // error is greater than necessary -> bad -> split the node -> 4 children 
        // variables just to make life easier
        unsigned int half_h = h / 2;
        unsigned int half_w = w / 2;
        
        (root->children[0]) = create_quadtree_helper(image, r, c, half_h, half_w, max_rmse);
        (root->children[1]) = create_quadtree_helper(image, r, c + half_w, half_h, half_w, max_rmse);
        (root->children[2]) = create_quadtree_helper(image, r + half_h, c, half_h, half_w, max_rmse);
        (root->children[3]) = create_quadtree_helper(image, r + half_h, c + half_w, half_h, half_w, max_rmse);
    }
    return root;
}

QTNode *get_child1(QTNode *node) {
    (void)node;
    return node->children[0];
}

QTNode *get_child2(QTNode *node) {
    (void)node;
    return node->children[1];
}

QTNode *get_child3(QTNode *node) {
    (void)node;
    return node->children[2];
}

QTNode *get_child4(QTNode *node) {
    (void)node;
    return node->children[3];
}

unsigned char get_node_intensity(QTNode *node) {
    (void)node;
    return node->intensity;
}

void delete_quadtree(QTNode *root) {
    (void)root;
}

void save_qtree_as_ppm(QTNode *root, char *filename) {
    (void)root;
    (void)filename;
}

QTNode *load_preorder_qt(char *filename) {
    (void)filename;
    return NULL;
}

void save_preorder_qt(QTNode *root, char *filename) {
    (void)root;
    (void)filename;
}

