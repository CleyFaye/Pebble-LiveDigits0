/** @file
 * Code to manipulate mono bitmaps.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>
#include "bitres.h"
#include "monobitmap.h"

// =====================
// PALETTES MANAGEMENT =
// =====================
// PRIVATE TYPES =
// ===============

#ifdef PBL_COLOR

/** Information about a generated palette.
 *
 * Palettes are shared between all monobitmaps that have the same from/to pair
 * of colors.
 */
typedef struct palette_s {
    /** Background color */
    MonoColor to;
    /** Actual palette. 0 = transparent, 3 = front color */
    GColor colors[4];
    /** Number of time this palette is in use */
    unsigned ref_count;
    // Chaining
    struct palette_s* prev;
    struct palette_s* next;
}* palette_t;

/** The Pebble color corresponding to MonoColor values. */
static
GColor base_colors[12] = {
    { .a = 3, .r = 0, .g = 0, .b = 0},
    { .a = 3, .r = 3, .g = 3, .b = 3},
    { .a = 3, .r = 3, .g = 0, .b = 0},
    { .a = 3, .r = 3, .g = 1, .b = 0},
    { .a = 3, .r = 3, .g = 3, .b = 0},
    { .a = 3, .r = 0, .g = 3, .b = 0},
    { .a = 3, .r = 0, .g = 2, .b = 0},
    { .a = 3, .r = 0, .g = 3, .b = 3},
    { .a = 3, .r = 0, .g = 2, .b = 3},
    { .a = 3, .r = 0, .g = 0, .b = 3},
    { .a = 3, .r = 1, .g = 0, .b = 1},
    { .a = 3, .r = 3, .g = 0, .b = 2}
};

// ===================
// PRIVATE VARIABLES =
// ===================

/** All allocated dynamic palettes */
static
palette_t heap_palettes = NULL;

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

/** Return a palette handler for a given pair of colors.
 *
 * Each call must be mirrored by a call to free_palette()
 */
static
palette_t
get_palette(MonoColor from,
            MonoColor to);

/** Mark a palette as freed.
 *
 * Actual cleanup happen in commit_free_palette()
 */
static inline
void
free_palette(palette_t palette)
{
    --palette->ref_count;
}

/** Clear all unused palettes */
static
void
commit_free_palette(void);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
palette_t
get_palette(MonoColor from,
            MonoColor to)
{
    palette_t cursor = heap_palettes;

    while (cursor) {
        if (GColorEq(cursor->colors[3], base_colors[from]) &&
            cursor->to == to) {
            ++cursor->ref_count;
            return cursor;
        }

        cursor = cursor->next;
    }

    // Not found, allocate new
    palette_t new_palette = malloc(sizeof(struct palette_s));
    new_palette->to = to;
    new_palette->ref_count = 1;
    GColor from_color = base_colors[from];
    GColor to_color = base_colors[to];
    int red_offset = (int) to_color.r - from_color.r;
    int green_offset = (int) to_color.g - from_color.g;
    int blue_offset = (int) to_color.b - from_color.b;

    new_palette->colors[3] = from_color;

    for (unsigned i = 2; i > 0; --i) {
        new_palette->colors[i] = new_palette->colors[i + 1];

        if (red_offset > 0) {
            ++new_palette->colors[i].r;
            --red_offset;
        } else if (red_offset < 0) {
            --new_palette->colors[i].r;
            ++red_offset;
        }

        if (green_offset > 0) {
            ++new_palette->colors[i].g;
            --green_offset;
        } else if (green_offset < 0) {
            --new_palette->colors[i].g;
            ++green_offset;
        }

        if (blue_offset > 0) {
            ++new_palette->colors[i].b;
            --blue_offset;
        } else if (blue_offset < 0) {
            --new_palette->colors[i].b;
            ++blue_offset;
        }
    }

    new_palette->colors[0] = GColorClear;
    new_palette->prev = NULL;
    new_palette->next = heap_palettes;
    heap_palettes = new_palette;

    if (new_palette->next) {
        new_palette->next->prev = new_palette;
    }

    return new_palette;
}

