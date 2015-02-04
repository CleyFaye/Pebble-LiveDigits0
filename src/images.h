#ifndef INCL_IMAGES_H
#define INCL_IMAGES_H

#include <pebble.h>

typedef enum {
    DIGIT_HORIZONTAL = 0,
    DIGIT_VERTICAL
} digit_orientation_t;

typedef enum {
    DIGIT_BIG = 0,
    DIGIT_SMALL
} digit_size_t;
    
void load_images(void);
void unload_images(void);

GBitmap* get_digit_image(digit_orientation_t orientation, digit_size_t size, GSize* bitmap_size);
/**
 * @param segment_angle value between 0 and 17 (inclusive) indicating angles in increment of 9 degrees, skipping 90.
 */
GBitmap* get_anim_image(digit_size_t size, int segment_angle, GSize* bitmap_size);

#endif
