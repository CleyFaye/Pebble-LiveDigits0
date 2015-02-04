#include <pebble.h>
#include "images.h"
#include "window.h"

static MainWindow* main_window;

static void init(void);
static void clear(void);

static void init(void)
{
    load_images();
    main_window = main_window_create();
    window_stack_push(main_window, true);
}

static void clear(void)
{
    main_window_destroy(main_window);
    unload_images();
}

int main(void)
{
    init();
    app_event_loop();
    clear();
}