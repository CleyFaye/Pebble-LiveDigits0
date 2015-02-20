/** @file
 * Main watchface window
 *
 * Display the watchface.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_WINDOW_H
#define INCL_WINDOW_H

// Window
#include <pebble.h>

// =======
// TYPES =
// =======

typedef Window MainWindow;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Create the watchface main window. */
MainWindow*
main_window_create(void);

/** Destroy the watchface main window. */
void
main_window_destroy(MainWindow* window);

/** Update the window layout with user settings. */
void
main_window_update_settings(MainWindow* window);

#endif

