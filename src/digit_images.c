/** @file
 * Manage loading/retrieving digit segment's images.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// GBitmap
// GSize
// gbitmap_destroy()
#include <pebble.h>
// load_bitmap_into_array_from_id()
#include "utils.h"
// segment_orientation_t
// digit_size_t
// segment_res_ids
#include "digit_info.h"
// Associated header
#include "digit_images.h"

// ===================
// PRIVATE VARIABLES =
// ===================

/** Array of pointer of GBitmap for all possible orientations. */
typedef GBitmap* bitmap_array_t[SEGMENTS_ORIENTATION_COUNT];

/** Array of GSize for all possible orientations. */
typedef GSize size_array_t[SEGMENTS_ORIENTATION_COUNT];

/** All big segment's images. */
static
bitmap_array_t big_segments;

/** All big segment's images size. */
static
size_array_t big_segments_size = {
    // VERT
    { .w = 9, .h = 34},
    // 9
    { .w = 13, .h = 34},
    // 18
    { .w = 17, .h = 32},
    // 27
    { .w = 19, .h = 30},
    // 36
    { .w = 23, .h = 28},
    // 45
    { .w = 25, .h = 24},
    // 54
    { .w = 27, .h = 22},
    // 63
    { .w = 31, .h = 20},
    // 72
    { .w = 33, .h = 16},
    // 81
    { .w = 33, .h = 12},
    // HORIZ
    { .w = 34, .h = 9},
    // 99
    { .w = 34, .h = 13},
    // 108
    { .w = 32, .h = 17},
    // 117
    { .w = 30, .h = 19},
    // 126
    { .w = 28, .h = 23},
    // 135
    { .w = 24, .h = 25},
    // 144
    { .w = 22, .h = 27},
    // 153
    { .w = 20, .h = 31},
    // 162
    { .w = 16, .h = 33},
    // 171
    { .w = 12, .h = 33}
};

/** All medium segment's images. */
static
bitmap_array_t medium_segments;

/** All medium segment's images size. */
static
size_array_t medium_segments_size = {
    // VERT
    { .w = 7, .h = 32},
    // 9
    { .w = 11, .h = 32},
    // 18
    { .w = 15, .h = 30},
    // 27
    { .w = 17, .h = 28},
    // 36
    { .w = 21, .h = 26},
    // 45
    { .w = 23, .h = 24},
    // 54
    { .w = 25, .h = 20},
    // 63
    { .w = 29, .h = 18},
    // 72
    { .w = 31, .h = 14},
    // 81
    { .w = 31, .h = 10},
    // HORIZ
    { .w = 32, .h = 7},
    // 99
    { .w = 32, .h = 11},
    // 108
    { .w = 30, .h = 14},
    // 117
    { .w = 28, .h = 17},
    // 126
    { .w = 26, .h = 20},
    // 135
    { .w = 23, .h = 24},
    // 144
    { .w = 21, .h = 26},
    // 153
    { .w = 18, .h = 28},
    // 162
    { .w = 14, .h = 30},
    // 171
    { .w = 11, .h = 31}
};

/** List of segments images lists for convenience. */
static
bitmap_array_t* segments_images[DIGITS_SIZE_COUNT] = {
    &big_segments,
    &medium_segments
};

/** List of segments size lists for convenience. */
static
size_array_t* segments_sizes[DIGITS_SIZE_COUNT] = {
    &big_segments_size,
    &medium_segments_size
};

/** Keep track of how many times a set of images was acquired.
 *
 * Each size of digit can be loaded separately. The counter keep track of the
 * number of "loading"/"unloading" request. When the first loading happen,
 * images are loaded. When the last unloading happen, they are released.
 */
static
unsigned segments_images_load_counter[DIGITS_SIZE_COUNT] = { 0, 0};

/** Simple function profile, used to load/unload stuff. */
typedef void(*func_t)(void);

// ===============================
// PRIVATE FUNCTION DECLARATIONS =
// ===============================

/** Effectively load big segments bitmaps. */
static
void
do_load_big_segments(void);
/** Effectively unload big segments bitmaps. */
static
void
do_unload_big_segments(void);
/** Effectively load big segments bitmaps. */
static
void
do_load_medium_segments(void);
/** Effectively unload big segments bitmaps. */
static
void
do_unload_medium_segments(void);

/** Loading functions */
func_t digit_load_funcs[DIGITS_SIZE_COUNT] = {
    do_load_big_segments,
    do_load_medium_segments
};

/** Unloading functions */
func_t digit_unload_funcs[DIGITS_SIZE_COUNT] = {
    do_unload_big_segments,
    do_unload_medium_segments
};

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void
do_load_big_segments(void)
{
    load_bitmap_into_array_from_id(segment_res_ids[DS_BIG],
                                   big_segments,
                                   SEGMENTS_ORIENTATION_COUNT);
}

static
void
do_unload_big_segments(void)
{
    for (unsigned index = 0;
         index < SEGMENTS_ORIENTATION_COUNT;
         ++index) {
        gbitmap_destroy(big_segments[index]);
        big_segments[index] = NULL;
    }
}

static
void
do_load_medium_segments(void)
{
    load_bitmap_into_array_from_id(segment_res_ids[DS_MEDIUM],
                                   medium_segments,
                                   SEGMENTS_ORIENTATION_COUNT);
}

static
void
do_unload_medium_segments(void)
{
    for (unsigned index = 0;
         index < SEGMENTS_ORIENTATION_COUNT;
         ++index) {
        gbitmap_destroy(medium_segments[index]);
        medium_segments[index] = NULL;
    }
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

void
segment_load_images(digit_size_t size)
{
    if (segments_images_load_counter[size]++ == 0) {
        digit_load_funcs[size]();
    }
}

void
segment_unload_images(digit_size_t size)
{
    if (--segments_images_load_counter[size] == 0) {
        digit_unload_funcs[size]();
    }
}

GBitmap*
segment_get_image(digit_size_t size,
                  segment_orientation_t orientation,
                  GSize* bitmap_size)
{
    GBitmap* result = (*segments_images[size])[orientation];

    if (bitmap_size) {
        *bitmap_size = (*segments_sizes[size])[orientation];
    }

    return result;
}

