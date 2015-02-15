#include <pebble.h>
#include "anim.h"

/**
 * Segment position when animated.
 * First 9 are for 1->0, next 9 for 4->6, etc.
 */
static const animated_segment_info_t big_segment_info[7 * 9] = {
    { SO_171, {1, 6}},
    { SO_162, {2, 6}},
    { SO_153, {2, 5}},
    { SO_144, {4, 5}},
    { SO_135, {5, 5}},
    { SO_126, {5, 3}},
    { SO_117, {6, 3}},
    { SO_108, {6, 2}},
    { SO_99, {6, 2}},
    { SO_9, {1, 43}},
    { SO_18, {1, 45}},
    { SO_27, {2, 47}},
    { SO_36, {4, 50}},
    { SO_45, {5, 54}},
    { SO_54, {5, 57}},
    { SO_63, {6, 60}},
    { SO_72, {6, 65}},
    { SO_81, {6, 70}},
    { SO_81, {7, 1}},
    { SO_72, {7, 2}},
    { SO_63, {9, 3}},
    { SO_54, {13, 4}},
    { SO_45, {16, 5}},
    { SO_36, {19, 6}},
    { SO_27, {24, 6}},
    { SO_18, {28, 6}},
    { SO_9, {32, 6}},
    { SO_99, {6, 69}},
    { SO_108, {8, 65}},
    { SO_117, {10, 62}},
    { SO_126, {12, 57}},
    { SO_135, {16, 54}},
    { SO_144, {19, 52}},
    { SO_153, {23, 47}},
    { SO_162, {28, 44}},
    { SO_171, {33, 44}},
    { SO_171, {33, 7}},
    { SO_162, {28, 7}},
    { SO_153, {23, 10}},
    { SO_144, {19, 15}},
    { SO_135, {16, 17}},
    { SO_126, {12, 20}},
    { SO_117, {10, 25}},
    { SO_108, {8, 28}},
    { SO_99, {6, 32}},
    { SO_99, {6, 39}},
    { SO_108, {6, 39}},
    { SO_117, {6, 40}},
    { SO_126, {5, 40}},
    { SO_135, {5, 42}},
    { SO_144, {4, 42}},
    { SO_153, {2, 42}},
    { SO_162, {2, 43}},
    { SO_171, {1, 43}},
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

static const animated_segment_info_t small_segment_info[7 * 9] = {
    { SO_171, {2, 5}},
    { SO_162, {3, 5}},
    { SO_153, {2, 5}},
    { SO_144, {5, 5}},
    { SO_135, {5, 3}},
    { SO_126, {6, 3}},
    { SO_117, {6, 2}},
    { SO_108, {6, 2}},
    { SO_99, {6, 0}},
    { SO_9, {0, 40}},
    { SO_18, {1, 42}},
    { SO_27, {3, 45}},
    { SO_36, {3, 47}},
    { SO_45, {4, 49}},
    { SO_54, {5, 55}},
    { SO_63, {5, 57}},
    { SO_72, {5, 62}},
    { SO_81, {5, 66}},
    { SO_81, {6, 1}},
    { SO_72, {6, 2}},
    { SO_63, {8, 2}},
    { SO_54, {12, 3}},
    { SO_45, {14, 4}},
    { SO_36, {18, 4}},
    { SO_27, {23, 5}},
    { SO_18, {26, 5}},
    { SO_9, {31, 5}},
    { SO_99, {5, 65}},
    { SO_108, {7, 62}},
    { SO_117, {9, 58}},
    { SO_126, {11, 54}},
    { SO_135, {15, 50}},
    { SO_144, {18, 47}},
    { SO_153, {21, 45}},
    { SO_162, {27, 42}},
    { SO_171, {31, 41}},
    { SO_171, {31, 6}},
    { SO_162, {27, 7}},
    { SO_153, {21, 10}},
    { SO_144, {18, 12}},
    { SO_135, {15, 15}},
    { SO_126, {11, 19}},
    { SO_117, {9, 23}},
    { SO_108, {7, 27}},
    { SO_99, {5, 30}},
    { SO_99, {6, 35}},
    { SO_108, {6, 37}},
    { SO_117, {6, 37}},
    { SO_126, {6, 38}},
    { SO_135, {5, 38}},
    { SO_144, {5, 40}},
    { SO_153, {2, 40}},
    { SO_162, {3, 40}},
    { SO_171, {2, 40}},
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

typedef struct {
    animation_fixed_digits_t fixed_digits;
    animation_digit_segment_t moving_segments;
    int next_animation;
    int quick_next;
} digit_anim_t;

static const digit_anim_t moving_anim[] = {
    // 1 - 0>1
    {{{true, false, true, false, false, true, true}}, {{1, 2}}, 2, 2},
    // 2 - 0>1
    {{{false, false, true, false, false, true, false}}, {{3, 4}}, -2, -2},
    // 3 - 1>2
    {{{false, false, true, false, false, false, false}}, {{ -3, -4}}, 4, 4},
    // 4 - 1>2
    {{{true, false, true, false, false, false, true}}, {{5, -2}}, -3, -3},
    // 5 - 2>3
    {{{true, false, true, true, false, false, true}}, {{2, 0}}, 6, 6},
    // 6 - 2>3
    {{{true, false, true, true, false, false, true}}, {{4, 0}}, -4, -4},
    // 7 - 3>4
    {{{false, false, true, true, false, true, false}}, {{ -1, 4}}, -5, -5},
    // 8 - 4>5
    {{{false, true, false, true, false, true, false}}, {{ -3, -4}}, -6, -6},
    // 9 - 5>6
    {{{true, true, false, true, false, true, true}}, {{6, 0}}, -7, -7},
    // 10 - 6>7
    {{{true, true, false, false, false, true, true}}, {{ -5, 2}}, 11, 11},
    // 11 - 6>7
    {{{true, false, true, false, false, true, false}}, {{1, 4}}, -8, -8},
    // 12 - 7>8
    {{{true, false, true, false, false, true, false}}, {{ -1, -4}}, 13, 13},
    // 13 - 7>8
    {{{true, true, true, false, false, true, true}}, {{7, -2}}, -9, -9},
    // 14 - 8>9
    {{{true, true, true, true, false, true, true}}, {{ -6, 0}}, -10, -10},
    // 15 - 9>0
    {{{true, true, true, false, false, true, true}}, {{ -7, -2}}, -1, -1},
    // 16 - 1>0
    {{{false, false, true, false, false, true, false}}, {{ -3, -4}}, 17, 17},
    // 17 - 1>0
    {{{true, false, true, false, false, true, true}}, {{ -1, -2}}, -1, -1},
    // 18 - 2>0
    {{{true, false, true, false, true, false, true}}, {{ -7, 4}}, -1, -1},
    // 19 - 3>0
    {{{true, false, true, false, false, true, true}}, {{6, -1}}, -1, -1},
    // 20 - 4>0
    {{{false, true, true, false, false, true, false}}, {{6, 0}}, 21, 21},
    // 21 - 4>0
    {{{false, true, true, false, true, true, false}}, {{ -4, 1}}, -1, -1},
    // 22 - 5>0
    {{{true, true, false, false, false, true, true}}, {{3, -2}}, -1, -1},
    // 23 - 6>0
    {{{true, true, false, false, true, true, true}}, {{ -5, 0}}, -1, -1},
    // 24 - 7>0
    {{{true, false, true, false, false, true, false}}, {{ -4, -1}}, 25, 25},
    // 25 - 7>0
    {{{true, true, true, false, false, true, true}}, {{ -2, 0}}, 25, 25},
    // 26 - 8>0
    {{{true, true, true, false, true, true, true}}, {{6, 0}}, -1, -1}
};

static const digit_anim_t fixed_digits_anim[10] = {
    // -1 - 0
    {{{true, true, true, false, true, true, true}}, {{0, 0}}, 1, 1},
    // -2 - 1
    {{{false, false, true, false, false, true, false}}, {{0, 0}}, 3, 16},
    // -3 - 2
    {{{true, false, true, true, true, false, true}}, {{0, 0}}, 5, 18},
    // -4 - 3
    {{{true, false, true, true, false, true, true}}, {{0, 0}}, 7, 19},
    // -5 - 4
    {{{false, true, true, true, false, true, false}}, {{0, 0}}, 8, 20},
    // -6 - 5
    {{{true, true, false, true, false, true, true}}, {{0, 0}}, 9, 22},
    // -7 - 6
    {{{true, true, false, true, true, true, true}}, {{0, 0}}, 10, 23},
    // -8 - 7
    {{{true, false, true, false, false, true, false}}, {{0, 0}}, 12, 24},
    // -9 - 8
    {{{true, true, true, true, true, true, true}}, {{0, 0}}, 14, 26},
    // -10 - 9
    {{{true, true, true, true, false, true, true}}, {{0, 0}}, 15, 15}
};

static const digit_anim_t* retrieve_anim(int digit_anim_id);

static const digit_anim_t* retrieve_anim(int digit_anim_id)
{
    const digit_anim_t* source = (digit_anim_id > 0) ? moving_anim : fixed_digits_anim;

    if (digit_anim_id < 0) {
        digit_anim_id = -digit_anim_id;
    }

    --digit_anim_id;
    return &source[digit_anim_id];
}

const animated_segment_info_t* anim_segment_get(int anim_id, int anim_pos, digit_size_t digit_size)
{
    const animated_segment_info_t* info_source = (digit_size == DS_BIG) ? big_segment_info : small_segment_info;

    if (anim_id < 0) {
        anim_id = -anim_id;
        anim_pos = 8 - anim_pos;
    }

    --anim_id;
    return &info_source[anim_id * 9 + anim_pos];
}

const animation_fixed_digits_t* anim_get_fixed_digits(int digit_anim_id)
{
    return &retrieve_anim(digit_anim_id)->fixed_digits;
}

const animation_digit_segment_t* anim_get_segment_anim(int digit_anim_id)
{
    return &retrieve_anim(digit_anim_id)->moving_segments;
}

int anim_get_next_anim(int digit_anim_id)
{
    return retrieve_anim(digit_anim_id)->next_animation;
}

int anim_get_next_quick_anim(int digit_anim_id)
{
    return retrieve_anim(digit_anim_id)->quick_next;
}

