/** @file
 * Implementation of digit animation construction.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "digit_info.h"

#include "digit_anim.h"

// ===============
// PRIVATE TYPES =
// ===============

/** All info about a digit animation step. */
typedef struct {
    /** The segments to statically display (not animated) */
    digit_fixed_segments_t fixed_segments;
    /** The animated segments (up to two) */
    digit_segment_animation_t moving_segments;
    /** The next animation step in the sequence */
    digit_anim_t next_animation;
    /** The shortcut animation to 0 */
    digit_anim_t quick_next;
    /** If the animation is part of a two-step transition. */
    bool multipart;
} digit_anim_info_t;

/** All segment position when animated.
 *
 * There are 7 segments animations (illustrated by the values of
 * segment_anim_t). Each segments animations is made of 9 steps. Each of these
 * steps is a combination of an offset (this data type) and an orientation (in
 * animated_segment_orientation_t).
 */
typedef const GPoint animated_segment_offset_t[7 * 9];

/** All segment orientation when animated.
 *
 * See animated_segment_offset_t for details about the composition of this
 * array.
 */
typedef const segment_orientation_t animated_segment_orientation_t[7 * 9];

// ==============
// PRIVATE DATA =
// ==============

/** Segment orientation when animated.
 * These are shared between all digit's size.
 */
static
animated_segment_orientation_t animated_segment_orientation = {
    // SA_1_TO_0
    SO_171, SO_162, SO_153, SO_144, SO_135,
    SO_126, SO_117, SO_108, SO_99,
    // SA_4_TO_6
    SO_9,  SO_18, SO_27, SO_36, SO_45,
    SO_54, SO_63, SO_72, SO_81,
    // SA_0_TO_2
    SO_81, SO_72, SO_63, SO_54, SO_45,
    SO_36, SO_27, SO_18, SO_9,
    // SA_6_TO_5
    SO_99,  SO_108, SO_117, SO_126, SO_135,
    SO_144, SO_153, SO_162, SO_171,
    // SA_2_TO_3
    SO_171, SO_162, SO_153, SO_144, SO_135,
    SO_126, SO_117, SO_108, SO_99,
    // SA_3_TO_4
    SO_99,  SO_108, SO_117, SO_126, SO_135,
    SO_144, SO_153, SO_162, SO_171,
    // SA_1_TO_3
    SO_9,  SO_18, SO_27, SO_36, SO_45,
    SO_54, SO_63, SO_72, SO_81
};

/** Segment position when animated.
 * These are for the big, medium, and small digits.
 */
static
animated_segment_offset_t animated_segment_offsets[DIGITS_SIZE_COUNT] = {
    {
        // SA_1_TO_0
        {1, 6}, {2, 6}, {2, 5}, {4, 5}, {5, 5},
        {5, 3}, {6, 3}, {6, 2}, {6, 2},
        // SA_4_TO_6
        {1, 43}, {1, 45}, {2, 47}, {4, 50}, {5, 54},
        {5, 57}, {6, 60}, {6, 65}, {6, 70},
        // SA_0_TO_2
        {7, 1},  {7, 2},  {9, 3},  {13, 4}, {16, 5},
        {19, 6}, {24, 6}, {28, 6}, {32, 6},
        // SA_6_TO_5
        {6, 69},  {8, 65},  {10, 62}, {12, 57}, {16, 54},
        {19, 52}, {23, 47}, {28, 44}, {33, 44},
        // SA_2_TO_3
        {33, 7},  {28, 7},  {23, 10}, {19, 15}, {16, 17},
        {12, 20}, {10, 25}, {8, 28},  {6, 32},
        // SA_3_TO_4
        {6, 39}, {6, 39}, {6, 40}, {5, 40}, {5, 42},
        {4, 42}, {2, 42}, {2, 43}, {1, 43},
        // SA_1_TO_3
        {1, 6},  {1, 8},  {2, 10}, {4, 13}, {5, 17},
        {5, 20}, {6, 23}, {6, 28}, {6, 33}
    }, {
        // SA_1_TO_0
        {2, 5}, {3, 5}, {2, 5}, {5, 5}, {5, 3},
        {6, 3}, {6, 2}, {6, 2}, {6, 0},
        // SA_4_TO_6
        {0, 40}, {1, 42}, {3, 45}, {3, 47}, {4, 49},
        {5, 55}, {5, 57}, {5, 62}, {5, 66},
        // SA_0_TO_2
        {6, 1},  {6, 2},  {8, 2},  {12, 3}, {14, 4},
        {18, 4}, {23, 5}, {26, 5}, {31, 5},
        // SA_6_TO_5
        {5, 65},  {7, 62},  {9, 58},  {11, 54}, {15, 50},
        {18, 47}, {21, 45}, {27, 42}, {31, 41},
        // SA_2_TO_3
        {31, 6},  {27, 7}, {21, 10}, {18, 12}, {15, 15},
        {11, 19}, {9, 23}, {7, 27},  {5, 30},
        // SA_3_TO_4
        {6, 35}, {6, 37}, {6, 37}, {6, 38}, {5, 38},
        {5, 40}, {2, 40}, {3, 40}, {2, 40},
        // SA_1_TO_3
        {0, 5},  {1, 7},  {3, 10}, {3, 12}, {4, 14},
        {5, 20}, {5, 22}, {5, 27}, {5, 31},
    }, {
        // SA_1_TO_0
        {0, 3}, {1, 3}, {0, 3}, {2, 3}, {2, 3},
        {2, 2}, {2, 2}, {2, 0}, {2, 0},
        // SA_4_TO_6
        {1, 19}, {2, 18}, {1, 21}, {2, 21}, {2, 23},
        {2, 23}, {2, 25}, {2, 27}, {2, 29},
        // SA_0_TO_2
        {3, 0}, {3, 0}, {3, 1},  {5, 1}, {5, 2},
        {7, 2}, {8, 2}, {10, 3}, {11, 3},
        // SA_6_TO_5
        {3, 29}, {3, 27}, {3, 26}, {5, 24},  {5, 23},
        {7, 21}, {7, 21}, {9, 18}, {11, 18},
        // SA_2_TO_3
        {11, 3}, {9, 3},  {7, 6},  {7, 6},  {5, 8},
        {5, 9},  {3, 11}, {3, 12}, {3, 14},
        // SA_3_TO_4
        {2, 15}, {2, 15}, {2, 17}, {2, 17}, {2, 18},
        {2, 18}, {0, 18}, {1, 18}, {0, 18},
        // SA_1_TO_3
        {1, 4}, {2, 3},  {1, 6},  {2, 6}, {2, 8},
        {2, 8}, {2, 10}, {2, 12}, {2, 14}
    }
};

