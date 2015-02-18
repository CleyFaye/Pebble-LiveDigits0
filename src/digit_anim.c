// true, false
#include <pebble.h>
// segment_orientation_t
// digit_size_t
#include "digit_info.h"
// Associated header
#include "digit_anim.h"

// ===============
// PRIVATE TYPES =
// ===============

/** All info about a digit animation step. */
typedef struct {
    /** The digits to statically display (not animated) */
    animation_fixed_digits_t fixed_digits;
    /** The animated digits (up to two) */
    animation_digit_segment_t moving_segments;
    /** The next animation in the sequence */
    digit_anim_t next_animation;
    /** The shortcut animation to 0 */
    digit_anim_t quick_next;
} digit_anim_info_t;

// ==============
// PRIVATE DATA =
// ==============

/** Segment position when animated.
 * First 9 are for 1->0, next 9 for 4->6, etc. and represent all segments
 * positions for each segment_anim_t positive values.
 * Negative values are simply the positive one taken in reverse.
 * To accomodate negative animation values, they start at 1/-1 and not at 0.
 * These are for the big digits.
 */
static const animated_segment_info_t big_segment_info[7 * 9] = {
    // SA_1_TO_0
    { SO_171, {1, 6}},
    { SO_162, {2, 6}},
    { SO_153, {2, 5}},
    { SO_144, {4, 5}},
    { SO_135, {5, 5}},
    { SO_126, {5, 3}},
    { SO_117, {6, 3}},
    { SO_108, {6, 2}},
    { SO_99, {6, 2}},
    // SA_4_TO_6
    { SO_9, {1, 43}},
    { SO_18, {1, 45}},
    { SO_27, {2, 47}},
    { SO_36, {4, 50}},
    { SO_45, {5, 54}},
    { SO_54, {5, 57}},
    { SO_63, {6, 60}},
    { SO_72, {6, 65}},
    { SO_81, {6, 70}},
    // SA_0_TO_2
    { SO_81, {7, 1}},
    { SO_72, {7, 2}},
    { SO_63, {9, 3}},
    { SO_54, {13, 4}},
    { SO_45, {16, 5}},
    { SO_36, {19, 6}},
    { SO_27, {24, 6}},
    { SO_18, {28, 6}},
    { SO_9, {32, 6}},
    // SA_6_TO_5
    { SO_99, {6, 69}},
    { SO_108, {8, 65}},
    { SO_117, {10, 62}},
    { SO_126, {12, 57}},
    { SO_135, {16, 54}},
    { SO_144, {19, 52}},
    { SO_153, {23, 47}},
    { SO_162, {28, 44}},
    { SO_171, {33, 44}},
    // SA_2_TO_3
    { SO_171, {33, 7}},
    { SO_162, {28, 7}},
    { SO_153, {23, 10}},
    { SO_144, {19, 15}},
    { SO_135, {16, 17}},
    { SO_126, {12, 20}},
    { SO_117, {10, 25}},
    { SO_108, {8, 28}},
    { SO_99, {6, 32}},
    // SA_3_TO_4
    { SO_99, {6, 39}},
    { SO_108, {6, 39}},
    { SO_117, {6, 40}},
    { SO_126, {5, 40}},
    { SO_135, {5, 42}},
    { SO_144, {4, 42}},
    { SO_153, {2, 42}},
    { SO_162, {2, 43}},
    { SO_171, {1, 43}},
    // SA_1_TO_3
    { SO_9, {1, 6}},
    { SO_18, {1, 8}},
    { SO_27, {2, 10}},
    { SO_36, {4, 13}},
    { SO_45, {5, 17}},
    { SO_54, {5, 20}},
    { SO_63, {6, 23}},
    { SO_72, {6, 28}},
    { SO_81, {6, 33}}
};

/** Segment position when animated.
 * First 9 are for 1->0, next 9 for 4->6, etc. and represent all segments
 * positions for each segment_anim_t positive values.
 * Negative values are simply the positive one taken in reverse.
 * To accomodate negative animation values, they start at 1/-1 and not at 0.
 * These are for the medium digits.
 */
