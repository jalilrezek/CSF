#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tctest.h"
#include "imgproc.h"

// An expected color identified by a (non-zero) character code.
// Used in the "Picture" data type.
typedef struct {
  char c;
  uint32_t color;
} ExpectedColor;

// Type representing a "picture" of an expected image.
// Useful for creating a very simple Image to be accessed
// by test functions.
typedef struct {
  ExpectedColor colors[20];
  int width, height;
  const char *data;
} Picture;

// Some "basic" colors to use in test Pictures
#define TEST_COLORS \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0xFF0000FF }, \
      { 'g', 0x00FF00FF }, \
      { 'b', 0x0000FFFF }, \
      { 'c', 0x00FFFFFF }, \
      { 'm', 0xFF00FFFF }, \
    }

// Expected "basic" colors after grayscale transformation
#define TEST_COLORS_GRAYSCALE \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0x4E4E4EFF }, \
      { 'g', 0x7F7F7FFF }, \
      { 'b', 0x303030FF }, \
      { 'c', 0xB0B0B0FF }, \
      { 'm', 0x7F7F7FFF }, \
    }

// Colors for test overlay image (for testing the composite
// transformation). Has some fully-opaque colors,
// some partially-transparent colors, and a complete
// transparent color.
#define OVERLAY_COLORS \
  { \
    { 'r', 0xFF0000FF }, \
    { 'R', 0xFF000080 }, \
    { 'g', 0x00FF00FF }, \
    { 'G', 0x00FF0080 }, \
    { 'b', 0x0000FFFF }, \
    { 'B', 0x0000FF80 }, \
    { ' ', 0x00000000 }, \
  }

// Data type for the test fixture object.
// This contains data (including Image objects) that
// can be accessed by test functions. This is useful
// because multiple test functions can access the same
// data (so you don't need to create/initialize that
// data multiple times in different test functions.)
typedef struct {
  // smiley-face picture
  Picture smiley_pic;

  // original smiley-face Image object
  struct Image *smiley;

  // empty Image object to use for output of
  // transformation on smiley-face image
  struct Image *smiley_out;

  // Picture for overlay image (for basic imgproc_composite test)
  Picture overlay_pic;

  // overlay image object
  struct Image *overlay;
} TestObjs;

// Functions to create and clean up a test fixture object
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// Helper functions used by the test code
struct Image *picture_to_img( const Picture *pic );
uint32_t lookup_color(char c, const ExpectedColor *colors);
bool images_equal( struct Image *a, struct Image *b );
void destroy_img( struct Image *img );

// Test functions
void test_mirror_h_basic( TestObjs *objs );
void test_mirror_v_basic( TestObjs *objs );
void test_tile_basic( TestObjs *objs );
void test_grayscale_basic( TestObjs *objs );
void test_composite_basic( TestObjs *objs );
// TODO: add prototypes for additional test functions
void test_grayscale_single_color(TestObjs *objs);
void test_grayscale_multiple_colors(TestObjs *objs);
void test_composite_basic_opacity(TestObjs *objs);
void test_composite_completely_opaque(TestObjs *objs);
void test_composite_full_transparency(TestObjs *objs);
// void test_to2D(TestObjs *objs);
void test_mirror_h_2x2(TestObjs *objs);
void test_mirror_h_symmetrical(TestObjs *objs);
void test_mirror_h_with_single_column(TestObjs *objs);
void test_mirror_h_3x3(TestObjs *objs);
void test_mirror_h_4x4(TestObjs *objs);
void test_mirror_v_basic_2(TestObjs *objs);
void test_mirror_v_with_single_row(TestObjs *objs);
void test_mirror_v_4x4(TestObjs *objs);
void test_mirror_v_3x3(TestObjs *objs);

int main( int argc, char **argv ) {
  // allow the specific test to execute to be specified as the
  // first command line argument
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  // Run tests.
  // Make sure you add additional TEST() macro invocations
  // for any additional test functions you add.
  TEST( test_mirror_h_basic );
  TEST( test_mirror_v_basic );
  TEST( test_tile_basic );
  TEST( test_grayscale_basic );
  TEST( test_composite_basic );
  TEST(test_grayscale_single_color);
  TEST(test_grayscale_multiple_colors);
  TEST(test_composite_basic_opacity);
  TEST(test_composite_completely_opaque);
  TEST(test_composite_full_transparency);
  // TEST(test_to2D);
  TEST(test_mirror_h_2x2);
  TEST(test_mirror_h_symmetrical);
  TEST(test_mirror_h_with_single_column);
  TEST(test_mirror_h_3x3);
  TEST(test_mirror_h_4x4);
  TEST(test_mirror_v_basic_2);
  TEST(test_mirror_v_with_single_row);
  TEST(test_mirror_v_4x4);
  TEST(test_mirror_v_3x3);
  TEST_FINI();
}

