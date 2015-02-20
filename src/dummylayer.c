/** @file
 * A dummy placeholder for widgets
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// GRect
// GPointZero
// GSize
// struct Layer
// TextLayer
// layer_create_with_data()
// layer_destroy()
// text_layer_create()
// text_layer_get_layer()
// text_layer_set_text_alignment()
// text_layer_set_text()
// text_layer_destroy()
// GTextAlignmentCenter
// layer_add_child()
// snprintf()
#include <pebble.h>
// widget_size
// layout_get_widget_offset()
#include "layout.h"
// Associated header
#include "dummylayer.h"

// ===============
// PRIVATE TYPES =
// ===============

typedef struct {
    int line;
    TextLayer* text_layer;
    char msg[2];
} dummy_info_t;

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

/** Return the dummy_info_t associated with a layer */
static
dummy_info_t*
get_info(DummyLayer* layer);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
dummy_info_t*
get_info(DummyLayer* layer)
{
    return (dummy_info_t*) layer_get_data(layer);
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

DummyLayer*
dummy_layer_create(unsigned line)
{
    GRect layer_rect;
    layer_rect.origin = layout_get_widget_offset(line,
                        GPointZero);
    layer_rect.size = GSize(widget_size,
                            widget_size);
    DummyLayer* result = layer_create_with_data(layer_rect,
                         sizeof(dummy_info_t));
    dummy_info_t* info = get_info(result);
    info->line = line;
    GRect frame;
    frame.origin = GPointZero;
    frame.size = layer_rect.size;
    info->text_layer = text_layer_create(frame);
    layer_add_child(result,
                    text_layer_get_layer(info->text_layer));
    text_layer_set_text_alignment(info->text_layer,
                                  GTextAlignmentCenter);
    snprintf(info->msg, 2, "%u", line);
    text_layer_set_text(info->text_layer,
                        info->msg);
    return result;
}

void
dummy_layer_destroy(DummyLayer* layer)
{
    dummy_info_t* info = get_info(layer);
    text_layer_destroy(info->text_layer);
    layer_destroy(layer);
}

