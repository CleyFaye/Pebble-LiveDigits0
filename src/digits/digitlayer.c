/** @file
 * Implementation of DigitLayer code
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "utils.h"
#include "digit_anim.h"
#include "digit_images.h"
#include "digit_info.h"

#include "digitlayer.h"

// ===============
// PRIVATE TYPES =
// ===============

/** State of a digit layer. */
typedef struct {
    digit_size_t size;
    /** Currently displayed number (updated with each transition) */
    int current_number;
    /** Target number to display (set by digit_layer_set_number()) */
    unsigned target_number;
    /** Current animation step */
    digit_anim_t current_anim;
    /** Current animation position (always <9) */
    int current_anim_position;

    animation_speed_t animate_speed;
    bool quick_wrap;
    /** Used to skip every other tick in slow animation mode */
    bool animate_skipbeat;
} digit_info_t;

/** Offsets to draw the 7 static segments of a digit. */
typedef const GPoint static_segment_offset_t[7];

// ===============
// PRIVATE CONST =
// ===============

/** Placement of the segments to display each digit size. */
static
static_segment_offset_t static_segment_offset[DIGITS_SIZE_COUNT] = {
    {
        {6, 0},  {0, 6},   {37, 6}, {6, 37},
        {0, 43}, {37, 43}, {6, 74}
    }, {
        {5, 0},  {0, 5},   {35, 5}, {5, 35},
        {0, 40}, {35, 40}, {5, 70}
    }, {
        {2, 0},  {0, 3},   {13, 3}, {2, 15},
        {0, 18}, {13, 18}, {2, 30}
    }
};

/** Orientation of static segments. (shared by all digits size) */
static
segment_orientation_t static_segment_orientation[7] = {
    SO_HORIZONTAL, SO_VERTICAL, SO_VERTICAL,
    SO_HORIZONTAL, SO_VERTICAL, SO_VERTICAL,
    SO_HORIZONTAL
};

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

/** Draw the current digit animation step on the layer. */
static
void
handle_layer_update(struct Layer* layer,
                    GContext* ctx);

/** Draw static part of a digit. */
static
void
draw_static_digit(digit_info_t* info,
                  GPoint* layer_offset,
                  GContext* ctx);

/** Draw animated part of a digit */
static
void
draw_animated_segments(digit_info_t* info,
                       GPoint* layer_offset,
                       GContext* ctx);

/** Return the digit_info_t associated with a layer */
static inline
digit_info_t*
get_info(DigitLayer* layer)
{
    return (digit_info_t*) layer_get_data(layer);
}

/** Initialize the digit_info_t */
static
void
info_init(digit_info_t* info);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void
handle_layer_update(struct Layer* layer,
                    GContext* ctx)
{
    digit_info_t* info = get_info(layer);
    GRect layer_bounds = layer_get_bounds(layer);
#ifdef PBL_COLOR
    graphics_context_set_compositing_mode(ctx,
                                          GCompOpSet);
#else
    graphics_context_set_compositing_mode(ctx,
            GCompOpOr);
#endif

    draw_static_digit(info,
                      &layer_bounds.origin,
                      ctx);
    draw_animated_segments(info,
                           &layer_bounds.origin,
                           ctx);
}

static
void
draw_static_digit(digit_info_t* info,
                  GPoint* layer_offset,
                  GContext* ctx)
{
    const GPoint* segment_offsets = static_segment_offset[info->size];

    digit_fixed_segments_t segments =
        anim_get_fixed_segments(info->current_anim);

    for (int i = 0;
         i < 7;
         ++i) {
        if (!anim_get_fixed_segment_state(segments,
                                          i)) {
            continue;
        }

        GRect draw_rect;
        draw_rect.origin = segment_offsets[i];
        draw_rect.origin.x += layer_offset->x;
        draw_rect.origin.y += layer_offset->y;
        GBitmap* digit_bitmap =
            segment_get_image(info->size,
                              static_segment_orientation[i],
                              &draw_rect.size);
        graphics_draw_bitmap_in_rect(ctx,
                                     digit_bitmap,
                                     draw_rect);
    }
}

static
void
draw_animated_segments(digit_info_t* info,
                       GPoint* layer_offset,
                       GContext* ctx)
{
    const digit_segment_animation_t* segment_anim =
        anim_get_segment_anim(info->current_anim);

    for (int i = 0;
         i < 2;
         ++i) {
        segment_anim_t anim = (*segment_anim)[i];

        if (anim == SA_NOANIM) {
            continue;
        }

        GRect draw_rect;
        segment_orientation_t orientation =
            anim_segment_get(anim,
                             info->current_anim_position,
                             info->size,
                             &draw_rect.origin);
        draw_rect.origin.x += layer_offset->x;
        draw_rect.origin.y += layer_offset->y;
        GBitmap* digit_bitmap =
            segment_get_image(info->size,
                              orientation,
                              &draw_rect.size);
        graphics_draw_bitmap_in_rect(ctx,
                                     digit_bitmap,
                                     draw_rect);
    }
}

