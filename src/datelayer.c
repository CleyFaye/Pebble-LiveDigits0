/** @file
 * Date digit
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// struct Layer
// TextLayer
// layer_create_with_data()
// layer_destroy()
#include <pebble.h>
// Date widget configuration
#include "config.h"
#include "layout.h"
// Associated header
#include "datelayer.h"

// =======
// TYPES =
// =======

typedef struct {
    TextLayer* day_text;
    TextLayer* month_text;
    char day_str[3];
    char month_str[3];
} date_info_t;

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

/** Return the date_info_t associated with a layer */
static
date_info_t*
get_info(DateLayer* layer);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
date_info_t*
get_info(DateLayer* layer)
{
    return (date_info_t*) layer_get_data(layer);
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

DateLayer*
date_layer_create(void)
{
    GRect layer_rect;
    layer_rect.origin = layout_get_widget_offset(WT_DATE);
    layer_rect.size = GSize(widget_size,
                            widget_size);
    DateLayer* result =
        layer_create_with_data(layer_rect,
                               sizeof(date_info_t));
    date_info_t* info = get_info(result);
    info->day_text = NULL;
    info->month_text = NULL;

    static const int small_font_offset = 10;
    static const int big_font_offset = 9;
    GRect top_rect = GRect(0,
                           0 - small_font_offset,
                           widget_size,
                           widget_size / 2 + small_font_offset);
    GRect bottom_rect = GRect(0,
                              widget_size / 2 - small_font_offset,
                              widget_size,
                              widget_size / 2 + small_font_offset);
    GRect big_rect = GRect(0,
                           7 - big_font_offset,
                           widget_size,
                           21 + big_font_offset);

    switch (cfg_get_date_style()) {
    default:
    case DATE_STYLE_DAYFIRST:
        info->day_text = text_layer_create(top_rect);
        info->month_text = text_layer_create(bottom_rect);
        text_layer_set_font(info->day_text,
                            fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        text_layer_set_font(info->month_text,
                            fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        break;

    case DATE_STYLE_DAYONLY:
        info->day_text = text_layer_create(big_rect);
        text_layer_set_font(info->day_text,
                            fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
        break;

    case DATE_STYLE_MONFIRST:
        info->day_text = text_layer_create(bottom_rect);
        info->month_text = text_layer_create(top_rect);
        text_layer_set_font(info->day_text,
                            fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        text_layer_set_font(info->month_text,
                            fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        break;

    case DATE_STYLE_MONONLY:
        info->month_text = text_layer_create(big_rect);
        text_layer_set_font(info->day_text,
                            fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
    }

    if (info->day_text) {
        text_layer_set_background_color(info->day_text,
                                        GColorClear);
        text_layer_set_text_color(info->day_text,
                                  GColorWhite);
        text_layer_set_text_alignment(info->day_text,
                                      GTextAlignmentRight);
        layer_add_child(result,
                        text_layer_get_layer(info->day_text));
    }

    if (info->month_text) {
        text_layer_set_background_color(info->month_text,
                                        GColorClear);
        text_layer_set_text_color(info->month_text,
                                  GColorWhite);
        text_layer_set_text_alignment(info->month_text,
                                      GTextAlignmentRight);
        layer_add_child(result,
                        text_layer_get_layer(info->month_text));
    }

    return result;
}

void
date_layer_set_date(DateLayer* layer,
                    unsigned day,
                    unsigned month)
{
    date_info_t* info = get_info(layer);

    if (info->day_text) {
        snprintf(info->day_str,
                 3,
                 "%u",
                 day);
        text_layer_set_text(info->day_text,
                            info->day_str);
    }

    if (info->month_text) {
        snprintf(info->month_str,
                 3,
                 "%u",
                 month);
        text_layer_set_text(info->month_text,
                            info->month_str);
    }
}

void
date_layer_destroy(DateLayer* layer)
{
    if (!layer) {
        return;
    }

    date_info_t* info = get_info(layer);

    if (info->day_text) {
        text_layer_destroy(info->day_text);
    }

    if (info->month_text) {
        text_layer_destroy(info->month_text);
    }

    layer_destroy(layer);
}

