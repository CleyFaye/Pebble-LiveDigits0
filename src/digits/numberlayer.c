/** @đile
 * Implementation of NumberLayer code
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// struct Layer
// GPoint
// GSize
// bool
// layer_create_with_data()
// layer_get_data()
// layer_destroy()
#include <pebble.h>
// digit_size_t
// animation_speed_t
// digit_dimensions
#include "digit_info.h"
// DigitLayer
#include "digitlayer.h"
// Associated header
#include "numberlayer.h"

// ===============
// PRIVATE TYPES =
// ===============

/** State of a number layer. */
typedef struct {
    /** Number of digits */
    unsigned digits_count;
    /** Target number to display */
    unsigned displayed_number;
    /** The digits */
    DigitLayer* digits[];
} number_info_t;

// ================
// PRIVATE CONSTS =
// ================

/** Spacing between digits for each digit size. */
static
const unsigned digit_spacing[DIGITS_SIZE_COUNT] = {
    5,
    3,
    2
};

// =============================
// PUBLIC FUNCTION DEFINITIONS =
// =============================

NumberLayer*
number_layer_create(digit_size_t size,
                    unsigned digit_count,
                    GPoint offset)
{
    GRect layer_rect;
    layer_rect.origin = offset;
    unsigned digit_width = digit_dimensions[size].w;
    unsigned layer_width = digit_width * digit_count +
                           digit_spacing[size] * (digit_count - 1);
    layer_rect.size = GSize(layer_width,
                            digit_dimensions[size].h);

    size_t number_info_size = sizeof(number_info_t) +
                              sizeof(DigitLayer*) * digit_count;

    NumberLayer* result =
        layer_create_with_data(layer_rect,
                               number_info_size);
    number_info_t* info = (number_info_t*) layer_get_data(result);
    info->displayed_number = 0;
    info->digits_count = digit_count;

    unsigned digit_placement_offset = digit_dimensions[size].w +
                                      digit_spacing[size];

    for (unsigned i = 0;
         i < digit_count;
         ++i) {
        info->digits[i] =
            digit_layer_create(size,
                               GPoint(digit_placement_offset * i,
                                      0));
        layer_add_child(result,
                        info->digits[i]);
    }

    return result;
}

void
number_layer_set_number(NumberLayer* layer,
                        unsigned number,
                        bool animate)
{
    number_info_t* info = (number_info_t*) layer_get_data(layer);

    if (info->displayed_number == number) {
        return;
    }

    info->displayed_number = number;

    unsigned digit_index = info->digits_count;

    for (unsigned i = 0;
         i < info->digits_count;
         ++i) {
        unsigned digit_value = number % 10;
        number /= 10;
        digit_layer_set_number(info->digits[--digit_index],
                               digit_value,
                               animate);
    }
}

void
number_layer_kill_anim(NumberLayer* layer)
{
    number_info_t* info = (number_info_t*) layer_get_data(layer);

    for (unsigned i = 0;
         i < info->digits_count;
         ++i) {
        digit_layer_kill_anim(info->digits[i]);
    }
}

void
number_layer_set_quick_wrap(NumberLayer* layer,
                            bool quick_wrap)
{
    number_info_t* info = (number_info_t*) layer_get_data(layer);

    for (unsigned i = 0;
         i < info->digits_count;
         ++i) {
        digit_layer_set_quick_wrap(info->digits[i],
                                   quick_wrap);
    }
}

void
number_layer_set_animate_speed(NumberLayer* layer,
                               animation_speed_t speed)
{
    number_info_t* info = (number_info_t*) layer_get_data(layer);

    for (unsigned i = 0;
         i < info->digits_count;
         ++i) {
        digit_layer_set_animate_speed(info->digits[i],
                                      speed);
    }
}

bool
number_layer_animate(NumberLayer* layer)
{
    number_info_t* info = (number_info_t*) layer_get_data(layer);

    bool need_animate = false;

    for (unsigned i = 0;
         i < info->digits_count;
         ++i) {
        need_animate |= digit_layer_animate(info->digits[i]);
    }

    return need_animate;
}

void
number_layer_destroy(NumberLayer* layer)
{
    number_info_t* info = (number_info_t*) layer_get_data(layer);

    for (unsigned i = 0;
         i < info->digits_count;
         ++i) {
        digit_layer_destroy(info->digits[i]);
    }

    layer_destroy(layer);
}

