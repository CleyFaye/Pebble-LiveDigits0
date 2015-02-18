/* AUTO-GENERATED HEADER DO NOT MODIFY */
#ifndef __INCL_CONFIG_H
#define __INCL_CONFIG_H

#include <pebble.h>


typedef void(*config_callback_t)(void* data);

bool cfg_get_invert_colors(void);
bool cfg_get_skip_digits(void);
bool cfg_get_anim_on_time(void);
bool cfg_get_anim_on_load(void);
bool cfg_get_anim_on_shake(void);

void cfg_init(config_callback_t callback, void* data);
void cfg_clear(void);

#endif