////////////////////////////////////////////////////////////////////////
// Test fixture setup/cleanup functions
////////////////////////////////////////////////////////////////////////

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof(TestObjs) );

  Picture smiley_pic = {
    TEST_COLORS,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };
  objs->smiley_pic = smiley_pic;
  objs->smiley = picture_to_img( &smiley_pic );

  objs->smiley_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->smiley_out, objs->smiley->width, objs->smiley->height );

  Picture overlay_pic = {
    OVERLAY_COLORS,
    16, 10,
   "                "
   "                "
   "                "
   "                "
   "                "
   "  rRgGbB        "
   "                "
   "                "
   "                "
   "                "
  };
  objs->overlay_pic = overlay_pic;
  objs->overlay = picture_to_img( &overlay_pic );

  return objs;
}

void cleanup( TestObjs *objs ) {
  destroy_img( objs->smiley );
  destroy_img( objs->smiley_out );
  destroy_img( objs->overlay );

  free( objs );
}

////////////////////////////////////////////////////////////////////////
// Test code helper functions
////////////////////////////////////////////////////////////////////////

struct Image *picture_to_img( const Picture *pic ) {
  struct Image *img;

  img = (struct Image *) malloc( sizeof(struct Image) );
  img_init( img, pic->width, pic->height );

  for ( int i = 0; i < pic->height; ++i ) {
    for ( int j = 0; j < pic->width; ++j ) {
      int index = i * img->width + j;
      uint32_t color = lookup_color( pic->data[index], pic->colors );
      img->data[index] = color;
    }
  }

  return img;
}

uint32_t lookup_color(char c, const ExpectedColor *colors) {
  for (int i = 0; ; i++) {
    assert(colors[i].c != 0);
    if (colors[i].c == c) {
      return colors[i].color;
    }
  }
}

// Returns true IFF both Image objects are identical
bool images_equal( struct Image *a, struct Image *b ) {
  if ( a->width != b->width || a->height != b->height )
    return false;

  int num_pixels = a->width * a->height;
  for ( int i = 0; i < num_pixels; ++i ) {
    if ( a->data[i] != b->data[i] )
      return false;
  }

  return true;
}

void destroy_img( struct Image *img ) {
  if ( img != NULL )
    img_cleanup( img );
  free( img );
}

////////////////////////////////////////////////////////////////////////
// Test functions
////////////////////////////////////////////////////////////////////////

void test_mirror_h_basic( TestObjs *objs ) {
  Picture smiley_mirror_h_pic = {
    TEST_COLORS,
    16, 10,
    "    cbggrrrm    "
    "   b        c   "
    "  c   b  r   r  "
    " b            b "
    " r            b "
    " r   c    b   g "
    "  b   brgg   c  "
    "   c        m   "
    "    cmbrrggg    "
    "                "
  };
  struct Image *smiley_mirror_h_expected = picture_to_img( &smiley_mirror_h_pic );

  imgproc_mirror_h( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_mirror_h_expected, objs->smiley_out ) );

  destroy_img( smiley_mirror_h_expected );
}

void test_mirror_v_basic( TestObjs *objs ) {
  Picture smiley_mirror_v_pic = {
    TEST_COLORS,
    16, 10,
    "                "
    "    gggrrbmc    "
    "   m        c   "
    "  c   ggrb   b  "
    " g   b    c   r "
    " b            r "
    " b            b "
    "  r   r  b   c  "
    "   c        b   "
    "    mrrrggbc    "
  };
  struct Image *smiley_mirror_v_expected = picture_to_img( &smiley_mirror_v_pic );

  imgproc_mirror_v( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_mirror_v_expected, objs->smiley_out ) );

  destroy_img( smiley_mirror_v_expected );
}

void test_tile_basic( TestObjs *objs ) {
  Picture smiley_tile_3_pic = {
    TEST_COLORS,
    16, 10,
    "  rg    rg   rg "
    "                "
    "  gb    gb   gb "
    "                "
    "  rg    rg   rg "
    "                "
    "  gb    gb   gb "
    "  rg    rg   rg "
    "                "
    "  gb    gb   gb "
  };
  struct Image *smiley_tile_3_expected = picture_to_img( &smiley_tile_3_pic );

  int success = imgproc_tile( objs->smiley, 3, objs->smiley_out );
  ASSERT( success );
  ASSERT( images_equal( smiley_tile_3_expected, objs->smiley_out ) );

  destroy_img( smiley_tile_3_expected );
}

