/** @file
 * Control digit's segments animation.
 *
 * Animations informations tells what digits should be drawn, and the position
 * of each segments.
 *
 * An "animation" is identified by a numeric id. Each animation is made of 9
 * steps, and some animations are marked as either "solo" or "double". Double
 * animations mean that two animations steps are needed to move from one digit
 * to another, and are sped up in "merge" animation speed.
 *
 * To summarize:
 * A digit animation = [fixed segments, two animated segments, next animation]
 * An animated segments = 9 steps of [position, orientation]
 * To animate, first get through the 9 steps of an animation, placing the
 * segments as indicated, then move to the next animation.
 *
 * Segments images are provided by digit_images.h
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DIGIT_ANIM_H
#define INCL_DIGIT_ANIM_H

// GPoint
// bool
#include <pebble.h>
// segment_orientation_t
#include "digit_info.h"

// =======
// TYPES =
// =======

/** A single segment animation.
 * Numbers are relative to the explanation in digit_info.h header.
 */
typedef enum {
    SA_3_TO_1 = -7,
    SA_4_TO_3,
    SA_3_TO_2,
    SA_5_TO_6,
    SA_2_TO_0,
    SA_6_TO_4,
    SA_0_TO_1,
    SA_NOANIM,
    SA_1_TO_0,
    SA_4_TO_6,
    SA_0_TO_2,
    SA_6_TO_5,
    SA_2_TO_3,
    SA_3_TO_4,
    SA_1_TO_3
} segment_anim_t;

/** A digit animation step.
 *
 * Steps are named after the number they transition from/to. Animations in two
 * steps are suffixed with _a and _b.
 * Full digits are treated as "special" animations, and all have negative
 * value, indicating that the animation can stop here.
 */
typedef enum {
    DA_9 = -10,
    DA_8,
    DA_7,
    DA_6,
    DA_5,
    DA_4,
    DA_3,
    DA_2,
    DA_1,
    DA_0,
    DA_0_TO_1_a = 1,
    DA_0_TO_1_b,
    DA_1_TO_2_a,
    DA_1_TO_2_b,
    DA_2_TO_3_a,
    DA_2_TO_3_b,
    DA_3_TO_4,
    DA_4_TO_5,
    DA_5_TO_6,
    DA_6_TO_7_a,
    DA_6_TO_7_b,
    DA_7_TO_8_a,
    DA_7_TO_8_b,
    DA_8_TO_9,
    DA_9_TO_0,
    DA_1_TO_0_a,
    DA_1_TO_0_b,
    DA_2_TO_0,
    DA_3_TO_0,
    DA_4_TO_0_a,
    DA_4_TO_0_b,
    DA_5_TO_0,
    DA_6_TO_0,
    DA_7_TO_0_a,
    DA_7_TO_0_b,
    DA_8_TO_0
} digit_anim_t;

/** Represent the position of a segment during an animation.
 *
 * This is returned by anim_segment_get().
 */
typedef struct {
    /** The segment orientation. */
    segment_orientation_t segment_angle;
    /** The offset of the segment's image. */
    GPoint offset;
} animated_segment_info_t;

/** Indicate which segment should be displayed statically during an animation
 * step. */
typedef struct {
    bool enabled[7];
} animation_fixed_digits_t;

/** Represent which segments animation must be displayed.
 * Each animation step can have at most two segment animations.
 * These values are passed back to anim_segment_get() to get the actual segment
 * placement.
 */
typedef struct {
    /** Animation identifier, suitable for anim_segment_get() */
    segment_anim_t segment_anim[2];
} animation_digit_segment_t;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Return the placement of a given segment.
 *
 * This function return the placement and orientation of a segment for a given
 * segment animation id and animation step.
 *
 * From a technical point of view, negative animation id are just positive ones
 * reversed.
 *
 * @param anim_id The segment animation id
 * @param anim_pos The animation position. From 0 to 9.
 * @param digit_size The size of the digit.
 * @return The animated segment position and orientation, or NULL if a parameter
 * is invalid.
 */
const animated_segment_info_t* anim_segment_get(segment_anim_t segment_anim,
        int anim_pos,
        digit_size_t digit_size);

/** Return the segments that must be drawn at a fixed position for a given
 * animation step.
 *
 * @param digit_anim The digit anim id.
 * @return The fixed segments.
 */
const animation_fixed_digits_t* anim_get_fixed_digits(digit_anim_t digit_anim);

/** Return the animated segments from a given animation step.
 *
 * @param digit_anim The digit anim id.
 * @return The animated segments.
 */
const animation_digit_segment_t* anim_get_segment_anim(digit_anim_t digit_anim);

/** Return the next animation in a sequence.
 * The next animation is the animation step that must follow the given step.
 *
 * @param digit_anim The digit anim id.
 * @return The next animation.
 */
digit_anim_t anim_get_next_anim(digit_anim_t digit_anim);

/** Return the next animation to 0.
 * The next animation to 0 is the shortest animation path that return from the
 * given animation step to 0.
 *
 * @param digit_anim The digit anim id.
 * @return The next animation to use as a shortcut to 0.
 */
digit_anim_t anim_get_next_quick_anim(digit_anim_t digit_anim);

/** Return true if the given animation step is a static digit.
 *
 * Static digit are negative anim id.
 *
 * @param digit_anim The digit animation id to check.
 * @return true if it is a static digit.
 */
bool anim_is_static_digit(digit_anim_t digit_anim);

/** Return the number actually displayed by a given animation step.
 *
 * Transition steps will return -1, while static digits will return the
 * appropriate number.
 *
 * @param digit_anim The digit animation id
 * @return The number displayed, or -1 if it is a transition step.
 */
int anim_get_displayed_number(digit_anim_t digit_anim);

/** Return the static digit animation step for a given number.
 *
 * @param number The requested number
 * @return The digit animation step
 */
digit_anim_t anim_get_anim_for_number(int number);

#endif

