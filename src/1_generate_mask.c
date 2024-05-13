#include "ppmIO.h"
#include <stdio.h>
#include <stdlib.h>

#define USECPP 0
#define COLOR_THRESHOLD 30 // for in-between values of blue and green

int main(int argc, char *argv[]) {
  Pixel *image;
  Pixel *mask;
  int rows, cols, colors;
  long imagesize;
  long i;

  if (argc != 4) {
    printf("Usage: %s <input file> <output file> <mask color (b/g)>\n",
           argv[0]);
    return -1;
  }

  char maskColor = argv[3][0]; // 'b' for blue, 'g' for green

  /* read in the image */
  image = readPPM(&rows, &cols, &colors, argv[1]);
  if (!image) {
    fprintf(stderr, "Unable to read %s\n", argv[1]);
    exit(-1);
  }

  /* Allocate memory for the mask */
  mask = (Pixel *)malloc(rows * cols * sizeof(Pixel));
  if (!mask) {
    fprintf(stderr, "Unable to allocate memory for mask\n");
    exit(-1);
  }

  /* calculate the image size */
  imagesize = (long)rows * (long)cols;

  /* create the mask based on the blue or green threshold */
  for (i = 0; i < imagesize; i++) {
    float r, g, b;
    r = image[i].r;
    g = image[i].g;
    b = image[i].b;


    if ((maskColor == 'b' && ((b - r) > COLOR_THRESHOLD) && ((b - g) > COLOR_THRESHOLD)) ||
        (maskColor == 'g' && ((g - r) > COLOR_THRESHOLD) && ((g - b) > COLOR_THRESHOLD))) {
      mask[i].r = 0; // Background
      mask[i].g = 0;
      mask[i].b = 0;
    } else {
      mask[i].r = 255; // Foreground
      mask[i].g = 255;
      mask[i].b = 255;
    }
  }

  /* Output the mask */
  writePPM(mask, rows, cols, colors, argv[2]);


  /* free the image memory */
#if USECPP
  delete[] image;
#else
  free(image);
  free(mask);
#endif

  return (0);
}
