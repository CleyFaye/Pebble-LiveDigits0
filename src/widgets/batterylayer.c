/** @file
 * Display the battery state.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "layout.h"
#include "utils.h"

#include "batterylayer.h"

// ===============
// PRIVATE TYPES =
// ===============

typedef struct {
    GBitmap* batt_plug;
    GBitmap* batt_border;
    GFont* font;
    unsigned last_state;
    bool loading;

    char text[3];
} battery_info_t;

// ================
// PRIVATE CONSTS =
// ================

static const unsigned plug_width = 10;
static const unsigned plug_height = 16;

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

static inline
battery_info_t* get_info(BatteryLayer* layer)
{
    return (battery_info_t*) layer_get_data(layer);
}

static
void
info_init(battery_info_t* info);

static
void
handle_layer_update(Layer* layer,
                    GContext* ctx);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void
info_init(battery_info_t* info)
{
    info->batt_plug = NULL;
    info->batt_border = NULL;
    info->font = NULL;
    info->text[0] = '\0';
    info->last_state = 101;
    info->loading = false;
}

static
void
handle_layer_update(Layer* layer,
                    GContext* ctx)
{
    battery_info_t* info = get_info(layer);

    if (cfg_get_battery_style() == BATTERY_STYLE_STATE) {
        // Display progress icons
        graphics_draw_bitmap_in_rect(ctx,
                                     info->batt_border,
                                     GRect(19, 10,
                                           10, 17));

        graphics_context_set_fill_color(ctx,
                                        GColorWhite);

        for (unsigned buchette = 0;
             buchette < info->last_state && buchette < 4;
             ++buchette) {
            graphics_fill_rect(ctx,
                               GRect(21, 22 - (buchette * 4),
                                     6, 3),
                               0,
                               GCornerNone);
        }

        if (info->loading) {
            graphics_draw_bitmap_in_rect(ctx,
                                         info->batt_plug,
                                         GRect(5, 11,
                                               plug_width, plug_height));
        }
    } else {
        // Display text percent
        graphics_draw_bitmap_in_rect(ctx,
                                     info->batt_border,
                                     GRect(1, 9,
                                           23, 17));

        const unsigned text_vertical_offset = 7;
        const unsigned layer_x_position = 4;
        const unsigned layer_y_position = 12;

        graphics_context_set_text_color(ctx,
                                        GColorWhite);
        graphics_draw_text(ctx,
                           info->text,
                           info->font,
                           GRect(layer_x_position,
                                 layer_y_position - text_vertical_offset,
                                 20,
                                 11 + text_vertical_offset),
                           GTextOverflowModeWordWrap,
                           GTextAlignmentLeft,
                           NULL);

        if (info->loading) {
            graphics_draw_bitmap_in_rect(ctx,
                                         info->batt_plug,
                                         GRect(25, 9,
                                               plug_width, plug_height));
        }
    }
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

BatteryLayer*
battery_layer_create(void)
{
    if (!layout_widget_is_active(WT_BATTERY)) {
        return NULL;
    }

    BatteryLayer* result =
        layer_create_with_init_data(layout_get_widget_area(WT_BATTERY),
                                    sizeof(battery_info_t),
                                    (layer_data_init_t) info_init);
    layer_set_update_proc(result,
                          handle_layer_update);
    battery_info_t* info = get_info(result);

    info->batt_plug =
        gbitmap_create_with_resource(RESOURCE_ID_BATT_PLUG);

    if (cfg_get_battery_style() == BATTERY_STYLE_STATE) {
        info->batt_border =
            gbitmap_create_with_resource(RESOURCE_ID_BATT_CONTOUR);
    } else {
        info->font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
        info->batt_border =
            gbitmap_create_with_resource(RESOURCE_ID_BATT_TEXTIMG);
    }

    BatteryChargeState batt_state = battery_state_service_peek();
    battery_layer_set_state(result,
                            batt_state.is_plugged,
                            batt_state.charge_percent);

    return result;
}

void
battery_layer_set_state(BatteryLayer* layer,
                        bool charging,
                        uint8_t charge_percent)
{
    battery_info_t* info = get_info(layer);
    bool need_update;

    if (cfg_get_battery_style() == BATTERY_STYLE_STATE) {
        unsigned new_state;

        if (charge_percent >= 65) {
            new_state = 3;
        } else if (charge_percent >= 35) {
            new_state = 2;
        } else if (charge_percent >= 10) {
            new_state = 1;
        } else {
            new_state = 0;
        }

        if (info->last_state == new_state) {
            need_update = false;
        } else {
            need_update = true;
            info->last_state = new_state;
        }
    } else {
        if (charge_percent >= 100) {
            charge_percent = 99;
        }

        if (info->last_state == charge_percent) {
            need_update = false;
        } else {
            need_update = true;
            info->last_state = charge_percent;
            snprintf(info->text,
                     3,
                     "%u",
                     info->last_state);
        }
    }

    if (info->loading != charging) {
        need_update = true;
        info->loading = charging;
    }

    if (need_update) {
        layer_mark_dirty(layer);
    }
}

void
battery_layer_destroy(BatteryLayer* layer)
{
    battery_info_t* info = get_info(layer);

    gbitmap_destroy(info->batt_plug);

    if (info->batt_border) {
        gbitmap_destroy(info->batt_border);
    }

    layer_destroy(layer);
}