static const animated_segment_info_t medium_segment_info[7 * 9] = {
    // SA_1_TO_0
    { SO_171, {2, 5}},
    { SO_162, {3, 5}},
    { SO_153, {2, 5}},
    { SO_144, {5, 5}},
    { SO_135, {5, 3}},
    { SO_126, {6, 3}},
    { SO_117, {6, 2}},
    { SO_108, {6, 2}},
    { SO_99, {6, 0}},
    // SA_4_TO_6
    { SO_9, {0, 40}},
    { SO_18, {1, 42}},
    { SO_27, {3, 45}},
    { SO_36, {3, 47}},
    { SO_45, {4, 49}},
    { SO_54, {5, 55}},
    { SO_63, {5, 57}},
    { SO_72, {5, 62}},
    { SO_81, {5, 66}},
    // SA_0_TO_2
    { SO_81, {6, 1}},
    { SO_72, {6, 2}},
    { SO_63, {8, 2}},
    { SO_54, {12, 3}},
    { SO_45, {14, 4}},
    { SO_36, {18, 4}},
    { SO_27, {23, 5}},
    { SO_18, {26, 5}},
    { SO_9, {31, 5}},
    // SA_6_TO_5
    { SO_99, {5, 65}},
    { SO_108, {7, 62}},
    { SO_117, {9, 58}},
    { SO_126, {11, 54}},
    { SO_135, {15, 50}},
    { SO_144, {18, 47}},
    { SO_153, {21, 45}},
    { SO_162, {27, 42}},
    { SO_171, {31, 41}},
    // SA_2_TO_3
    { SO_171, {31, 6}},
    { SO_162, {27, 7}},
    { SO_153, {21, 10}},
    { SO_144, {18, 12}},
    { SO_135, {15, 15}},
    { SO_126, {11, 19}},
    { SO_117, {9, 23}},
    { SO_108, {7, 27}},
    { SO_99, {5, 30}},
    // SA_3_TO_4
    { SO_99, {6, 35}},
    { SO_108, {6, 37}},
    { SO_117, {6, 37}},
    { SO_126, {6, 38}},
    { SO_135, {5, 38}},
    { SO_144, {5, 40}},
    { SO_153, {2, 40}},
    { SO_162, {3, 40}},
    { SO_171, {2, 40}},
    // SA_1_TO_3
    { SO_9, {0, 5}},
    { SO_18, {1, 7}},
    { SO_27, {3, 10}},
    { SO_36, {3, 12}},
    { SO_45, {4, 14}},
    { SO_54, {5, 20}},
    { SO_63, {5, 22}},
    { SO_72, {5, 27}},
    { SO_81, {5, 31}},
};

/** Digit animation steps.
 * Each value correspond to a positive value of digit_anim_t.
 */
