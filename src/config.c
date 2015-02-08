#include <pebble.h>
#include "config.h"
#include "window.h"

#define ANIM_ON_DISPLAY_KEY 1
#define ANIM_ON_SHAKE_KEY 2
#define WHITE_BACKGROUND_KEY 3
#define QUICK_WRAP_KEY 3

static const char* ON_VALUE = "on";
static const char* OFF_VALUE = "off";
    
static AppSync sync;
static uint8_t sync_buffer[64];

static bool animate_on_display = false;
static bool animate_on_shake = false;
static bool white_background = false;
static bool quick_wrap = true;

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context);
static const char* bool_to_str(bool value);

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context)
{
    switch (key) {
        case ANIM_ON_DISPLAY_KEY:
        animate_on_display = strcmp(new_tuple->value->cstring, ON_VALUE) == 0;
        break;
        case ANIM_ON_SHAKE_KEY:
        animate_on_shake = strcmp(new_tuple->value->cstring, ON_VALUE) == 0;
        break;
        case WHITE_BACKGROUND_KEY:
        white_background = strcmp(new_tuple->value->cstring, ON_VALUE) == 0;
        break;
    }
    main_window_update_config();
}

static const char* bool_to_str(bool value)
{
    return value ? ON_VALUE : OFF_VALUE;
}

void config_init(void)
{
    animate_on_display = persist_read_bool(ANIM_ON_DISPLAY_KEY);
    animate_on_shake = persist_read_bool(ANIM_ON_SHAKE_KEY);
    white_background = persist_read_bool(WHITE_BACKGROUND_KEY);
    app_message_open(64, 16);
    Tuplet initial_values[] = {
        TupletCString(ANIM_ON_DISPLAY_KEY, bool_to_str(animate_on_display)),
        TupletCString(ANIM_ON_SHAKE_KEY, bool_to_str(animate_on_shake)),
        TupletCString(WHITE_BACKGROUND_KEY, bool_to_str(white_background))
    };
    app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                 sync_tuple_changed_callback, NULL, NULL);
}

void config_clear(void)
{
    persist_write_bool(ANIM_ON_DISPLAY_KEY, animate_on_display);
    persist_write_bool(ANIM_ON_SHAKE_KEY, animate_on_shake);
    persist_write_bool(WHITE_BACKGROUND_KEY, white_background);
    app_sync_deinit(&sync);
}
    
bool config_get_animate_on_display(void)
{
    return animate_on_display;
}

void config_set_animate_on_display(bool animate)
{
    animate_on_display = animate;
}

bool config_get_animate_on_shake(void)
{
    return animate_on_shake;
}

void config_set_animate_on_shake(bool animate)
{
    animate_on_shake = animate;
}

bool config_get_white_background(void)
{
    return white_background;
}

void config_set_white_background(bool white_bg)
{
    white_background = white_bg;
}

bool config_get_quick_wrap(void)
{
    return quick_wrap;
}

void config_set_quick_wrap(bool value)
{
    quick_wrap = value;
}