static
void
commit_free_palette(void)
{
    palette_t cursor = heap_palettes;

    while (cursor) {
        palette_t next = cursor->next;

        if (cursor->ref_count == 0) {
            if (cursor->prev) {
                cursor->prev->next = next;
            } else {
                heap_palettes = next;
            }

            if (cursor->next) {
                cursor->next->prev = cursor->prev;
            }

            free(cursor);
        }

        cursor = next;
    }
}

#else
#define free_palette(x)
#define commit_free_palette()
#endif

// =======================
// MONOBITMAP MANAGEMENT =
// =======================
// PRIVATE TYPES =
// ===============

/** The info about a monobitmap */
struct monobitmap_s {
    /** Resource used to load this monobitmap */
    uint32_t res_id;
    /** Reference counter */
    unsigned ref_count;
#ifdef PBL_COLOR
    /** The palette used for this bitmap */
    palette_t palette;
#else
    bool white;
#endif
    bool vertical_flip;
    bool horizontal_flip;
    /** The pebble bitmap object. We'll draw this. */
    GBitmap* real_bitmap;
    struct monobitmap_s* prev;
    struct monobitmap_s* next;
};

// ===================
// PRIVATE VARIABLES =
// ===================

static
MonoBitmap heap_monobitmaps = NULL;

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

/** Return the next stripe in an RLE encoded mono bitmap.
 *
 * For mono bitmap file with version 1 and 2, pixels colors are stored as
 * "stripes". This function fetch the next stripes from the resource.
 *
 * @param stripe_length The length of the next stripe, in pixels
 * @param stripe_value The color value of the pixels for the next stripe
 */
static
void
get_next_stripe(bitres_t in_res,
                unsigned* stripe_length,
                unsigned* stripe_value);

/** Set a pixel in a raw bitmap data.
 *
 * This function modify a pixel in a 2bit-palette format GBitmap.
 */
static
void
bitmap_data_set_pixel_2b(uint8_t* data,
                         unsigned x,
                         unsigned y,
                         unsigned row_byte_size,
                         uint8_t value);

/** Decode an horizontal-encoded mono bitmap resource. */
static
void
decodeHRLE(bitres_t in_res,
           GSize bitmap_size,
           unsigned row_byte_size,
           uint8_t* bitmap_data,
           bool horizontal_flip,
           bool vertical_flip);

/** Decode a vertical-encoded mono bitmap resource. */
static
void
decodeVRLE(bitres_t in_res,
           GSize bitmap_size,
           unsigned row_byte_size,
           uint8_t* bitmap_data,
           bool horizontal_flip,
           bool vertical_flip);

/** Decode a raw-encoded mono bitmap resource. */
static
void
decodeRAW(bitres_t in_res,
          GSize bitmap_size,
          unsigned row_byte_size,
          uint8_t* bitmap_data,
          bool horizontal_flip,
          bool vertical_flip);

static
void
commit_free_bitmaps(void);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void
get_next_stripe(bitres_t in_res,
                unsigned* stripe_length,
                unsigned* stripe_value)
{
    if (bitres_read(in_res)) {
        // A real stripe
        *stripe_length =
            bitres_readbyte(in_res,
                            6)
            + 4;
    } else {
        // A single pixel
        *stripe_length = 1;
    }

    *stripe_value = bitres_readbyte(in_res,
                                    2);
}

static
void
bitmap_data_set_pixel_2b(uint8_t* data,
                         unsigned x,
                         unsigned y,
                         unsigned row_byte_size,
                         uint8_t value)
{
#ifdef PBL_COLOR
    unsigned byte_id = y * row_byte_size + x / 4;
    // Each pixel is two bit wide
    unsigned bit_id = 7 - (x * 2 % 8);

    if (value & 1) {
        data[byte_id] |= 1u << bit_id;
    }

    if (value & 2) {
        data[byte_id] |= 1u << (bit_id - 1);
    }

#else

    if (value == 3) {
        unsigned byte_id = y * row_byte_size + x / 8;
        // Each pixel is one bit wide
        unsigned bit_id = x % 8;
        data[byte_id] |= 1u << bit_id;
    }

#endif
}

