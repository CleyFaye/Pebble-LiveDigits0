/** @file
 * Implementation of DigitLayer code
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "utils.h"
#include "digit_anim.h"
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
    /** Segment polygon path */
    GPath* segment_poly;
    /** Draw outlines */
    bool draw_outlines;

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
        {4, 4},  {4, 4},   {41, 4}, {4, 41},
        {4, 41}, {41, 41}, {4, 78}
    }, {
        {3, 3},  {3, 3},   {38, 3}, {3, 38},
        {3, 38}, {38, 38}, {3, 73}
    }, {
        {1, 1},  {1, 2},   {14, 2}, {1, 16},
        {1, 17}, {14, 17}, {1, 31}
    }
};

/** Orientation of static segments. (shared by all digits size) */
static
int32_t static_segment_orientation[7] = {
    0, 90, 90, 0, 90, 90, 0
};

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

static
void
draw_segment(GContext* ctx,
             GPath* path,
             GPoint offset,
             int32_t angle,
             GColor color,
             bool fill);

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
draw_segment(GContext* ctx,
             GPath* path,
             GPoint offset,
             int32_t angle,
             GColor color,
             bool fill)
{
    gpath_move_to(path,
                  offset);
    gpath_rotate_to(path,
                    angle_to_pebangle(angle));
    graphics_context_set_fill_color(ctx,
                                    color);
    graphics_context_set_stroke_color(ctx,
                                      color);

    if (fill) {
        gpath_draw_filled(ctx,
                          path);
    }

    gpath_draw_outline(ctx,
                       path);
}

static
void
handle_layer_update(struct Layer* layer,
                    GContext* ctx)
{
    digit_info_t* info = get_info(layer);
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
    const GPoint* segment_offsets = static_segment_offset[info->size];

    digit_fixed_segments_t segments =
        anim_get_fixed_segments(info->current_anim);
    bool draw_outline = info->draw_outlines && info->size != DS_SMALL;

    for (int i = 0;
         i < 7;
         ++i) {
        bool digit_active = anim_get_fixed_segment_state(segments,
                            i);

        if (!digit_active && !draw_outline) {
            continue;
        }

        draw_segment(ctx,
                     info->segment_poly,
                     segment_offsets[i],
                     static_segment_orientation[i],
                     GColorWhite,
                     digit_active);
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

        GPoint offset;
        int32_t orientation =
            anim_segment_get(anim,
                             info->current_anim_position,
                             info->size,
                             &offset);
        draw_segment(ctx,
                     info->segment_poly,
                     offset,
                     orientation,
                     GColorWhite,
                     true);
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
    info->segment_poly = NULL;
    info->draw_outlines = true;
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
    info->segment_poly = poly_aquire(size);
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
    poly_release(info->size);
    layer_destroy(layer);
}

