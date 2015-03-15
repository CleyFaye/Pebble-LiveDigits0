/** @file
 * Application entry point
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "config.h"
#include "monobitmaps/monobitmap.h"
#include "mainwindow.h"

// DEBUG
//#include "debug.h"

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

static void init(MainWindow** main_window);
static void clear(MainWindow* main_window);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static inline
void init(MainWindow** main_window)
{
    srand(time(NULL));

    MainWindow* window;
    window = main_window_create();
    cfg_init((config_callback_t) main_window_update_settings,
             window);
    window_stack_push(window,
                      true);

    *main_window = window;
}

static void clear(MainWindow* main_window)
{
    main_window_destroy(main_window);
    cfg_clear();
}

// ======
// MAIN =
// ======

static inline
void main_real(void)
{
    MainWindow* main_window;

    init(&main_window);
    app_event_loop();
    clear(main_window);
    monobitmap_commit_destroy();
}


int main(void)
{
    main_real();
}

