#include "ppmIO.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define USECPP 0

/* scale an image using nearest neighbor interpolation */
static Pixel *scaleImage(Pixel *input, int oldRows, int oldCols,
                         float scaleFactor, int *newRows, int *newCols);

/* rotate an image by 90 degrees clockwise */
static Pixel *rotateImage90(Pixel *input, int oldRows, int oldCols,
                            int *newRows, int *newCols);


Pixel *scaleImage(Pixel *input, int oldRows, int oldCols, float scaleFactor,
                  int *newRows, int *newCols) {
  *newRows = (int)(oldRows * scaleFactor);
  *newCols = (int)(oldCols * scaleFactor);
  Pixel *output = malloc(*newRows * *newCols * sizeof(Pixel));
  if (!output) {
    fprintf(stderr, "Unable to allocate memory for scaled image\n");
    exit(-1);
  }

  for (int y = 0; y < *newRows; ++y) {
    for (int x = 0; x < *newCols; ++x) {
      int oldX = (int)(x / scaleFactor);
      int oldY = (int)(y / scaleFactor);
      output[y * (*newCols) + x] = input[oldY * oldCols + oldX];
    }
  }

  return output;
}

Pixel *rotateImage90(Pixel *input, int oldRows, int oldCols, int *newRows,
                     int *newCols) {
  *newRows = oldCols;
  *newCols = oldRows;
  Pixel *output = malloc(*newRows * *newCols * sizeof(Pixel));
  if (!output) {
    fprintf(stderr, "Unable to allocate memory for rotated image\n");
    exit(-1);
  }

  for (int y = 0; y < oldRows; ++y) {
    for (int x = 0; x < oldCols; ++x) {
      output[x * (*newCols) + (oldRows - y - 1)] = input[y * oldCols + x];
    }
  }

  return output;
}

int main(int argc, char *argv[]) {
  Pixel *foreground, *background, *output;
  Pixel *mask, *scaledForeground, *scaledMask;
  int fgRows, fgCols, bgRows, bgCols, maskRows, maskCols;
  int scaledFgRows, scaledFgCols;
  int colors;
  long i, j;
  int dx, dy;
  float scaleFactor;
  int rotate;

  if (argc != 9) {
    printf("Usage: %s <foreground file> <background file> <mask file> <dx> "
           "<dy> <scaleFactor> <rotate (0 or 1)> <output file>\n",
           argv[0]);
    return -1;
  }

  dx = atoi(argv[4]);
  dy = atoi(argv[5]);
  scaleFactor = atof(argv[6]);
  rotate = atoi(argv[7]);

  /* read foreground image */
  foreground = readPPM(&fgRows, &fgCols, &colors, argv[1]);
  if (!foreground) {
    fprintf(stderr, "Unable to read %s\n", argv[1]);
    exit(-1);
  }

  /* read background image */
  background = readPPM(&bgRows, &bgCols, &colors, argv[2]);
  if (!background) {
    fprintf(stderr, "Unable to read %s\n", argv[2]);
    exit(-1);
  }

  /* read mask image */
  mask = readPPM(&maskRows, &maskCols, &colors, argv[3]);
  if (!mask) {
    fprintf(stderr, "Unable to read %s\n", argv[3]);
    exit(-1);
  }

  /* rotate foreground and mask images if needed */
  if (rotate) {
    Pixel *rotatedForeground = rotateImage90(foreground, fgRows, fgCols, &fgRows, &fgCols);
    free(foreground);
    foreground = rotatedForeground;
    Pixel *rotatedMask = rotateImage90(mask, maskRows, maskCols, &maskRows, &maskCols);
    free(mask);
    mask = rotatedMask;
  }

  /* scale foreground and mask images */
  scaledForeground = scaleImage(foreground, fgRows, fgCols, scaleFactor,
                                &scaledFgRows, &scaledFgCols);
  scaledMask = scaleImage(mask, maskRows, maskCols, scaleFactor, &scaledFgRows,
                          &scaledFgCols);

  fprintf(stdout, "Scaled maskRows: %d, Scaled maskCols: %d\n", scaledFgRows,
          scaledFgCols);
  fprintf(stdout, "bgRows: %d, bgCols: %d\n", bgRows, bgCols);

  /* ensure scaled dimensions and offsets are compatible */
  if (dx < 0 || dy < 0 || dx + scaledFgCols > bgCols ||
      dy + scaledFgRows > bgRows) {
    fprintf(stderr, "Invalid offsets or dimensions too large for background\n");
    exit(-1);
  }

  /* allocate memory for the output image */
  output = malloc(bgRows * bgCols * sizeof(Pixel));
  if (!output) {
    fprintf(stderr, "Unable to allocate memory for output image\n");
    exit(-1);
  }

  /* Copy background to output initially */
  for (i = 0; i < bgRows * bgCols; i++) {
    output[i] = background[i];
  }

  /* blend the scaled images together at the offsets */
  for (j = 0; j < scaledFgRows; j++) {
    for (i = 0; i < scaledFgCols; i++) {
      long indexFG = j * scaledFgCols + i;
      long indexBG = (j + dy) * bgCols + (i + dx);
      float alpha = scaledMask[indexFG].g / 255.0;
      float beta = scaledMask[indexFG].r / 255.0;
      float gamma = scaledMask[indexFG].b / 255.0;

      output[indexBG].r = (unsigned char)(beta * scaledForeground[indexFG].r +
                                          (1 - beta) * output[indexBG].r);
      output[indexBG].g = (unsigned char)(alpha * scaledForeground[indexFG].g +
                                          (1 - alpha) * output[indexBG].g);
      output[indexBG].b = (unsigned char)(gamma * scaledForeground[indexFG].b +
                                          (1 - gamma) * output[indexBG].b);
    }
  }

  /* output the blended image */
  writePPM(output, bgRows, bgCols, colors, argv[8]);

#if USECPP
  delete[] foreground;
  delete[] background;
  delete[] mask;
  delete[] output;
  delete[] scaledForeground;
  delete[] scaledMask;
#else
  free(foreground);
  free(background);
  free(mask);
  free(output);
  free(scaledForeground);
  free(scaledMask);
#endif

  return 0;
}
