#ifndef INCL_ANIM_H
#define INCL_ANIM_H

#include <pebble.h>
#include "images.h"

/** List of animations (segment to segment):
 * 1 = 1 -> 0
 * 2 = 4 -> 6
 * 3 = 0 -> 2
 * 4 = 6 -> 5
 * 5 = 2 -> 3
 * 6 = 3 -> 4
 * 7 = 1 -> 3
 * Each segment animation have 9 steps and include the segment orientation and offset for both big and small digits.
 */

typedef struct {
    int segment_angle;
    GPoint offset;
} animated_segment_info_t;

typedef struct {
    bool enabled[7];
} animation_fixed_digits_t;

typedef struct {
    int segment_anim[2];
} animation_digit_segment_t;

typedef struct digit_animation* digit_animation_t;

const animated_segment_info_t* anim_segment_get(int anim_id, int anim_pos, digit_size_t digit_size);

/**
 * Each animation have 9 frames
 * Digit anim id hold two informations: which digits are fixed, and which digits should move.
 * 0 mean no animation (and a bug in the final version!)
 * 1: 0->1, first part
 * 2: 0->1, second part
 * 3: 1->2, first part
 * 4: 1->2, second part
 * 5: 2->3, first part
 * 6: 2->3, second part
 * 7: 3->3
 * 8: 4->5
 * 9: 5->6
 * 10: 6->7, first part
 * 11: 6->7, second part
 * 12: 7->8, first part
 * 13: 7->8, second part
 * 14: 8->9
 * 15: 9->0
 * -1: digit 0
 * -2: digit 1
 * etc.
 *
 * Negative digit_anim_id mean the static digits, and stop the animation loop.
 * @param digit_anim_id The digit anim id.
 */
const animation_fixed_digits_t* anim_get_fixed_digits(int digit_anim_id);

const animation_digit_segment_t* anim_get_segment_anim(int digit_anim_id);

int anim_get_next_anim(int digit_anim_id);

#endif
    