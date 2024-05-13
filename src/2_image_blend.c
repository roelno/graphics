#include "ppmIO.h"
#include <stdio.h>
#include <stdlib.h>

#define USECPP 0


/* take in a foreground image, a background image, and a mask image
 * and blend the foreground and background images together using the mask
 * to create a new image */

int main(int argc, char *argv[]) {
  Pixel *foreground, *background, *output;
  Pixel *mask;
  int rows, cols, colors;
  long imagesize;
  long i;

  if (argc != 5) {
    printf("Usage: %s <foreground file> <background file> <mask file> <output "
           "file>\n",
           argv[0]);
    return -1;
  }

  /* read foreground image */
  foreground = readPPM(&rows, &cols, &colors, argv[1]);
  if (!foreground) {
    fprintf(stderr, "Unable to read %s\n", argv[1]);
    exit(-1);
  }

  /* read background image */
  background = readPPM(&rows, &cols, &colors, argv[2]);
  if (!background) {
    fprintf(stderr, "Unable to read %s\n", argv[2]);
    exit(-1);
  }

  /* read mask image */
  mask = (Pixel *)readPPM(&rows, &cols, &colors, argv[3]);
  if (!mask) {
    fprintf(stderr, "Unable to read %s\n", argv[3]);
    exit(-1);
  }

  /* allocate memory for the output image */
  output = (Pixel *)malloc(rows * cols * sizeof(Pixel));
  if (!output) {
    fprintf(stderr, "Unable to allocate memory for output image\n");
    exit(-1);
  }

  imagesize = (long)rows * (long)cols;

  /* blend the images together */
  for (i = 0; i < imagesize; i++) {
    float alpha = mask[i].g / 255.0;
    float beta = mask[i].r / 255.0;
    float gamma = mask[i].b / 255.0;
    // float alpha = 1;
    output[i].r = (unsigned char)(beta * foreground[i].r +
                                  (1 - beta) * background[i].r);
    output[i].g = (unsigned char)(alpha * foreground[i].g +
                                  (1 - alpha) * background[i].g);
    output[i].b = (unsigned char)(gamma * foreground[i].b +
                                  (1 - gamma) * background[i].b);
  }

  /* output the blended image */
  writePPM(output, rows, cols, 255, argv[4]);


  // Free memory
#if USECPP
  delete[] foreground;
  delete[] background;
  delete[] mask;
  delete[] output;
#else
  free(foreground);
  free(background);
  free(mask);
  free(output);
#endif

  return 0;
}
