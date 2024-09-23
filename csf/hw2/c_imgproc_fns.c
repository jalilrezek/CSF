// C implementations of image processing functions

#include <stdlib.h>
#include <assert.h>
#include "imgproc.h"

// to free: twoDim from to2D() ; oneDim from backTo1D; twoDimInData from mirror_h; subPic; outData (tile); 
// heights (tile); widths (tile)

// TODO: define your helper functions here

void free2DArray(uint32_t** data, int32_t height) {
  for (int i = 0; i < height; i++) {
    free(data[i]);
  }
  free(data);
}

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

// paste 1D version of 2D result into output_img->data
void backTo1D(struct Image* output_img, uint32_t** twoDim, int32_t width, int32_t height) {

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      output_img->data[row * width + col] = twoDim[row][col];
    }
  }
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
  backTo1D(output_img, twoDimOutData, width, height); // paste 1D version of 2D result into output_img->data


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
  backTo1D(output_img, twoDimOutData, width, height); // must be freed


  // clean up
  for (int i = 0; i < height; i++) { // free twoDimInData and twoDimOutData
    free(twoDimInData[i]);
    free(twoDimOutData[i]);
  }
  free(twoDimInData);
  free(twoDimOutData);
}

uint32_t** makeSubpic(uint32_t** twoDimInput, int n, int32_t subHeight, int32_t subWidth) {


  // allocate space for subpic
  uint32_t** subPic = (uint32_t**) malloc(sizeof(uint32_t*) * subHeight); // FREE later
  for (int row = 0; row < subHeight; row++) {
    subPic[row] = (uint32_t*) malloc(sizeof(uint32_t) * subWidth); // FREE later
  }

  for (int r = 0; r < subHeight; r++) {
    for (int c = 0; c < subWidth;  c++) {
      subPic[r][c] = twoDimInput[r * n][c * n];
    }
  }

  return subPic;


}

