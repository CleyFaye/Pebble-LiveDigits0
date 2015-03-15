/** @file
 * Bluetooth widget
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "config.h"
#include "layout.h"
#include "utils.h"
#include "monobitmaps/monobitmap.h"

#include "btlayer.h"

// ===============
// PRIVATE TYPES =
// ===============

typedef struct {
    MonoBitmap image_on;
    MonoBitmap image_off;
    bool bt_state;
    /** Flag set by the call to bt_layer_set_hidden() */
    bool required_hidden;
} bt_info_t;

// ================
// PRIVATE CONSTS =
// ================

/** Bluetooth state image offset */
static
const GPoint image_offset = {9, 6};

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

static inline
bt_info_t* get_info(BtLayer* layer)
{
    return (bt_info_t*) layer_get_data(layer);
}

static
void info_init(bt_info_t* info);

/** Return true if the watch must vibrate when the Bt is lost */
static inline
bool vibrate_on_lost(void)
{
    switch (cfg_get_bluetooth_style()) {
    case BLUETOOTH_STYLE_ONLYOFFBUZZ:
    case BLUETOOTH_STYLE_ONLYOFFFORCEBUZZ:
        return true;

    default:
        return false;
    }
}

/** If the widget must be kept visible when Bt is lost */
static inline
bool force_display_when_off(void)
{
    switch (cfg_get_bluetooth_style()) {
    case BLUETOOTH_STYLE_ONLYOFFFORCE:
    case BLUETOOTH_STYLE_ONLYOFFFORCEBUZZ:
        return true;

    default:
        return false;
    }
}

/** Update the effective visibility of the widget according the the current
 * Bluetooth state and user config. */
static
void
update_visibility(BtLayer* layer);

static
void
handle_layer_update(struct Layer* layer,
                    GContext* ctx);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
void info_init(bt_info_t* info)
{
    info->image_on = NULL;
    info->image_off = NULL;
    info->bt_state = false;
    info->required_hidden = false;
}

static
void
update_visibility(BtLayer* layer)
{
    bt_info_t* info = get_info(layer);

    if (!info->bt_state && force_display_when_off()) {
        // If bluetooth is lost and display is forced, ignore hide request
        layer_set_hidden(layer,
                         false);
    } else {
        layer_set_hidden(layer,
                         info->required_hidden);
    }
}

static
void
handle_layer_update(struct Layer* layer,
                    GContext* ctx)
{
    bt_info_t* info = get_info(layer);
    int style = cfg_get_bluetooth_style();

    if (info->bt_state) {
        switch (style) {
        default:
            break;

        case BLUETOOTH_STYLE_ALWAYS:
        case BLUETOOTH_STYLE_ONLYON:
            monobitmap_draw_bitmap(ctx,
                                   info->image_on,
                                   image_offset);
        }
    } else {
        switch (style) {
        default:
            break;

        case BLUETOOTH_STYLE_ALWAYS:
        case BLUETOOTH_STYLE_ONLYOFF:
        case BLUETOOTH_STYLE_ONLYOFFBUZZ:
        case BLUETOOTH_STYLE_ONLYOFFFORCE:
        case BLUETOOTH_STYLE_ONLYOFFFORCEBUZZ:
            monobitmap_draw_bitmap(ctx,
                                   info->image_off,
                                   image_offset);
        }
    }
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

BtLayer*
bt_layer_create(void)
{
    if (!layout_widget_is_active(WT_BLUETOOTH)) {
        return NULL;
    }

    BtLayer* result =
        layer_create_with_init_data(layout_get_widget_area(WT_BLUETOOTH),
                                    sizeof(bt_info_t),
                                    (layer_data_init_t) info_init);
    layer_set_update_proc(result,
                          handle_layer_update);
    bt_info_t* info = get_info(result);
    info->image_on = monobitmap_create_with_resource(RESOURCE_ID_BT_ON,
                     MONO_WHITE,
                     MONO_BLACK,
                     false,
                     false);
    info->image_off = monobitmap_create_with_resource(RESOURCE_ID_BT_OFF,
                      MONO_WHITE,
                      MONO_BLACK,
                      false,
                      false);
    info->bt_state = bluetooth_connection_service_peek();
    return result;
}

void
bt_layer_set_state(BtLayer* layer,
                   bool connected)
{
    bt_info_t* info = get_info(layer);

    if (info->bt_state == connected) {
        return;
    }

    if (!connected && vibrate_on_lost()) {
        // Bluetooth just lost
        if (vibrate_on_lost()) {
            vibes_long_pulse();
        }
    }

    info->bt_state = connected;
    update_visibility(layer);
    layer_mark_dirty(layer);
}

void
bt_layer_set_hidden(BtLayer* layer,
                    bool hidden)
{
    bt_info_t* info = get_info(layer);
    info->required_hidden = hidden;
    update_visibility(layer);
}

void
bt_layer_destroy(BtLayer* layer)
{
    bt_info_t* info = get_info(layer);
    monobitmap_destroy(info->image_on);
    monobitmap_destroy(info->image_off);
    layer_destroy(layer);
}