void test_grayscale_basic( TestObjs *objs ) {
  Picture smiley_grayscale_pic = {
    TEST_COLORS_GRAYSCALE,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };

  struct Image *smiley_grayscale_expected = picture_to_img( &smiley_grayscale_pic );

  imgproc_grayscale( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_grayscale_expected, objs->smiley_out ) );

  destroy_img( smiley_grayscale_expected );
}

void test_composite_basic( TestObjs *objs ) {
  imgproc_composite( objs->smiley, objs->overlay, objs->smiley_out );

  // for all of the fully-transparent pixels in the overlay image,
  // the result image should have a pixel identical to the corresponding
  // pixel in the base image
  for ( int i = 0; i < 160; ++i ) {
    if ( objs->overlay->data[i] == 0x00000000 )
      ASSERT( objs->smiley->data[i] == objs->smiley_out->data[i] );
  }

  // check the computed colors for the partially transparent or
  // fully opaque colors in the overlay image
  ASSERT( 0xFF0000FF == objs->smiley_out->data[82] );
  ASSERT( 0x800000FF == objs->smiley_out->data[83] );
  ASSERT( 0x00FF00FF == objs->smiley_out->data[84] );
  ASSERT( 0x00807FFF == objs->smiley_out->data[85] );
  ASSERT( 0x0000FFFF == objs->smiley_out->data[86] );
  ASSERT( 0x000080FF == objs->smiley_out->data[87] );
}

// additional test cases for grayscale
void test_grayscale_single_color(TestObjs *objs) {
    Picture red_pic = {
        TEST_COLORS,
        2, 2,
        "rr"
        "rr"
    };
    struct Image *red_img = picture_to_img(&red_pic);
    struct Image output_img;
    img_init(&output_img, red_img->width, red_img->height);
    imgproc_grayscale(red_img, &output_img);

    // grayscale value of red is 0x4E4E4EFF
    for (int i = 0; i < output_img.width * output_img.height; ++i) {
        ASSERT(output_img.data[i] == 0x4E4E4EFF);
    }

    destroy_img(red_img);
    img_cleanup(&output_img);
}

void test_grayscale_multiple_colors(TestObjs *objs) {
    Picture rgb_pic = {
        TEST_COLORS,
        2, 2,
        "rg"
        "bc"
    };
    struct Image *color_img = picture_to_img(&rgb_pic);
    struct Image output_img;
    img_init(&output_img, color_img->width, color_img->height);
    imgproc_grayscale(color_img, &output_img);

    uint32_t expected[] = {
        0x4E4E4EFF,  // red
        0x7F7F7FFF,  // green
        0x303030FF,  // blue
        0xB0B0B0FF   // white
    };

    for (int i = 0; i < 4; ++i) {
        ASSERT(output_img.data[i] == expected[i]);
    }

    destroy_img(color_img);
    img_cleanup(&output_img);
}

// additional tests for composite 
void test_composite_basic_opacity(TestObjs *objs) {
    // all red base image and overlay image with varying opacity values
    Picture pic = {
        TEST_COLORS,
        2, 2,
        "rr"
        "rr"
    };
    struct Image *base_img = picture_to_img(&pic);
    Picture overlay_pic = {
        OVERLAY_COLORS,
        2, 2,
        "R "  // Semi-transparent red and fully transparent pixel
        " g"  // Fully opaque green and fully transparent pixel
    };
    struct Image *overlay_img = picture_to_img(&overlay_pic);
    struct Image output_img;
    img_init(&output_img, base_img->width, base_img->height);
    imgproc_composite(base_img, overlay_img, &output_img);

    uint32_t expected[] = {
        0xFF0000FF,  // Fully opaque red
        0xFF0000FF,  // Semi-transparent red blended with red
        0xFF0000FF,  // Fully opaque green
        0xFF0000FF   // Transparent pixel does not modify the base image
    };

    for (int i = 0; i < 4; ++i) {
        if (output_img.data[i] != expected[i]) {
            printf("Pixel %d mismatch: got 0x%X, expected 0x%X\n", i, output_img.data[i], expected[i]);
        }
        ASSERT(output_img.data[i] == expected[i]);
    }

    destroy_img(base_img);
    destroy_img(overlay_img);
    img_cleanup(&output_img);
}

