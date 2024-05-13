#include "ppmIO.h"
#include <stdio.h>
#include <stdlib.h>

#define USECPP 0

/* Take in a foreground image, a background image, a mask image, and place the
 * foreground image onto the background with specified offsets dx and dy using
 * the mask to blend the images. */

/* Compile with ../bin/3_image_blend_offset powerpuff.ppm background_large.ppm
 * mask_powerpuff.ppm 300 0 blend_result_offset_powerpuff.ppm */

int main(int argc, char *argv[]) {
  Pixel *foreground, *background, *output;
  Pixel *mask;
  int fgRows, fgCols, bgRows, bgCols, maskRows, maskCols;
  int colors;
  long i, j;
  int dx, dy;

  if (argc != 7) {
    printf("Usage: %s <foreground file> <background file> <mask file> <dx> "
           "<dy> <output file>\n",
           argv[0]);
    return -1;
  }

  dx = atoi(argv[4]);
  dy = atoi(argv[5]);

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

  fprintf(stdout, "maskRows: %d, maskCols: %d\n", maskRows, maskCols);
  fprintf(stdout, "fgRows: %d, fgCols: %d\n", fgRows, fgCols);
  fprintf(stdout, "bgRows: %d, bgCols: %d\n", bgRows, bgCols);

  /* ensure foreground, mask, and background dimensions and offsets are
   * compatible */
  if (fgRows != maskRows || fgCols != maskCols || dx < 0 || dy < 0 ||
      dx + fgCols > bgCols || dy + fgRows > bgRows) {
    fprintf(stderr, "Dimension mismatch or invalid offsets\n");
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

  /* blend the images together at the offsets */
  for (j = 0; j < fgRows; j++) {
    for (i = 0; i < fgCols; i++) {
      long indexFG = j * fgCols + i;
      long indexBG = (j + dy) * bgCols + (i + dx);
      float alpha = mask[indexFG].g / 255.0;
      float beta = mask[indexFG].r / 255.0;
      float gamma = mask[indexFG].b / 255.0;

      output[indexBG].r = (unsigned char)(beta * foreground[indexFG].r +
                                          (1 - beta) * output[indexBG].r);
      output[indexBG].g = (unsigned char)(alpha * foreground[indexFG].g +
                                          (1 - alpha) * output[indexBG].g);
      output[indexBG].b = (unsigned char)(gamma * foreground[indexFG].b +
                                          (1 - gamma) * output[indexBG].b);
    }
  }

  /* output the blended image */
  writePPM(output, bgRows, bgCols, colors, argv[6]);

  /* Free memory */
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
