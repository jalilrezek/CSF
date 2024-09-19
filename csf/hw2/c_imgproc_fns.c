// C implementations of image processing functions

#include <stdlib.h>
#include <assert.h>
#include "imgproc.h"

// to free: twoDim from to2D() ; oneDim from backTo1D; twoDimInData from mirror_h;

// TODO: define your helper functions here
uint32_t** to2D(uint32_t* data, int32_t width, int32_t height) {
  uint32_t** twoDim = (uint32_t**) malloc(sizeof(uint32_t*) * height);

  for (int row = 0; row < height; row++) {
    twoDim[row] = (uint32_t*) malloc(sizeof(uint32_t) * width);
  }

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      twoDim[row][col] = data[row * width + col];
    }
  }
  return twoDim;

  // TO FREE: twoDim
}

uint32_t* backTo1D(uint32_t* oneDimResult, uint32_t** twoDim, int32_t width, int32_t height) {

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      oneDimResult[row * width + col] = twoDim[row][col];
    }
  }
  return oneDimResult; // must free oneDim?
}

// Mirror input image horizontally.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_mirror_h( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  // for each row: original[i] = flipped[length - i]
  int32_t width = input_img->width;
  int32_t height = input_img->height;

  uint32_t** twoDimInData = to2D(input_img->data, width, height); // must be freed later?
  uint32_t** twoDimOutData = to2D(output_img->data, width, height); // must be freed later?

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      twoDimOutData[row][width - col - 1] = twoDimInData[row][col];
    }
  }
  output_img->data = backTo1D(output_img->data, twoDimOutData, width, height); // must be freed


  // clean up
  for (int i = 0; i < height; i++) { // free twoDimInData and twoDimOutData
    free(twoDimInData[i]);
    free(twoDimOutData[i]);
  }
  free(twoDimInData);
  free(twoDimOutData);

  
}

// Mirror input image vertically.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_mirror_v( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement

    int32_t width = input_img->width;
  int32_t height = input_img->height;

  uint32_t** twoDimInData = to2D(input_img->data, width, height); // must be freed later?
  uint32_t** twoDimOutData = to2D(output_img->data, width, height); // must be freed later?

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      twoDimOutData[height - y - 1][x] = twoDimInData[y][x];
    }
  }
  output_img->data = backTo1D(output_img->data, twoDimOutData, width, height); // must be freed


  // clean up
  for (int i = 0; i < height; i++) { // free twoDimInData and twoDimOutData
    free(twoDimInData[i]);
    free(twoDimOutData[i]);
  }
  free(twoDimInData);
  free(twoDimOutData);
}

// Transform image by generating a grid of n x n smaller tiles created by
// sampling every n'th pixel from the original image.
//
// Parameters:
//   input_img  - pointer to original struct Image
//   n          - tiling factor (how many rows and columns of tiles to generate)
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
// Returns:
//   1 if successful, or 0 if either
//     - n is less than 1, or
//     - the output can't be generated because at least one tile would
//       be empty (i.e., have 0 width or height)
int imgproc_tile( struct Image *input_img, int n, struct Image *output_img ) {
  // TODO: implement
  return 0;
}

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
}

// Overlay a foreground image on a background image, using each foreground
// pixel's alpha value to determine its degree of opacity in order to blend
// it with the corresponding background pixel.
//
// Parameters:
//   base_img - pointer to base (background) image
//   overlay_img - pointer to overlaid (foreground) image
//   output_img - pointer to output Image
//
// Returns:
//   1 if successful, or 0 if the transformation fails because the base
//   and overlay image do not have the same dimensions
int imgproc_composite( struct Image *base_img, struct Image *overlay_img, struct Image *output_img ) {
  // TODO: implement
  return 0;
}
