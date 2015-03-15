/** @đile
 * Implementation of NumberLayer layer.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "utils.h"
#include "digit_info.h"
#include "digitlayer.h"

#include "numberlayer.h"

// ===============
// PRIVATE TYPES =
// ===============

/** State of a number layer. */
typedef struct {
    unsigned digits_count;
    /** Target number to display */
    unsigned displayed_number;
    DigitLayer* digits[];
} number_info_t;

// ===============================
// PRIVATE FUNCTION DECLARATIONS =
// ===============================

static inline
number_info_t*
get_info(NumberLayer* layer)
{
    return (number_info_t*) layer_get_data(layer);
}

static
void
info_init(number_info_t* info);

// ==============================
// PRIVATE FUNCTION DEFINITIONS =
// ==============================

static
void
info_init(number_info_t* info)
{
    info->digits_count = 0;
    info->displayed_number = 0;
}

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
        layer_create_with_init_data(layer_rect,
                                    number_info_size,
                                    (layer_data_init_t) info_init);
    number_info_t* info = get_info(result);
    info->digits_count = digit_count;

    unsigned digit_placement_offset = digit_dimensions[size].w +
                                      digit_spacing[size];

    for (unsigned i = 0;
         i < digit_count;
         ++i) {
        info->digits[i] =
            digit_layer_create(size,
                               GPoint(digit_placement_offset * i,
                                      0),
                               MONO_RED,
                               MONO_BLACK);
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
    number_info_t* info = get_info(layer);

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
    number_info_t* info = get_info(layer);

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
    number_info_t* info = get_info(layer);

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
    number_info_t* info = get_info(layer);

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
    number_info_t* info = get_info(layer);

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
    number_info_t* info = get_info(layer);

    for (unsigned i = 0;
         i < info->digits_count;
         ++i) {
        digit_layer_destroy(info->digits[i]);
    }

    layer_destroy(layer);
}

