#include <pebble.h>
#include "anim.h"
    
/** 
 * Segment position when animated.
 * First 9 are for 1->0, next 9 for 4->6, etc.
 */
static const animated_segment_info_t big_segment_info[7 * 9] = {
    { 17, {1, 6}},
    { 16, {2, 6}},
    { 15, {2, 5}},
    { 14, {4, 5}},
    { 13, {5, 5}},
    { 12, {5, 3}},
    { 11, {6, 3}},
    { 10, {6, 2}},
    { 9, {6, 2}},
    { 0, {1, 43}},
    { 1, {1, 45}},
    { 2, {2, 47}},
    { 3, {4, 50}},
    { 4, {5, 54}},
    { 5, {5, 57}},
    { 6, {6, 60}},
    { 7, {6, 65}},
    { 8, {6, 70}},
    { 8, {7, 1}},
    { 7, {7, 2}},
    { 6, {9, 3}},
    { 5, {13, 4}},
    { 4, {16, 5}},
    { 3, {19, 6}},
    { 2, {24, 6}},
    { 1, {28, 6}},
    { 0, {32, 6}},
    { 9, {6, 69}},
    { 10, {8, 65}},
    { 11, {10, 62}},
    { 12, {12, 57}},
    { 13, {16, 54}},
    { 14, {19, 52}},
    { 15, {23, 47}},
    { 16, {28, 44}},
    { 17, {33, 44}},
    { 17, {33, 7}},
    { 16, {28, 7}},
    { 15, {23, 10}},
    { 14, {19, 15}},
    { 13, {16, 17}},
    { 12, {12, 20}},
    { 11, {10, 25}},
    { 10, {8, 28}},
    { 9, {6, 32}},
    { 9, {6, 39}},
    { 10, {6, 39}},
    { 11, {6, 40}},
    { 12, {5, 40}},
    { 13, {5, 42}},
    { 14, {4, 42}},
    { 15, {2, 42}},
    { 16, {2, 43}},
    { 17, {1, 43}},
    { 0, {1, 6}},
    { 1, {1, 8}},
    { 2, {2, 10}},
    { 3, {4, 13}},
    { 4, {5, 17}},
    { 5, {5, 20}},
    { 6, {6, 23}},
    { 7, {6, 28}},
    { 8, {6, 33}}
};

static const animated_segment_info_t small_segment_info[7 * 9] = {
    { 17, {2, 5}},
    { 16, {3, 5}},
    { 15, {2, 5}},
    { 14, {5, 5}},
    { 13, {5, 3}},
    { 12, {6, 3}},
    { 11, {6, 2}},
    { 10, {6, 2}},
    { 9, {6, 0}},
    { 0, {0, 40}},
    { 1, {1, 42}},
    { 2, {3, 45}},
    { 3, {3, 47}},
    { 4, {4, 49}},
    { 5, {5, 55}},
    { 6, {5, 57}},
    { 7, {5, 62}},
    { 8, {5, 66}},
    { 8, {6, 1}},
    { 7, {6, 2}},
    { 6, {8, 2}},
    { 5, {12, 3}},
    { 4, {14, 4}},
    { 3, {18, 4}},
    { 2, {23, 5}},
    { 1, {26, 5}},
    { 0, {31, 5}},
    { 9, {5, 65}},
    { 10, {7, 62}},
    { 11, {9, 58}},
    { 12, {11, 54}},
    { 13, {15, 50}},
    { 14, {18, 47}},
    { 15, {21, 45}},
    { 16, {27, 42}},
    { 17, {31, 41}},
    { 17, {31, 6}},
    { 16, {27, 7}},
    { 15, {21, 10}},
    { 14, {18, 12}},
    { 13, {15, 15}},
    { 12, {11, 19}},
    { 11, {9, 23}},
    { 10, {7, 27}},
    { 9, {5, 30}},
    { 9, {6, 35}},
    { 10, {6, 37}},
    { 11, {6, 37}},
    { 12, {6, 38}},
    { 13, {5, 38}},
    { 14, {5, 40}},
    { 15, {2, 40}},
    { 16, {3, 40}},
    { 17, {2, 40}},
    { 0, {0, 5}},
    { 1, {1, 7}},
    { 2, {3, 10}},
    { 3, {3, 12}},
    { 4, {4, 14}},
    { 5, {5, 20}},
    { 6, {5, 22}},
    { 7, {5, 27}},
    { 8, {5, 31}},
};

typedef struct {
    animation_fixed_digits_t fixed_digits;
    animation_digit_segment_t moving_segments;
    int next_animation;
} digit_anim_t;

static const digit_anim_t moving_anim[15] = {
    {{{true, false, true, false, false, true, true}}, {{1, 2}}, 2},
    {{{false, false, true, false, false, true, false}}, {{3, 4}}, -2},
    {{{false, false, true, false, false, false, false}}, {{-3, -4}}, 4},
    {{{true, false, true, false, false, false, true}}, {{5, -2}}, -3},
    {{{true, false, true, true, false, false, true}}, {{2, 0}}, 6},
    {{{true, false, true, true, false, false, true}}, {{4, 0}}, -4},
    {{{false, false, true, true, false, true, false}}, {{-1, 4}}, -5},
    {{{false, true, false, true, false, true, false}}, {{-3, -4}}, -6},
    {{{true, true, false, true, false, true, true}}, {{6, 0}}, -7},
    {{{true, true, false, false, false, true, true}}, {{-5, 2}}, 11},
    {{{true, false, true, false, false, true, false}}, {{1, 4}}, -8},
    {{{true, false, true, false, false, true, false}}, {{-1, -4}}, 13},
    {{{true, true, true, false, false, true, true}}, {{7, -2}}, -9},
    {{{true, true, true, true, false, true, true}}, {{-6, 0}}, -10},
    {{{true, true, true, false, false, true, true}}, {{-7, -2}}, -1}
};

static const digit_anim_t fixed_digits_anim[10] = {
    {{{true, true, true, false, true, true, true}}, {{0, 0}}, 1},
    {{{false, false, true, false, false, true, false}}, {{0, 0}}, 3},
    {{{true, false, true, true, true, false, true}}, {{0, 0}}, 5},
    {{{true, false, true, true, false, true, true}}, {{0, 0}}, 7},
    {{{false, true, true, true, false, true, false}}, {{0, 0}}, 8},
    {{{true, true, false, true, false, true, true}}, {{0, 0}}, 9},
    {{{true, true, false, true, true, true, true}}, {{0, 0}}, 10},
    {{{true, false, true, false, false, true, false}}, {{0, 0}}, 12},
    {{{true, true, true, true, true, true, true}}, {{0, 0}}, 14},
    {{{true, true, true, true, false, true, true}}, {{0, 0}}, 15}
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
    const animated_segment_info_t* info_source = (digit_size == DIGIT_BIG) ? big_segment_info : small_segment_info;
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
