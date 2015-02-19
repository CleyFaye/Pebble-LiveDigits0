/** @file
 * Implementation of DigitLayer code
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// struct Layer
// GContext
// GPoint
// GRect
// GCompOpOr
// GBitmap
// layer_create_with_data()
// layer_get_data()
// layer_get_bounds()
// layer_set_update_proc()
// layer_destroy()
// graphics_context_set_compositing_mode()
// graphics_draw_bitmap_in_rect()
#include <pebble.h>
// digit_anim_t
// animated_segment_info_t
// animation_digit_segment_t
// animation_fixed_digits_t
// anim_get_anim_for_number()
// anim_get_fixed_digits()
// anim_get_segment_anim()
// anim_segment_get()
#include "digit_anim.h"
// segment_get_image()
// segment_load_images()
// segment_unload_images()
#include "digit_images.h"
// digit_size_t
// segment_orientation_t
// digit_dimensions
#include "digit_info.h"
// Associated header
#include "digitlayer.h"

// ===============
// PRIVATE TYPES =
// ===============

/** State of a digit layer. */
typedef struct {
    /** Size of the digit */
    digit_size_t size;
    /** Currently displayed number */
    int current_number;
    /** Target number to display */
    int target_number;
    /** Current animation step */
    digit_anim_t current_anim;
    /** Current animation position (always <9) */
    int current_anim_position;
    /** Animation speed */
    animation_speed_t animate_speed;
    /** Quick wrap flag */
    bool quick_wrap;
    /** Skip every other tick in slow animation mode */
    bool animate_skipbeat;
} digit_info_t;

/** Placement of the segments to display a big digit. */
static const animated_segment_info_t big_digit_draw_info[7] = {
    {SO_HORIZONTAL, { .x = 6, .y = 0}},
    {SO_VERTICAL, { .x = 0, .y = 6}},
    {SO_VERTICAL, { .x = 37, .y = 6}},
    {SO_HORIZONTAL, { .x = 6, .y = 37}},
    {SO_VERTICAL, { .x = 0, .y = 43}},
    {SO_VERTICAL, { .x = 37, .y = 43}},
    {SO_HORIZONTAL, { .x = 6, .y = 74}}
};

/** Placement of the segments to display a medium digit. */
static const animated_segment_info_t medium_digit_draw_info[7] = {
    {SO_HORIZONTAL, { .x = 5, .y = 0}},
    {SO_VERTICAL, { .x = 0, .y = 5}},
    {SO_VERTICAL, { .x = 35, .y = 5}},
    {SO_HORIZONTAL, { .x = 5, .y = 35}},
    {SO_VERTICAL, { .x = 0, .y = 40}},
    {SO_VERTICAL, { .x = 35, .y = 40}},
    {SO_HORIZONTAL, { .x = 5, .y = 70}}
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

static
void
draw_animated_segments(digit_info_t* info,
                       GPoint* layer_offset,
                       GContext* ctx);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void
handle_layer_update(struct Layer* layer,
                    GContext* ctx)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    GRect layer_bounds = layer_get_bounds(layer);
    graphics_context_set_compositing_mode(ctx,
                                          GCompOpOr);

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
    const animated_segment_info_t* digit_draw_info =
        (info->size == DS_BIG)
        ? big_digit_draw_info
        : medium_digit_draw_info;
    const animation_fixed_digits_t* fixed_digits =
        anim_get_fixed_digits(info->current_anim);

    for (int i = 0;
         i < 7;
         ++i) {
        if (!fixed_digits->enabled[i]) {
            continue;
        }

        GRect draw_rect;
        draw_rect.origin = digit_draw_info[i].offset;
        draw_rect.origin.x += layer_offset->x;
        draw_rect.origin.y += layer_offset->y;
        GBitmap* digit_bitmap =
            segment_get_image(info->size,
                              digit_draw_info[i].segment_angle,
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
    const animation_digit_segment_t* segment_anim =
        anim_get_segment_anim(info->current_anim);

    for (int i = 0;
         i < 2;
         ++i) {
        if (segment_anim->segment_anim[i] != SA_NOANIM) {
            const animated_segment_info_t* segment_info =
                anim_segment_get(segment_anim->segment_anim[i],
                                 info->current_anim_position,
                                 info->size);
            GRect draw_rect;
            draw_rect.origin = segment_info->offset;
            draw_rect.origin.x += layer_offset->x;
            draw_rect.origin.y += layer_offset->y;
            GBitmap* digit_bitmap =
                segment_get_image(info->size,
                                  segment_info->segment_angle,
                                  &draw_rect.size);
            graphics_draw_bitmap_in_rect(ctx,
                                         digit_bitmap,
                                         draw_rect);
        }
    }
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

DigitLayer*
digit_layer_create(digit_size_t size,
                   GPoint offset)
{
    GRect layer_rect;
    layer_rect.origin = offset;
    segment_load_images(size);

    layer_rect.size = digit_dimensions[size];
    DigitLayer* result =
        layer_create_with_data(layer_rect,
                               sizeof(digit_info_t));
    digit_info_t* info = (digit_info_t*) layer_get_data(result);
    info->size = size;
    info->current_number = 0;
    info->target_number = 0;
    info->current_anim = DA_0;
    info->current_anim_position = 0;
    info->animate_speed = FAST_MERGED;
    info->quick_wrap = false;
    info->animate_skipbeat = false;
    layer_set_update_proc(result,
                          handle_layer_update);
    return result;
}

void
digit_layer_set_quick_wrap(DigitLayer* layer,
                           bool quick_wrap)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    info->quick_wrap = quick_wrap;
}

void
digit_layer_set_animate_speed(DigitLayer* layer,
                              animation_speed_t speed)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    info->animate_speed = speed;
}

void
digit_layer_set_number(DigitLayer* layer,
                       int target_number,
                       bool animate)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    info->target_number = target_number % 10;

    if (!animate) {
        info->current_number = info->target_number;
        info->current_anim = anim_get_anim_for_number(info->current_number);
        info->current_anim_position = 0;
    }
}

bool
digit_layer_animate(DigitLayer* layer)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);

    // Only animate if needed
    if (info->current_number == info->target_number) {
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

        ++info->current_anim_position;

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

        ++info->current_anim_position;

        break;

    case FAST_SEPARATE:
        // Tick every time.
        ++info->current_anim_position;
        break;

    case FAST_MERGED:

        // Tick every time, twice faster for two-step transitions
        if (anim_is_multipart(info->current_anim)) {
            info->current_anim_position += 2;
        } else {
            ++info->current_anim_position;
        }

        break;
    }

    if (anim_is_complete(info->current_anim,
                         info->current_anim_position)) {
        info->current_anim_position -= anim_get_step_count(info->current_anim);

        if (info->quick_wrap && info->target_number < info->current_number) {
            // If we're quickwrapping and we want a lower digit, start back from
            // 0
            // Here we forcefully set the currently displayed digit to -1 so we
            // don't loop endlessly trying to reach it. Animation step are still
            // dictated by the current_anim property.
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
    return info->current_number != info->target_number;
}

void
digit_layer_destroy(DigitLayer* layer)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    segment_unload_images(info->size);
    layer_destroy(layer);
}