void test_composite_full_transparency(TestObjs *objs) {
    // fully transparent overlay
    Picture pic = {
        TEST_COLORS,
        2, 2,
        "rr"
        "rr"
    };
    struct Image *base_img = picture_to_img(&pic);

    Picture overlay_pic = {
        OVERLAY_COLORS,
        2, 2,
        "  "
        "  "
    };
    struct Image *overlay_img = picture_to_img(&overlay_pic);
    struct Image output_img;
    img_init(&output_img, base_img->width, base_img->height);
    imgproc_composite(base_img, overlay_img, &output_img);

    for (int i = 0; i < 4; ++i) {
        ASSERT(output_img.data[i] == base_img->data[i]);
    }

    destroy_img(base_img);
    destroy_img(overlay_img);
    img_cleanup(&output_img);
}

void test_composite_completely_opaque(TestObjs *objs) {
    // fully opaque overlay
    Picture pic = {
        TEST_COLORS,
        2, 2,
        "rr"
        "rr"
    };
    struct Image *base_img = picture_to_img(&pic);

    Picture overlay_pic = {
        OVERLAY_COLORS,
        2, 2,
        "gg"
        "gg"
    };
    struct Image *overlay_img = picture_to_img(&overlay_pic);
    struct Image output_img;
    img_init(&output_img, base_img->width, base_img->height);
    imgproc_composite(base_img, overlay_img, &output_img);

    for (int i = 0; i < 4; ++i) {
        ASSERT(output_img.data[i] == overlay_img->data[i]);
    }

    destroy_img(base_img);
    destroy_img(overlay_img);
    img_cleanup(&output_img);
}

/*
//Function prototype for to2D (needs to be fixed)

void test_to2D(TestObjs * objs) {
    int width = 3;
    int height = 3;

    // Input 1D data for testing
    uint32_t inputData[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    // Expected 2D data
    uint32_t expectedData[3][3] = {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 }
    };

    // Call the function to test
    uint32_t** outputData = to2D(inputData, width, height);

    // Assert that each element in the output matches the expected data
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            assert(outputData[row][col] == expectedData[row][col]);
        }
    }

    // Free allocated memory after the test
    for (int row = 0; row < height; row++) {
        free(outputData[row]);
    }
    free(outputData);

    // Print a success message if all assertions pass
    printf("test_to2D passed.\n");
}
*/

// my tests for mirror horizontal function 
void test_mirror_h_2x2(TestObjs *objs) {
    // 2x2 image
    Picture pic = {
        TEST_COLORS,
        2, 2,
        "rg"
        "bc"
    };
    struct Image *input_img = picture_to_img(&pic);
    struct Image output_img;
    img_init(&output_img, input_img->width, input_img->height);
    imgproc_mirror_h(input_img, &output_img);

    // should be horizontally mirrored 
    Picture expected = {
        TEST_COLORS,
        2, 2,
        "gr"
        "cb"
    };
    struct Image *expected_img = picture_to_img(&expected);

    ASSERT(images_equal(&output_img, expected_img));

    destroy_img(input_img);
    destroy_img(expected_img);
    img_cleanup(&output_img);
}

void test_mirror_h_symmetrical(TestObjs *objs) {
    // horizontally symmetric image
    Picture pic = {
        TEST_COLORS,
        2, 2,
        "rr"
        "gg"
    };
    struct Image *input_img = picture_to_img(&pic);
    struct Image output_img;
    img_init(&output_img, input_img->width, input_img->height);
    imgproc_mirror_h(input_img, &output_img);

    // Expect the image to remain unchanged
    ASSERT(images_equal(input_img, &output_img));

    destroy_img(input_img);
    img_cleanup(&output_img);
}

void test_mirror_h_with_single_column(TestObjs *objs) {
    // with single column, horizontal mirror shouldn't do anything  
    Picture pic = {
        TEST_COLORS,
        1, 2,
        "r"
        "g"
    };
    struct Image *input_img = picture_to_img(&pic);
    struct Image output_img;
    img_init(&output_img, input_img->width, input_img->height);
    imgproc_mirror_h(input_img, &output_img);

    // should remain unchanged
    ASSERT(images_equal(input_img, &output_img));

    destroy_img(input_img);
    img_cleanup(&output_img);
}