static const digit_anim_info_t moving_anim[] = {
    // 1 - 0>1
    {   {{true, false, true, false, false, true, true}},
        {{SA_1_TO_0, SA_4_TO_6}}, DA_0_TO_1_b, DA_0_TO_1_b
    },
    // 2 - 0>1
    {   {{false, false, true, false, false, true, false}},
        {{SA_0_TO_2, SA_6_TO_5}}, DA_1, DA_1
    },
    // 3 - 1>2
    {   {{false, false, true, false, false, false, false}},
        {{SA_2_TO_0, SA_5_TO_6}}, DA_1_TO_2_b, DA_1_TO_2_b
    },
    // 4 - 1>2
    {   {{true, false, true, false, false, false, true}},
        {{SA_2_TO_3, SA_6_TO_4}}, DA_2, DA_2
    },
    // 5 - 2>3
    {   {{true, false, true, true, false, false, true}},
        {{SA_4_TO_6, SA_NOANIM}}, DA_2_TO_3_b, DA_2_TO_3_b
    },
    // 6 - 2>3
    {   {{true, false, true, true, false, false, true}},
        {{SA_6_TO_5, SA_NOANIM}}, DA_3, DA_3
    },
    // 7 - 3>4
    {   {{false, false, true, true, false, true, false}},
        {{SA_0_TO_1, SA_6_TO_5}}, DA_4, DA_4
    },
    // 8 - 4>5
    {   {{false, true, false, true, false, true, false}},
        {{SA_2_TO_0, SA_5_TO_6}}, DA_5, DA_5
    },
    // 9 - 5>6
    {   {{true, true, false, true, false, true, true}},
        {{SA_3_TO_4, SA_NOANIM}}, DA_6, DA_6
    },
    // 10 - 6>7
    {   {{true, true, false, false, false, true, true}},
        {{SA_3_TO_2, SA_4_TO_6}}, DA_6_TO_7_b, DA_6_TO_7_b
    },
    // 11 - 6>7
    {   {{true, false, true, false, false, true, false}},
        {{SA_1_TO_0, SA_6_TO_5}}, DA_7, DA_7
    },
    // 12 - 7>8
    {   {{true, false, true, false, false, true, false}},
        {{SA_0_TO_1, SA_5_TO_6}}, DA_7_TO_8_b, DA_7_TO_8_b
    },
    // 13 - 7>8
    {   {{true, true, true, false, false, true, true}},
        {{SA_1_TO_3, SA_6_TO_4}}, DA_8, DA_8
    },
    // 14 - 8>9
    {   {{true, true, true, true, false, true, true}},
        {{SA_4_TO_3, SA_NOANIM}}, DA_9, DA_9
    },
    // 15 - 9>0
    {   {{true, true, true, false, false, true, true}},
        {{SA_3_TO_1, SA_6_TO_4}}, DA_0, DA_0
    },
    // 16 - 1>0
    {   {{false, false, true, false, false, true, false}},
        {{SA_2_TO_0, SA_5_TO_6}}, DA_1_TO_0_b, DA_1_TO_0_b
    },
    // 17 - 1>0
    {   {{true, false, true, false, false, true, true}},
        {{SA_0_TO_1, SA_6_TO_4}}, DA_0, DA_0
    },
    // 18 - 2>0
    {   {{true, false, true, false, true, false, true}},
        {{SA_3_TO_1, SA_6_TO_5}}, DA_0, DA_0
    },
    // 19 - 3>0
    {   {{true, false, true, false, false, true, true}},
        {{SA_3_TO_4, SA_0_TO_1}}, DA_0, DA_0
    },
    // 20 - 4>0
    {   {{false, true, true, false, false, true, false}},
        {{SA_3_TO_4, SA_NOANIM}}, DA_4_TO_0_b, DA_4_TO_0_b
    },
    // 21 - 4>0
    {   {{false, true, true, false, true, true, false}},
        {{SA_5_TO_6, SA_1_TO_0}}, DA_0, DA_0
    },
    // 22 - 5>0
    {   {{true, true, false, false, false, true, true}},
        {{SA_0_TO_2, SA_6_TO_4}}, DA_0, DA_0
    },
    // 23 - 6>0
    {   {{true, true, false, false, true, true, true}},
        {{SA_3_TO_2, SA_NOANIM}}, DA_0, DA_0
    },
    // 24 - 7>0
    {   {{true, false, true, false, false, true, false}},
        {{SA_5_TO_6, SA_0_TO_1}}, DA_7_TO_0_b, DA_7_TO_0_b
    },
    // 25 - 7>0
    {   {{true, true, true, false, false, true, true}},
        {{SA_6_TO_4, SA_NOANIM}}, DA_0, DA_0
    },
    // 26 - 8>0
    {   {{true, true, true, false, true, true, true}},
        {{SA_3_TO_4, SA_NOANIM}}, DA_0, DA_0
    }
};

/** Digit animation steps.
 * These are for static digits, treated as animation because it's easier.
 *
 * Each value correspond to a negative value in digit_anim_t.
 */