/** Digit animation steps.
 * Each value correspond to a positive value of digit_anim_t.
 */
static const digit_anim_info_t moving_anim[] = {
    // 1 - 0>1
    {0x65, {SA_1_TO_0, SA_4_TO_6}, DA_0_TO_1_b, DA_0_TO_1_b, true},
    // 2 - 0>1
    {0x24, {SA_0_TO_2, SA_6_TO_5}, DA_1, DA_1, true},
    // 3 - 1>2
    {0x04, {SA_2_TO_0, SA_5_TO_6}, DA_1_TO_2_b, DA_1_TO_2_b, true},
    // 4 - 1>2
    {0x45, {SA_2_TO_3, SA_6_TO_4}, DA_2, DA_2, true},
    // 5 - 2>3
    {0x4D, {SA_4_TO_6, SA_NOANIM}, DA_2_TO_3_b, DA_2_TO_3_b, true},
    // 6 - 2>3
    {0x4D, {SA_6_TO_5, SA_NOANIM}, DA_3, DA_3, true},
    // 7 - 3>4
    {0x2C, {SA_0_TO_1, SA_6_TO_5}, DA_4, DA_4, false},
    // 8 - 4>5
    {0x2A, {SA_2_TO_0, SA_5_TO_6}, DA_5, DA_5, false},
    // 9 - 5>6
    {0x6B, {SA_3_TO_4, SA_NOANIM}, DA_6, DA_6, false},
    // 10 - 6>7
    {0x63, {SA_3_TO_2, SA_4_TO_6}, DA_6_TO_7_b, DA_6_TO_7_b, true},
    // 11 - 6>7
    {0x25, {SA_1_TO_0, SA_6_TO_5}, DA_7, DA_7, true},
    // 12 - 7>8
    {0x25, {SA_0_TO_1, SA_5_TO_6}, DA_7_TO_8_b, DA_7_TO_8_b, true},
    // 13 - 7>8
    {0x67, {SA_1_TO_3, SA_6_TO_4}, DA_8, DA_8, true},
    // 14 - 8>9
    {0x6F, {SA_4_TO_3, SA_NOANIM}, DA_9, DA_9, false},
    // 15 - 9>0
    {0x67, {SA_3_TO_1, SA_6_TO_4}, DA_0, DA_0, false},
    // 16 - 1>0
    {0x24, {SA_2_TO_0, SA_5_TO_6}, DA_1_TO_0_b, DA_1_TO_0_b, true},
    // 17 - 1>0
    {0x65, {SA_0_TO_1, SA_6_TO_4}, DA_0, DA_0, true},
    // 18 - 2>0
    {0x55, {SA_3_TO_1, SA_6_TO_5}, DA_0, DA_0, false},
    // 19 - 3>0
    {0x65, {SA_3_TO_4, SA_0_TO_1}, DA_0, DA_0, false},
    // 20 - 4>0
    {0x26, {SA_3_TO_4, SA_NOANIM}, DA_4_TO_0_b, DA_4_TO_0_b, true},
    // 21 - 4>0
    {0x36, {SA_5_TO_6, SA_1_TO_0}, DA_0, DA_0, true},
    // 22 - 5>0
    {0x63, {SA_0_TO_2, SA_6_TO_4}, DA_0, DA_0, false},
    // 23 - 6>0
    {0x73, {SA_3_TO_2, SA_NOANIM}, DA_0, DA_0, false},
    // 24 - 7>0
    {0x25, {SA_5_TO_6, SA_0_TO_1}, DA_7_TO_0_b, DA_7_TO_0_b, true},
    // 25 - 7>0
    {0x67, {SA_6_TO_4, SA_NOANIM}, DA_0, DA_0, true},
    // 26 - 8>0
    {0x77, {SA_3_TO_4, SA_NOANIM}, DA_0, DA_0, false}
};

