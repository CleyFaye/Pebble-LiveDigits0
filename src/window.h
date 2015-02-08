#ifndef INCL_WINDOW_H
#define INCL_WINDOW_H

#include <pebble.h>

typedef Window MainWindow;
    
MainWindow* main_window_create(void);
void main_window_destroy(MainWindow* window);
void main_window_update_config(void);
    
#endif
    