static const digit_anim_info_t fixed_digits_anim[10] = {
    // -1 - 0
    {   {{true, true, true, false, true, true, true}},
        {{SA_NOANIM, SA_NOANIM}}, DA_0_TO_1_a, DA_0_TO_1_a
    },
    // -2 - 1
    {   {{false, false, true, false, false, true, false}},
        {{SA_NOANIM, SA_NOANIM}}, DA_1_TO_2_a, DA_1_TO_0_a
    },
    // -3 - 2
    {   {{true, false, true, true, true, false, true}},
        {{SA_NOANIM, SA_NOANIM}}, DA_2_TO_3_a, DA_2_TO_0
    },
    // -4 - 3
    {   {{true, false, true, true, false, true, true}},
        {{SA_NOANIM, SA_NOANIM}}, DA_3_TO_4, DA_3_TO_0
    },
    // -5 - 4
    {   {{false, true, true, true, false, true, false}},
        {{SA_NOANIM, SA_NOANIM}}, DA_4_TO_5, DA_4_TO_0_a
    },
    // -6 - 5
    {   {{true, true, false, true, false, true, true}},
        {{SA_NOANIM, SA_NOANIM}}, DA_5_TO_6, DA_5_TO_0
    },
    // -7 - 6
    {   {{true, true, false, true, true, true, true}},
        {{SA_NOANIM, SA_NOANIM}}, DA_6_TO_7_a, DA_6_TO_0
    },
    // -8 - 7
    {   {{true, false, true, false, false, true, false}},
        {{SA_NOANIM, SA_NOANIM}}, DA_7_TO_8_a, DA_7_TO_0_a
    },
    // -9 - 8
    {   {{true, true, true, true, true, true, true}},
        {{SA_NOANIM, SA_NOANIM}}, DA_8_TO_9, DA_8_TO_0
    },
    // -10 - 9
    {   {{true, true, true, true, false, true, true}},
        {{SA_NOANIM, SA_NOANIM}}, DA_9_TO_0, DA_9_TO_0
    }
};

// ================================
// PRIVATE FUNCTIONS DECLARATIONS =
// ================================

/** Return the appropriate digit_anim_info_t.
 *
 * This function perform the triage between animation steps and static digits
 * steps.
 *
 * @param digit_anim The digit animation id
 * @return The digit_anim_info_t to use.
 */
static const digit_anim_info_t* retrieve_anim(digit_anim_t digit_anim);

// ===============================
// PRIVATE FUNCTIONS DEFINITIONS =
// ===============================

static const digit_anim_info_t* retrieve_anim(digit_anim_t digit_anim)
{
    const digit_anim_info_t* source = (digit_anim > 0) ? moving_anim : fixed_digits_anim;

    if (digit_anim < 0) {
        digit_anim = -digit_anim;
    }

    --digit_anim;
    return &source[digit_anim];
}

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================
//
const animated_segment_info_t* anim_segment_get(segment_anim_t segment_anim,
        int anim_pos,
        digit_size_t digit_size)
{
    const animated_segment_info_t* info_source = (digit_size == DS_BIG) ? big_segment_info : medium_segment_info;

    if (segment_anim < 0) {
        segment_anim = -segment_anim;
        anim_pos = 8 - anim_pos;
    }

    --segment_anim;
    return &info_source[segment_anim * 9 + anim_pos];
}

const animation_fixed_digits_t* anim_get_fixed_digits(digit_anim_t digit_anim)
{
    return &retrieve_anim(digit_anim)->fixed_digits;
}

const animation_digit_segment_t* anim_get_segment_anim(digit_anim_t digit_anim)
{
    return &retrieve_anim(digit_anim)->moving_segments;
}

digit_anim_t anim_get_next_anim(digit_anim_t digit_anim)
{
    return retrieve_anim(digit_anim)->next_animation;
}

digit_anim_t anim_get_next_quick_anim(digit_anim_t digit_anim)
{
    return retrieve_anim(digit_anim)->quick_next;
}

bool anim_is_static_digit(digit_anim_t digit_anim)
{
    return digit_anim < 0;
}

int anim_get_displayed_number(digit_anim_t digit_anim)
{
    if (!anim_is_static_digit(digit_anim)) {
        return -1;
    } else {
        return -digit_anim - 1;
    }
}

digit_anim_t anim_get_anim_for_number(int number)
{
    return -number - 1;
}