static
void
decodeHRLE(bitres_t in_res,
           GSize bitmap_size,
           unsigned row_byte_size,
           uint8_t* bitmap_data,
           bool horizontal_flip,
           bool vertical_flip)
{
    unsigned stripe_length = 0;
    unsigned stripe_value = 0;

    for (unsigned y = 0;
         (int) y < bitmap_size.h;
         ++y) {
        for (unsigned x = 0;
             (int) x < bitmap_size.w;
             ++x) {
            if (stripe_length == 0) {
                get_next_stripe(in_res,
                                &stripe_length,
                                &stripe_value);
            }

            bitmap_data_set_pixel_2b(bitmap_data,
                                     (horizontal_flip
                                      ? (bitmap_size.w - x - 1)
                                      : x),
                                     (vertical_flip
                                      ? (bitmap_size.h - y - 1)
                                      : y),
                                     row_byte_size,
                                     stripe_value);
            --stripe_length;
        }
    }
}

static
void
decodeVRLE(bitres_t in_res,
           GSize bitmap_size,
           unsigned row_byte_size,
           uint8_t* bitmap_data,
           bool horizontal_flip,
           bool vertical_flip)
{
    unsigned stripe_length = 0;
    unsigned stripe_value = 0;

    for (unsigned x = 0;
         (int) x < bitmap_size.w;
         ++x) {
        for (unsigned y = 0;
             (int) y < bitmap_size.h;
             ++y) {
            if (stripe_length == 0) {
                get_next_stripe(in_res,
                                &stripe_length,
                                &stripe_value);
            }

            bitmap_data_set_pixel_2b(bitmap_data,
                                     (horizontal_flip
                                      ? (bitmap_size.w - x - 1)
                                      : x),
                                     (vertical_flip
                                      ? (bitmap_size.h - y - 1)
                                      : y),
                                     row_byte_size,
                                     stripe_value);
            --stripe_length;
        }
    }
}

static
void
decodeRAW(bitres_t in_res,
          GSize bitmap_size,
          unsigned row_byte_size,
          uint8_t* bitmap_data,
          bool horizontal_flip,
          bool vertical_flip)
{
    for (unsigned y = 0;
         (int) y < bitmap_size.h;
         ++y) {
        for (unsigned x = 0;
             (int) x < bitmap_size.w;
             ++x) {
            int byteValue = bitres_readbyte(in_res, 2);
            bitmap_data_set_pixel_2b(bitmap_data,
                                     (horizontal_flip
                                      ? (bitmap_size.w - x - 1)
                                      : x),
                                     (vertical_flip
                                      ? (bitmap_size.h - y - 1)
                                      : y),
                                     row_byte_size,
                                     byteValue);

        }
    }
}

