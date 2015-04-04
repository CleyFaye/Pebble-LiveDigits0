/** @file
 * Control digit's segments animation.
 *
 * Animations informations tells what segments should be drawn, and the position
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
 * To animate, first get through the 9 steps of a segment animation, placing the
 * segments as indicated, then move to the next animation step of the digit.
 *
 * Segments images are provided by digit_images
 * Actual animation logic with different speed is handled in digitlayer
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DIGIT_ANIM_H
#define INCL_DIGIT_ANIM_H

#include <pebble.h>

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
 * Full digits are treated as "special" animations. They animate no digit but
 * provide the list of segments that must be displayed for a given number.
 */
typedef enum {
    DA_9 = -10, DA_8, DA_7, DA_6, DA_5, DA_4, DA_3, DA_2, DA_1, DA_0,
    DA_0_TO_1_a = 1, DA_0_TO_1_b,
    DA_1_TO_2_a, DA_1_TO_2_b,
    DA_2_TO_3_a, DA_2_TO_3_b,
    DA_3_TO_4,
    DA_4_TO_5,
    DA_5_TO_6,
    DA_6_TO_7_a, DA_6_TO_7_b,
    DA_7_TO_8_a, DA_7_TO_8_b,
    DA_8_TO_9,
    DA_9_TO_0,
    DA_1_TO_0_a, DA_1_TO_0_b,
    DA_2_TO_0,
    DA_3_TO_0,
    DA_4_TO_0_a, DA_4_TO_0_b,
    DA_5_TO_0,
    DA_6_TO_0,
    DA_7_TO_0_a, DA_7_TO_0_b,
    DA_8_TO_0
} digit_anim_t;

/** Indicate which segment should be displayed statically during an animation
 * step.
 * LSB is segment 0.
 */
typedef uint8_t digit_fixed_segments_t;

/** Represent which segments animation must be displayed.
 * Each animation step can have at most two segment animations.
 * These values are passed back to anim_segment_get() to get the actual segment
 * placement.
 */
typedef segment_anim_t digit_segment_animation_t[2];

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Return the placement of a given segment.
 *
 * This function return the placement and orientation of a segment for a given
 * segment animation and animation step.
 *
 * From a technical point of view, negative animation id are just positive ones
 * reversed. It is handled transparently with the appropriate enum values.
 *
 * @param anim_pos The animation position. From 0 to 9.
 */
int32_t
anim_segment_get(segment_anim_t segment_anim,
                 int anim_pos,
                 digit_size_t digit_size,
                 GPoint* offset);

/** Return the segments that must be drawn at a fixed position for a given
 * animation step.
 */
digit_fixed_segments_t
anim_get_fixed_segments(digit_anim_t digit_anim);

/** Return true if the given segment is on */
inline
bool
anim_get_fixed_segment_state(digit_fixed_segments_t segments,
                             unsigned segment_id)
{
    return (segments & (1u << segment_id)) != 0;
}

/** Return the animated segments from a given animation step. */
const digit_segment_animation_t*
anim_get_segment_anim(digit_anim_t digit_anim);

/** Return the next animation in a sequence.
 * The next animation is the animation step that must follow the given step.
 *
 * @param digit_anim The current animation step
 */
digit_anim_t
anim_get_next_anim(digit_anim_t digit_anim);

/** Return the next animation to 0.
 * The next animation to 0 is the shortest animation path that return from the
 * given animation step to 0.
 *
 * @param digit_anim The current animation step
 */
digit_anim_t
anim_get_next_quick_anim(digit_anim_t digit_anim);

/** Return true if the given animation step is a static digit. */
inline
bool
anim_is_static_digit(digit_anim_t digit_anim)
{
    return digit_anim < 0;
}

/** Return true if the given animation step is part of a two-step transition.
 *
 * This is relevant for "merged" animations speed. If a whole transition needs
 * two animation step, half of their frames are skipped to merge them into one.
 */
bool
anim_is_multipart(digit_anim_t digit_anim);

/** Determine if an animation step is complete. */
inline
bool
anim_is_complete(digit_anim_t digit_anim,
                 int step)
{
    return (digit_anim < 0)
           ? true
           : (step >= 9);
}

/** Return the number actually displayed by a given animation step.
 *
 * Transition steps will return -1, while static digits will return the
 * appropriate number.
 */
inline
int
anim_get_displayed_number(digit_anim_t digit_anim)
{
    return (digit_anim < 0)
           ? (-digit_anim - 1)
           : -1;
}

/** Return the number of steps of an animation step.
 *
 * @return 9 for animation step, 0, for static digit.
 */
inline
int
anim_get_step_count(digit_anim_t digit_anim)
{
    return (digit_anim < 0)
           ? 0
           : 9;
}

/** Return the static digit animation step for a given number. */
inline
digit_anim_t
anim_get_anim_for_number(int number)
{
    return -number - 1;
}

#endif

