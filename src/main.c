#include <pebble.h>
#include "config.h"
#include "mainwindow.h"

static MainWindow* main_window;

static void init(void);
static void clear(void);

static void init(void)
{
    srand(time(NULL));
    main_window = main_window_create();
    cfg_init((config_callback_t) main_window_update_settings, 
            main_window);
    window_stack_push(main_window, true);
}

static void clear(void)
{
    main_window_destroy(main_window);
    cfg_clear();
}

int main(void)
{
    init();
    app_event_loop();
    clear();
}