/** Digit animation steps.
 * These are for static digits, treated as animation because it's easier.
 *
 * Each value correspond to a negative value in digit_anim_t.
 */
static const digit_anim_info_t static_anim[10] = {
    // -1 - 0
    {0x77, {SA_NOANIM, SA_NOANIM}, DA_0_TO_1_a, DA_0_TO_1_a, false},
    // -2 - 1
    {0x24, {SA_NOANIM, SA_NOANIM}, DA_1_TO_2_a, DA_1_TO_0_a, false},
    // -3 - 2
    {0x5D, {SA_NOANIM, SA_NOANIM}, DA_2_TO_3_a, DA_2_TO_0, false},
    // -4 - 3
    {0x6D, {SA_NOANIM, SA_NOANIM}, DA_3_TO_4, DA_3_TO_0, false},
    // -5 - 4
    {0x2E, {SA_NOANIM, SA_NOANIM}, DA_4_TO_5, DA_4_TO_0_a, false},
    // -6 - 5
    {0x6B, {SA_NOANIM, SA_NOANIM}, DA_5_TO_6, DA_5_TO_0, false},
    // -7 - 6
    {0x7B, {SA_NOANIM, SA_NOANIM}, DA_6_TO_7_a, DA_6_TO_0, false},
    // -8 - 7
    {0x25, {SA_NOANIM, SA_NOANIM}, DA_7_TO_8_a, DA_7_TO_0_a, false},
    // -9 - 8
    {0x7F, {SA_NOANIM, SA_NOANIM}, DA_8_TO_9, DA_8_TO_0, false},
    // -10 - 9
    {0x6F, {SA_NOANIM, SA_NOANIM}, DA_9_TO_0, DA_9_TO_0, false}
};

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

/** Return the appropriate digit_anim_info_t.
 *
 * This function perform the triage between animation steps and static digits
 * steps.
 */
static
const digit_anim_info_t*
retrieve_anim(digit_anim_t digit_anim);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static
const digit_anim_info_t*
retrieve_anim(digit_anim_t digit_anim)
{
    return (digit_anim > 0)
           ? &moving_anim[digit_anim - 1]
           : &static_anim[-digit_anim - 1];
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

segment_orientation_t
anim_segment_get(segment_anim_t segment_anim,
                 int anim_pos,
                 digit_size_t digit_size,
                 GPoint* offset)
{
    if (segment_anim < 0) {
        segment_anim = -segment_anim;
        anim_pos = 8 - anim_pos;
    }

    --segment_anim;
    *offset = animated_segment_offsets[digit_size][segment_anim * 9 + anim_pos];
    return animated_segment_orientation[segment_anim * 9 + anim_pos];
}

digit_fixed_segments_t
anim_get_fixed_segments(digit_anim_t digit_anim)
{
    return retrieve_anim(digit_anim)->fixed_segments;
}

bool
anim_get_fixed_segment_state(digit_fixed_segments_t segments,
                             unsigned segment_id)
{
    return (segments & (1u << segment_id)) != 0;
}

const digit_segment_animation_t*
anim_get_segment_anim(digit_anim_t digit_anim)
{
    return &retrieve_anim(digit_anim)->moving_segments;
}

digit_anim_t
anim_get_next_anim(digit_anim_t digit_anim)
{
    return retrieve_anim(digit_anim)->next_animation;
}

digit_anim_t
anim_get_next_quick_anim(digit_anim_t digit_anim)
{
    return retrieve_anim(digit_anim)->quick_next;
}

bool
anim_is_static_digit(digit_anim_t digit_anim)
{
    return digit_anim < 0;
}

bool
anim_is_multipart(digit_anim_t digit_anim)
{
    return retrieve_anim(digit_anim)->multipart;
}

bool
anim_is_complete(digit_anim_t digit_anim,
                 int step)
{
    return (digit_anim < 0)
           ? true
           : (step >= 9);
}

int
anim_get_displayed_number(digit_anim_t digit_anim)
{
    return (digit_anim < 0)
           ? (-digit_anim - 1)
           : -1;
}

int
anim_get_step_count(digit_anim_t digit_anim)
{
    return (digit_anim < 0)
           ? 0
           : 9;
}

digit_anim_t
anim_get_anim_for_number(int number)
{
    return -number - 1;
}

