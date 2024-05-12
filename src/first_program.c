#include "ppmIO.h"
#include <stdio.h>
#include <stdlib.h>

#define USECPP 0
#define COLOR_THRESHOLD 10

int main(int argc, char *argv[]) {
  Pixel *image;
  unsigned char *mask;
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
  mask = (unsigned char *)malloc(rows * cols);
  if (!mask) {
    fprintf(stderr, "Unable to allocate memory for mask\n");
    exit(-1);
  }

  /* calculate the image size */
  imagesize = (long)rows * (long)cols;

  /* Create the mask based on the blue or green threshold */
  for (i = 0; i < imagesize; i++) {
    float r, g, b , max_color, norm_factor;
    r = image[i].r;
    g = image[i].g;
    b = image[i].b;
    max_color = r > g ? r : g;
    max_color = max_color > b ? max_color : b;
    norm_factor = 255 / max_color;
    r *= norm_factor;
    g *= norm_factor;
    b *= norm_factor;

    if ((maskColor == 'b' && ((b - r) > COLOR_THRESHOLD) && ((b - g) > COLOR_THRESHOLD)) ||
        (maskColor == 'g' && ((g - r) > COLOR_THRESHOLD) && ((g - b) > COLOR_THRESHOLD))) {
      mask[i] = 0; // Background
    } else {
      mask[i] = 255; // Foreground
    }
  }

  /* Apply the mask to the image */
  for (i = 0; i < imagesize; i++) {
    image[i].r = image[i].r & mask[i];
    image[i].g = image[i].g & mask[i];
    image[i].b = image[i].b & mask[i];
  }

  /* write out the resulting image */
  writePPM(image, rows, cols, colors /* s/b 255 */, argv[2]);

  /* free the image memory */
#if USECPP
  delete[] image;
#else
  free(image);
  free(mask);
#endif

  return (0);
}