void pasteImage(uint32_t** res, uint32_t** twoDimInput, uint32_t** subData, int subWidth, int subHeight, int curRow, int curCol) {
  for (int r = 0; r < subHeight; r++) {
    for (int c = 0; c < subWidth;  c++) {
      res[r + curRow][c + curCol] = subData[r][c];
    }
  }
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
  
  // In what case does at least one tile have 0 width or height? Is it just when n exceeds either width or height?
  // if n is less than width, width can be split up into n subsections at least 1 px wide
  // if n is less than height, height can be split up into n subsections at least 1 px wide
  // if n is less than both H and W, the picture can be split up into n^2 subpictures each w both width and height >= 1

  if (n < 1 || n > input_img->width || n > input_img->height) {
    return 0; // should return 0 for failure. But returns 0 at the bottom. Should return 1 at the bottom I think.
  }

  int32_t width = input_img->width;
  int32_t height = input_img->height;

  int32_t numWithWidthRemainder = width % n;
  int32_t numWithHeightRemainder = height % n;

  int32_t subpicWidth = width / n;
  int32_t subpicHeight = height / n;

  uint32_t** twoDimInData = to2D(input_img->data, width, height); // FREE later

  // construct arrays to store heights and widths. 
  int32_t* widths = (int32_t*) malloc(sizeof(int32_t) * n); // widths[col] = width of all subpics in that col
  int32_t* heights = (int32_t*) malloc(sizeof(int32_t) * n); // heights[row] = height of all subpics in that row

  for (int i = 0; i < numWithWidthRemainder; i++) { // never entered if none with remainder
    widths[i] = subpicWidth + 1;
  }
  for (int i = numWithWidthRemainder; i < n; i++) {
    widths[i] = subpicWidth;
  }

  for (int i = 0; i < numWithHeightRemainder; i++) { // never entered if no remainder
    heights[i] = subpicHeight + 1;
  }
  for (int i = numWithHeightRemainder; i < n; i++) {
    heights[i] = subpicHeight;
  }

  // make the tiled pic

  uint32_t** outData = to2D(output_img->data, width, height); // FREE later
  int32_t curRow = 0;
  int curCol = 0;

  for (int row = 0; row < n; row++) {
    int32_t curHeight = heights[row];
    for (int col = 0; col < n; col++) {
      int32_t curWidth = widths[col];
      uint32_t** subPic = makeSubpic( twoDimInData, n, curHeight, curWidth);
      // FREE above later!

      pasteImage(outData, twoDimInData, subPic, curWidth, curHeight, curRow, curCol);
      curCol += curWidth;
      free2DArray(subPic, curHeight); // FREED subPic
    }
    curRow += curHeight;
    curCol = 0;
  }


  // paste 1D version of 2D result into output image and we're done
  backTo1D(output_img, outData, width, height);

  // cleanup
  free2DArray(twoDimInData, height); // FREED twoDimInData
  free2DArray(outData, height); // FREED outData
  free(heights);
  free(widths);
 
  return 1; // used to be return 0 at the end, for failure I guess. Now return 1.
}

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
  // set output image dimensions to match the input image
  output_img->width = input_img->width;
  output_img->height = input_img->height;

  // allocate memory for the output (1 uint32_t per 1 pixel)
  output_img->data = malloc(input_img->width * input_img->height * sizeof(uint32_t));

  // for loop that goes through all pixels in input image
  for (int i = 0; i < input_img->width * input_img->height; i++) {
    // get the colors for pixel (r, b, g, alpha) 
    uint32_t pixel = input_img->data[i];
    uint32_t red = (pixel >> 24) & 0xFF; // bits 24-31 for red
    uint32_t green = (pixel >> 16) & 0xFF; // bits 16-23 for green
    uint32_t blue = (pixel >> 8) & 0xFF;  // bits 8-15 for blue 
    uint32_t alpha = pixel & 0xFF;     // bits 0-7 for alpha

    // get grayscale color using equation 
    uint32_t gray = (79 * red + 128 * green + 49 * blue) / 256;

    // create new pixel by setting rbg values to gray (alpha doesn't change)
    output_img->data[i] = (gray << 24) | (gray << 16) | (gray << 8) | alpha;
  }
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
  // make sure that the dimensions match for the base and overlay image 
  if (base_img->width != overlay_img->width || base_img->height != overlay_img->height) {
      return 0;  // fail if they do not match
  }
  
  // set output image dimensions to match base and overlay images
  output_img->width = base_img->width;
  output_img->height = base_img->height;

  // allocate memory for the output (1 uint32_t per 1 pixel)
  output_img->data = malloc(base_img->width * base_img->height * sizeof(uint32_t));

  // for loop that goes through all pixels in base and overlay images
  for (int i = 0; i < base_img->width * base_img->height; i++) {
      // get the colors for overlay pixel (r, b, g, alpha) 
      uint32_t overlay_pixel = overlay_img->data[i];
      uint32_t o_red = (overlay_pixel >> 24) & 0xFF;  // bits 24-31 for red
      uint32_t o_green = (overlay_pixel >> 16) & 0xFF;  // bits 16-23 for green
      uint32_t o_blue = (overlay_pixel >> 8) & 0xFF;   // bits 8-15 for blue
      uint32_t o_alpha = overlay_pixel & 0xFF;          // bits 0-7 for opacity (alpha) 

      // get the colors for base pixel (r, b, g)
      uint32_t base_pixel = base_img->data[i];
      uint32_t b_red = (base_pixel >> 24) & 0xFF;  // bits 24-31 for red
      uint32_t b_green = (base_pixel >> 16) & 0xFF;  // bits 16-23 for green
      uint32_t b_blue = (base_pixel >> 8) & 0xFF;   // bits 8-15 for blue

      // blend the colors using alpha from the overlay pixel
      uint32_t r = (o_alpha * o_red + (255 - o_alpha) * b_red) / 255;
      uint32_t g = (o_alpha * o_green + (255 - o_alpha) * b_green) / 255;
      uint32_t b = (o_alpha * o_blue + (255 - o_alpha) * b_blue) / 255;

      // create the final pixel - blend r, b, g and set alpha to 255 for fully opaque
      output_img->data[i] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
  }
  
  return 1;
}
