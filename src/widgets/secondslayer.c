/** @file
 * Seconds digit
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "digits/numberlayer.h"
#include "digits/digit_info.h"
#include "layout.h"
#include "config.h"
#include "utils.h"

#include "secondslayer.h"

// =======
// TYPES =
// =======

/** Layer state */
typedef struct {
    NumberLayer* number_layer;
    // TODO dot-mode need to be redone for SDK3.0
    unsigned previous_seconds_value;
} seconds_info_t;

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

/** Initialize the layer data */
static
void
info_init(seconds_info_t* info);

/** Return the seconds_info_t associated with a layer */
static inline
seconds_info_t*
get_info(SecondsLayer* layer)
{
    return (seconds_info_t*) layer_get_data(layer);
}

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void
info_init(seconds_info_t* info)
{
    info->number_layer = NULL;
    info->previous_seconds_value = 0;
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

SecondsLayer*
seconds_layer_create(void)
{
    if (!layout_widget_is_active(WT_SECONDS)) {
        return NULL;
    }

    SecondsLayer* result =
        layer_create_with_init_data(layout_get_widget_area(WT_SECONDS),
                                    sizeof(seconds_info_t),
                                    (layer_data_init_t) info_init);
    seconds_info_t* info = get_info(result);

    if (cfg_get_seconds_style() == SECONDS_STYLE_DOT) {
        GRect dot_rect = GRect(widget_size / 2 - seconds_dot_size / 2,
                               widget_size / 2 - seconds_dot_size / 2,
                               seconds_dot_size,
                               seconds_dot_size);
    } else {
        info->number_layer = number_layer_create(DS_SMALL,
                             2,
                             GPoint(1, 2));
        number_layer_set_animate_speed(info->number_layer,
                                       FAST_MERGED);
        number_layer_set_quick_wrap(info->number_layer,
                                    true);
        layer_add_child(result,
                        info->number_layer);
    }

    return result;
}

bool
seconds_layer_animate(SecondsLayer* layer)
{
    if (layer_get_hidden(layer)) {
        return false;
    }

    seconds_info_t* info = get_info(layer);
    return info->number_layer
           ? number_layer_animate(info->number_layer)
           : false;
}

void
seconds_layer_set_time(SecondsLayer* layer,
                       unsigned seconds)
{
    seconds_info_t* info = get_info(layer);

    if (info->previous_seconds_value == seconds) {
        return;
    }

    info->previous_seconds_value = seconds;

    if (info->number_layer) {
        bool animate_seconds = cfg_get_seconds_style() != SECONDS_STYLE_FIXED;

        if (animate_seconds) {
            int prev_second = (seconds > 0)
                              ? (seconds - 1)
                              : 59;
            number_layer_set_number(info->number_layer,
                                    prev_second,
                                    false);
            number_layer_kill_anim(info->number_layer);
        }

        number_layer_set_number(info->number_layer,
                                seconds,
                                animate_seconds);
    }
}

void
seconds_layer_destroy(SecondsLayer* layer)
{
    seconds_info_t* info = get_info(layer);

    if (info->number_layer) {
        number_layer_destroy(info->number_layer);
    }

    layer_destroy(layer);
}