static
void
info_init(digit_info_t* info)
{
    info->size = DS_BIG;
    info->current_number = 0;
    info->target_number = 0;
    info->current_anim = DA_0;
    info->current_anim_position = 0;
    info->animate_speed = FAST_MERGED;
    info->quick_wrap = false;
    info->animate_skipbeat = false;
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

DigitLayer*
digit_layer_create(digit_size_t size,
                   GPoint offset)
{
    segment_load_images(size);
    GRect layer_rect;
    layer_rect.origin = offset;
    layer_rect.size = digit_dimensions[size];
    DigitLayer* result =
        layer_create_with_init_data(layer_rect,
                                    sizeof(digit_info_t),
                                    (layer_data_init_t) info_init);
    layer_set_update_proc(result,
                          handle_layer_update);

    digit_info_t* info = get_info(result);
    info->size = size;
    return result;
}

void
digit_layer_set_quick_wrap(DigitLayer* layer,
                           bool quick_wrap)
{
    get_info(layer)->quick_wrap = quick_wrap;
}

void
digit_layer_set_animate_speed(DigitLayer* layer,
                              animation_speed_t speed)
{
    get_info(layer)->animate_speed = speed;
}

void
digit_layer_set_number(DigitLayer* layer,
                       unsigned target_number,
                       bool animate)
{
    digit_info_t* info = get_info(layer);

    if (info->target_number == target_number) {
        return;
    }

    info->target_number = target_number % 10;

    // If we are already in an animation, keep going
    if (!animate && anim_is_static_digit(info->current_anim)) {
        info->current_number = info->target_number;
        info->current_anim = anim_get_anim_for_number(info->current_number);
        info->current_anim_position = 0;
    }

    layer_mark_dirty(layer);
}

void
digit_layer_kill_anim(DigitLayer* layer)
{
    digit_info_t* info = get_info(layer);
    info->current_anim = anim_get_anim_for_number(info->target_number);
    info->current_number = info->target_number;
    info->current_anim_position = 0;
    layer_mark_dirty(layer);
}

bool
digit_layer_animate(DigitLayer* layer)
{
    digit_info_t* info = get_info(layer);

    // Only animate if needed
    if (info->current_number == ((int) info->target_number)) {
        return false;
    }

    if (layer_get_hidden(layer)) {
        return false;
    }

    switch (info->animate_speed) {
    case SLOW_SEPARATE:

        // Skip every other tick in every cases
        if (!info->animate_skipbeat) {
            info->animate_skipbeat = true;
            return true;
        } else {
            info->animate_skipbeat = false;
        }

        break;

    case SLOW_MERGED:
        if (!anim_is_multipart(info->current_anim)) {
            // Multi-step transition just run normally,
            // for single-step transition, skip every other tick
            if (!info->animate_skipbeat) {
                info->animate_skipbeat = true;
                return true;
            } else {
                info->animate_skipbeat = false;
            }
        }

        break;

    case FAST_SEPARATE:
        break;

    case FAST_MERGED:

        // Tick every time, twice faster for two-step transitions
        if (anim_is_multipart(info->current_anim)) {
            ++info->current_anim_position;
        }
    }

    ++info->current_anim_position;

    if (anim_is_complete(info->current_anim,
                         info->current_anim_position)) {
        info->current_anim_position -= anim_get_step_count(info->current_anim);

        if (info->quick_wrap &&
            ((int) info->target_number) < info->current_number) {
            // If we're quickwrapping and we want a lower digit, start back from
            // 0
            // Forcefully set to -1 so we don't endlessly loop
            info->current_number = -1;
            info->current_anim = anim_get_next_quick_anim(info->current_anim);
        } else {
            // Just continue until we reach the right number
            info->current_anim = anim_get_next_anim(info->current_anim);

            if (anim_is_static_digit(info->current_anim)) {
                info->current_number =
                    anim_get_displayed_number(info->current_anim);
                info->current_anim_position = 0;
            }
        }
    }

    layer_mark_dirty(layer);
    return info->current_number != ((int) info->target_number);
}

void
digit_layer_destroy(DigitLayer* layer)
{
    digit_info_t* info = get_info(layer);
    segment_unload_images(info->size);
    layer_destroy(layer);
}

