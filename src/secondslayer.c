/** @file
 * Seconds digit
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>
#include "numberlayer.h"
#include "widgetfilter.h"
#include "layout.h"
#include "digit_info.h"
#include "config.h"
#include "secondslayer.h"

// =======
// TYPES =
// =======

typedef struct {
    NumberLayer* number_layer;
    InverterLayer* inverter_layer;
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
static
seconds_info_t*
get_info(SecondsLayer* layer);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void
info_init(seconds_info_t* info)
{
    info->number_layer = NULL;
    info->inverter_layer = NULL;
    info->previous_seconds_value = 0;
}

static
seconds_info_t*
get_info(SecondsLayer* layer)
{
    return (seconds_info_t*) layer_get_data(layer);
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

SecondsLayer*
seconds_layer_create(void)
{
    if (!widget_is_active(WT_SECONDS)) {
        return NULL;
    }

    GRect layer_rect;
    layer_rect.origin = layout_get_widget_offset(WT_SECONDS);

    SecondsLayer* result = NULL;
    seconds_info_t* info = NULL;

    if (cfg_get_seconds_style() == SECONDS_STYLE_DOT) {
        layer_rect.size = GSize(seconds_dot_size,
                                seconds_dot_size);
        result = layer_create_with_data(layer_rect,
                                        sizeof(seconds_info_t));
        info = get_info(result);
        info_init(info);
        layer_rect.origin = GPointZero;
        info->inverter_layer = inverter_layer_create(layer_rect);
        layer_add_child(result,
                        inverter_layer_get_layer(info->inverter_layer));
    } else {
        layer_rect.size = digit_dimensions[DS_SMALL];
        layer_rect.size.w *= 2;
        layer_rect.size.w += 3; // Safe margin for the spacing between two digits
        result = layer_create_with_data(layer_rect,
                                        sizeof(seconds_info_t));
        info = get_info(result);
        info_init(info);
        info->number_layer = number_layer_create(DS_SMALL,
                             2,
                             GPointZero);
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
        }

        number_layer_set_number(info->number_layer,
                                seconds,
                                animate_seconds);
    } else if (info->inverter_layer) {
        Layer* dot_layer = inverter_layer_get_layer(info->inverter_layer);
        layer_set_hidden(dot_layer,
                         !layer_get_hidden(dot_layer));
    }
}

void
seconds_layer_destroy(SecondsLayer* layer)
{
    if (!layer) {
        return;
    }

    seconds_info_t* info = get_info(layer);

    if (info->number_layer) {
        number_layer_destroy(info->number_layer);
    } else if (info->inverter_layer) {
        inverter_layer_destroy(info->inverter_layer);
    }

    layer_destroy(layer);
}