static
void
commit_free_bitmaps(void)
{
    MonoBitmap cursor = heap_monobitmaps;

    unsigned count = 0;

    while (cursor) {
        ++count;
        MonoBitmap next = cursor->next;

        if (cursor->ref_count == 0) {
            if (cursor->prev) {
                cursor->prev->next = cursor->next;
            } else {
                heap_monobitmaps = next;
            }

            if (cursor->next) {
                cursor->next->prev = cursor->prev;
            }

            gbitmap_destroy(cursor->real_bitmap);
            free_palette(cursor->palette);
            free(cursor);
        }

        cursor = next;
    }
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

GColor
monobitmap_color_from_monocolor(MonoColor color)
{
#ifdef PBL_COLOR
    return base_colors[color];
#else

    switch (color) {
    case MONO_BLACK:
    case MONO_ORANGE:
    case MONO_DARKGREEN:
    case MONO_BLUE:
    case MONO_DEEPBLUE:
    case MONO_PURPLE:
        return GColorBlack;

    default:
        return GColorWhite;
    }

#endif
}

MonoBitmap
monobitmap_create_with_resource(uint32_t resource_id,
                                MonoColor frontColor,
                                MonoColor backColor,
                                bool vertical_flip,
                                bool horizontal_flip)
{
#ifdef PBL_COLOR
    palette_t palette = get_palette(frontColor,
                                    backColor);
    // First search through already loaded bitmaps
    MonoBitmap cursor = heap_monobitmaps;

    while (cursor) {
        if (cursor->res_id == resource_id &&
            cursor->palette == palette &&
            cursor->vertical_flip == vertical_flip &&
            cursor->horizontal_flip == horizontal_flip) {
            // Keep palette ref counter consistent
            free_palette(palette);
            ++cursor->ref_count;
            return cursor;
        }

        cursor = cursor->next;
    }

#else
    bool white = monobitmap_color_from_monocolor(frontColor) == GColorWhite;
    // First search through already loaded bitmaps
    MonoBitmap cursor = heap_monobitmaps;

    while (cursor) {
        if (cursor->res_id == resource_id &&
            cursor->white == white &&
            cursor->vertical_flip == vertical_flip &&
            cursor->horizontal_flip == horizontal_flip) {
            ++cursor->ref_count;
            return cursor;
        }

        cursor = cursor->next;
    }

#endif

    // Allocate new monobitmap
    MonoBitmap result = malloc(sizeof(struct monobitmap_s));
    result->res_id = resource_id;
#ifdef PBL_COLOR
    result->palette = palette;
#else
    result->white = white;
#endif
    result->ref_count = 1;
    result->vertical_flip = vertical_flip;
    result->horizontal_flip = horizontal_flip;
    result->next = heap_monobitmaps;

    if (result->next) {
        result->next->prev = result;
    }

    result->prev = NULL;
    heap_monobitmaps = result;

    // Load MBP
    bitres_t in_res = bitres_open(resource_id);
    GSize bitmap_size;
    uint8_t version = bitres_readbyte(in_res,
                                      8);
    bitmap_size.w = bitres_readbyte(in_res,
                                    8);
    bitmap_size.h = bitres_readbyte(in_res,
                                    8);
#ifdef PBL_COLOR
    result->real_bitmap =
        gbitmap_create_blank_with_palette(bitmap_size,
                                          GBitmapFormat2BitPalette,
                                          result->palette->colors,
                                          false);
#else
    result->real_bitmap =
        gbitmap_create_blank(bitmap_size);
#endif
    unsigned row_byte_size = gbitmap_get_bytes_per_row(result->real_bitmap);
    uint8_t* bitmap_data = gbitmap_get_data(result->real_bitmap);

    switch (version) {
    case 1:
        decodeHRLE(in_res,
                   bitmap_size,
                   row_byte_size,
                   bitmap_data,
                   horizontal_flip,
                   vertical_flip);
        break;

    case 2:
        decodeVRLE(in_res,
                   bitmap_size,
                   row_byte_size,
                   bitmap_data,
                   horizontal_flip,
                   vertical_flip);
        break;

    case 3:
        decodeRAW(in_res,
                  bitmap_size,
                  row_byte_size,
                  bitmap_data,
                  horizontal_flip,
                  vertical_flip);
        break;
    }

#ifdef PBL_BW

    if (!result->white) {
        for (unsigned byte_index = 0;
             byte_index < (row_byte_size * bitmap_size.h);
             ++byte_index) {
            bitmap_data[byte_index] = ~bitmap_data[byte_index];
        }
    }

#endif

    bitres_close(in_res);
    return result;
}

GSize
monobitmap_get_size(const MonoBitmap bitmap)
{
    return gbitmap_get_bounds(bitmap->real_bitmap).size;
}

void
monobitmap_draw_bitmap_in_rect(GContext* ctx,
                               const MonoBitmap bitmap,
                               GRect rect)
{
#ifdef PBL_COLOR
    graphics_context_set_compositing_mode(ctx,
                                          GCompOpSet);
#else
    graphics_context_set_compositing_mode(ctx,
                                          bitmap->white
                                          ? GCompOpOr
                                          : GCompOpAnd);
#endif
    graphics_draw_bitmap_in_rect(ctx,
                                 bitmap->real_bitmap,
                                 rect);
}

void
monobitmap_destroy(MonoBitmap bitmap)
{
    --bitmap->ref_count;
}

void monobitmap_commit_destroy(void)
{
    commit_free_bitmaps();
    commit_free_palette();
}

