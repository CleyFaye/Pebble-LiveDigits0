/** @file
 * Display the am/pm text
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "config.h"
#include "layout.h"
#include "digits/digit_info.h"
#include "utils.h"

#include "daytimelayer.h"

// ===============
// PRIVATE TYPES =
// ===============

typedef struct {
    TextLayer* text_layer;
    char text[3];
} daytime_info_t;

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

static inline
daytime_info_t* get_info(DayTimeLayer* layer)
{
    return (daytime_info_t*) layer_get_data(layer);
}

static
void
info_init(daytime_info_t* info);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void
info_init(daytime_info_t* info)
{
    info->text_layer = NULL;
    info->text[0] = '\0';
    info->text[1] = 'M';
    info->text[2] = '\0';
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

DayTimeLayer*
daytime_layer_create(void)
{
    if (!cfg_get_display_daytime() || clock_is_24h_style()) {
        return NULL;
    }

    GRect layer_rect;
    layer_rect.origin = layout_get_hour_offset();
    layer_rect.origin.x += 62;
    layer_rect.origin.y += 63;
    layer_rect.size = GSize(24, 9);
    DayTimeLayer* result =
        layer_create_with_init_data(layer_rect,
                                    sizeof(daytime_info_t),
                                    (layer_data_init_t) info_init);
    daytime_info_t* info = get_info(result);
    const int text_vertical_offset = 5;
    layer_rect.origin.x = 0;
    layer_rect.origin.y = -text_vertical_offset;
    layer_rect.size.h += text_vertical_offset;
    info->text_layer = text_layer_create(layer_rect);
    text_layer_set_font(info->text_layer,
                        fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
    text_layer_set_text_alignment(info->text_layer,
                                  GTextAlignmentCenter);
    text_layer_set_text_color(info->text_layer,
                              GColorWhite);
    text_layer_set_background_color(info->text_layer,
                                    GColorClear);
    layer_add_child(result,
                    text_layer_get_layer(info->text_layer));
    return result;
}

void
daytime_layer_set_time(DayTimeLayer* layer,
                       struct tm* tick_time)
{
    daytime_info_t* info = get_info(layer);
    info->text[0] = (tick_time->tm_hour < 12)
                    ? 'A'
                    : 'P';
    text_layer_set_text(info->text_layer,
                        info->text);
}

void
daytime_layer_destroy(DayTimeLayer* layer)
{
    daytime_info_t* info = get_info(layer);
    text_layer_destroy(info->text_layer);
    layer_destroy(layer);
}