void test_mirror_h_3x3(TestObjs *objs) {
    // odd r x c (3x3)
    Picture pic = {
        TEST_COLORS,
        3, 3,
        "rgb"
        "bgr"
        "grb"
    };
    struct Image *input_img = picture_to_img(&pic);
    struct Image output_img;
    img_init(&output_img, input_img->width, input_img->height);
    imgproc_mirror_h(input_img, &output_img);

    // Expected horizontally mirrored image
    Picture expected = {
        TEST_COLORS,
        3, 3,
        "bgr"
        "rgb"
        "brg"
    };
    struct Image *expected_img = picture_to_img(&expected);
    ASSERT(images_equal(&output_img, expected_img));

    destroy_img(input_img);
    destroy_img(expected_img);
    img_cleanup(&output_img);
}

void test_mirror_h_4x4(TestObjs *objs) {
    // 4x4 image
    Picture pic = {
        TEST_COLORS,
        4, 4,
        "rgbr"
        "gbrg"
        "brgb"
        "rgrb"
    };
    struct Image *input_img = picture_to_img(&pic);
    struct Image output_img;
    img_init(&output_img, input_img->width, input_img->height);
    imgproc_mirror_h(input_img, &output_img);

    // should expect this 
    Picture expected = {
        TEST_COLORS,
        4, 4,
        "rbgr"
        "grgb"
        "bgrb"
        "brgr"
    };
    struct Image *expected_img = picture_to_img(&expected);

    for (int i = 0; i < output_img.width * output_img.height; ++i) {
        if (output_img.data[i] != expected_img->data[i]) {
            printf("Mismatch at pixel %d: got 0x%X, expected 0x%X\n", i, output_img.data[i], expected_img->data[i]);
        }
        ASSERT(output_img.data[i] == expected_img->data[i]);
    }
  
    // ASSERT(images_equal(&output_img, expected_img));

    destroy_img(input_img);
    destroy_img(expected_img);
    img_cleanup(&output_img);
}

// my tests for mirror vertical function 
void test_mirror_v_basic_2(TestObjs *objs) {
    // 2x2 image
    Picture pic = {
        TEST_COLORS,
        2, 2,
        "rg"
        "bc"
    };
    struct Image *input_img = picture_to_img(&pic);
    struct Image output_img;
    img_init(&output_img, input_img->width, input_img->height);
    imgproc_mirror_v(input_img, &output_img);

    // should expect this 
    Picture expected = {
        TEST_COLORS,
        2, 2,
        "bc"
        "rg"
    };
    struct Image *expected_img = picture_to_img(&expected);
    ASSERT(images_equal(&output_img, expected_img));
  
    destroy_img(input_img);
    destroy_img(expected_img);
    img_cleanup(&output_img);
}

void test_mirror_v_with_single_row(TestObjs *objs) {
    // should not change 
    Picture pic = {
        TEST_COLORS,
        2, 1,
        "rg"
    };
    struct Image *input_img = picture_to_img(&pic);
    struct Image output_img;
    img_init(&output_img, input_img->width, input_img->height);
    imgproc_mirror_v(input_img, &output_img);

    ASSERT(images_equal(input_img, &output_img));

    destroy_img(input_img);
    img_cleanup(&output_img);
}

void test_mirror_v_4x4(TestObjs *objs) {
    // 4x4 
    Picture pic = {
        TEST_COLORS,
        4, 4,
        "rgbr"
        "gbrg"
        "brgb"
        "rgrb"
    };
    struct Image *input_img = picture_to_img(&pic);
    struct Image output_img;
    img_init(&output_img, input_img->width, input_img->height);
    imgproc_mirror_v(input_img, &output_img);

    // expect this
    Picture expected = {
        TEST_COLORS,
        4, 4,
        "rgrb"
        "brgb"
        "gbrg"
        "rgbr"
    };
    struct Image *expected_img = picture_to_img(&expected);

    ASSERT(images_equal(&output_img, expected_img));

    destroy_img(input_img);
    destroy_img(expected_img);
    img_cleanup(&output_img);
}

void test_mirror_v_3x3(TestObjs *objs) {
    // 3x3
    Picture pic = {
        TEST_COLORS,
        3, 3,
        "rgb"
        "bgr"
        "grb"
    };
    struct Image *input_img = picture_to_img(&pic);
    struct Image output_img;
    img_init(&output_img, input_img->width, input_img->height);
    imgproc_mirror_v(input_img, &output_img);

    // expect this
    Picture expected = {
        TEST_COLORS,
        3, 3,
        "grb"
        "bgr"
        "rgb"
    };
    struct Image *expected_img = picture_to_img(&expected);

    ASSERT(images_equal(&output_img, expected_img));

    destroy_img(input_img);
    destroy_img(expected_img);
    img_cleanup(&output_img);
}
