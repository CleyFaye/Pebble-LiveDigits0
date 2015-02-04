#ifndef INCL_DIGITLAYER_H
#define INCL_DIGITLAYER_H

#include <pebble.h>
#include "images.h"
    
typedef Layer DigitLayer;

DigitLayer* digit_layer_create(digit_size_t size, GPoint offset);
void digit_layer_set_number(DigitLayer* layer, int target_number, bool animate);
bool digit_layer_need_animation(DigitLayer* layer);
void digit_layer_animate(DigitLayer* layer);
void digit_layer_destroy(DigitLayer* layer);

#endif
