#ifndef INCL_CONFIG_H
#define INCL_CONFIG_H

#include <stdbool.h>

void config_init(void);
void config_clear(void);
    
bool config_get_animate_on_display(void);
void config_set_animate_on_display(bool animate);

bool config_get_animate_on_shake(void);
void config_set_animate_on_shake(bool animate);

bool config_get_white_background(void);
void config_set_white_background(bool white_background);
    
bool config_get_quick_wrap(void);
void config_set_quick_wrap(bool value);
    
#endif
    