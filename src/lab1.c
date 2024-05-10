/*
  This is an example of reading and writing an image using the ppmIO.c
  routines.  To manipulate the image, change the pixel color values.

  Bruce A. Maxwell updated 2021-09-05
*/

#include <stdio.h>
#include <stdlib.h>
#include "ppmIO.h"

#define USECPP 0

int main(int argc, char *argv[]) {
  Pixel *image;
  int rows, cols, colors;
  long imagesize;
  long i, j;
  int min;
  int redDecreaseFactor = 50;  // Amount to decrease red color
  int greenIncreaseFactor = 30; // Amount to increase green color

  if(argc < 3) {
    printf("Usage: ppmtest <input file> <output file>\n");
    exit(-1);
  }

  /* read in the image */
  image = readPPM(&rows, &cols, &colors, argv[1]);
  if(!image) {
    fprintf(stderr, "Unable to read %s\n", argv[1]);
    exit(-1);
  }

  /* calculate the image size */
  imagesize = (long)rows * (long)cols;

  /* mess with the image here  */
  /* Adjust colors in the image */
  for(i=0;i<imagesize;i++) {
    // Decrease redness in specific red range
    if(image[i].r > 100 && image[i].r < 200) {
      image[i].r = image[i].r - redDecreaseFactor > 0 ? image[i].r - redDecreaseFactor : 0;
    }

    // Increase greenness in specific green range
    if(image[i].g > 50 && image[i].g < 150) { // Assuming leaves are in this green range
      image[i].g = image[i].g + greenIncreaseFactor < 255 ? image[i].g + greenIncreaseFactor : 255;
    }
  }

  /* write out the resulting image */
  writePPM(image, rows, cols, colors /* s/b 255 */, argv[2]);

  /* free the image memory */
#if USECPP
  delete[] image;
#else
  free(image);
#endif

  return(0);
}
