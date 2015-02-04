#include <pebble.h>
#include "images.h"

static
GBitmap* digits_segments[4];

static
GSize digits_size[4] = {
    { .w = 34, .h = 9},
    { .w = 9, .h = 34},
    { .w = 32, .h = 7},
    { .w = 7, .h = 32}
};

static
GBitmap* digits_anim[36];

static
GSize digits_anim_size[36] = {
// Big
    { .w = 13, .h = 34},
    { .w = 17, .h = 32},
    { .w = 19, .h = 30},
    { .w = 23, .h = 28},
    { .w = 25, .h = 24},
    { .w = 27, .h = 22},
    { .w = 31, .h = 20},
    { .w = 33, .h = 16},
    { .w = 33, .h = 12},
    { .w = 34, .h = 13},
    { .w = 32, .h = 17},
    { .w = 30, .h = 19},
    { .w = 28, .h = 23},
    { .w = 24, .h = 25},
    { .w = 22, .h = 27},
    { .w = 20, .h = 31},
    { .w = 16, .h = 33},
    { .w = 12, .h = 33},
// Small
    { .w = 11, .h = 32},
    { .w = 15, .h = 30},
    { .w = 17, .h = 28},
    { .w = 21, .h = 26},
    { .w = 23, .h = 24},
    { .w = 25, .h = 20},
    { .w = 29, .h = 18},
    { .w = 31, .h = 14},
    { .w = 31, .h = 10},
    { .w = 32, .h = 11},
    { .w = 30, .h = 14},
    { .w = 28, .h = 17},
    { .w = 26, .h = 20},
    { .w = 23, .h = 24},
    { .w = 21, .h = 26},
    { .w = 18, .h = 28},
    { .w = 14, .h = 30},
    { .w = 11, .h = 31}
};
  
void load_images(void)
{
    digits_segments[0] = gbitmap_create_with_resource(RESOURCE_ID_BIGDIGIT_HORIZONTAL_WHITE);
    digits_segments[1] = gbitmap_create_with_resource(RESOURCE_ID_BIGDIGIT_VERTICAL_WHITE);
    digits_segments[2] = gbitmap_create_with_resource(RESOURCE_ID_SMALLDIGIT_HORIZONTAL_WHITE);
    digits_segments[3] = gbitmap_create_with_resource(RESOURCE_ID_SMALLDIGIT_VERTICAL_WHITE);
    
    digits_anim[0] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_0_WHITE);
    digits_anim[1] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_1_WHITE);
    digits_anim[2] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_2_WHITE);
    digits_anim[3] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_3_WHITE);
    digits_anim[4] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_4_WHITE);
    digits_anim[5] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_5_WHITE);
    digits_anim[6] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_6_WHITE);
    digits_anim[7] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_7_WHITE);
    digits_anim[8] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_8_WHITE);
    digits_anim[9] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_9_WHITE);
    digits_anim[10] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_10_WHITE);
    digits_anim[11] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_11_WHITE);
    digits_anim[12] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_12_WHITE);
    digits_anim[13] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_13_WHITE);
    digits_anim[14] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_14_WHITE);
    digits_anim[15] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_15_WHITE);
    digits_anim[16] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_16_WHITE);
    digits_anim[17] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_BIG_17_WHITE);
    digits_anim[18] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_0_WHITE);
    digits_anim[19] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_1_WHITE);
    digits_anim[20] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_2_WHITE);
    digits_anim[21] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_3_WHITE);
    digits_anim[22] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_4_WHITE);
    digits_anim[23] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_5_WHITE);
    digits_anim[24] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_6_WHITE);
    digits_anim[25] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_7_WHITE);
    digits_anim[26] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_8_WHITE);
    digits_anim[27] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_9_WHITE);
    digits_anim[28] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_10_WHITE);
    digits_anim[29] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_11_WHITE);
    digits_anim[30] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_12_WHITE);
    digits_anim[31] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_13_WHITE);
    digits_anim[32] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_14_WHITE);
    digits_anim[33] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_15_WHITE);
    digits_anim[34] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_16_WHITE);
    digits_anim[35] = gbitmap_create_with_resource(RESOURCE_ID_SEGMENT_SMALL_17_WHITE);
}

void unload_images(void)
{
    for (int i = 0; i < 4; ++i) {
        gbitmap_destroy(digits_segments[i]);
    }
    for (int i = 0; i < 36; ++i) {
        gbitmap_destroy(digits_anim[i]);
    }
}

GBitmap* get_digit_image(digit_orientation_t orientation, digit_size_t size, GSize* bitmap_size)
{
    int stored_index = size * 2 + orientation;
    if (bitmap_size != NULL) {
        *bitmap_size = digits_size[stored_index];
    }
    return digits_segments[stored_index];
}

GBitmap* get_anim_image(digit_size_t size, int segment_angle, GSize* bitmap_size)
{
    int stored_index = size * 18 + segment_angle;
    if (bitmap_size != NULL) {
        *bitmap_size = digits_anim_size[stored_index];
    }
    return digits_anim[stored_index];
}
