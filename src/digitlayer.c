#include <pebble.h>
#include "anim.h"
#include "digitlayer.h"

typedef struct {
    digit_size_t size;
    int current_number;
    int target_number;
    int current_anim;
    int current_anim_position;
    AppTimer* anim_timer;
    int max_number;
    int animate_speed;
    bool quick_wrap;
} digit_info_t;

static const int BIG_WIDTH = 46;
static const int BIG_HEIGHT = 83;
static const int SMALL_WIDTH = 42;
static const int SMALL_HEIGHT = 77;
static const int DEFAULT_SPEED = 150;

typedef struct {
    digit_orientation_t orientation;
    GPoint offset;
} digit_draw_info_t;

static const digit_draw_info_t big_digit_draw_info[7] = {
    {DIGIT_HORIZONTAL, { .x = 6, .y = 0}},
    {DIGIT_VERTICAL, { .x = 0, .y = 6}},
    {DIGIT_VERTICAL, { .x = 37, .y = 6}},
    {DIGIT_HORIZONTAL, { .x = 6, .y = 37}},
    {DIGIT_VERTICAL, { .x = 0, .y = 43}},
    {DIGIT_VERTICAL, { .x = 37, .y = 43}},
    {DIGIT_HORIZONTAL, { .x = 6, .y = 74}}
};

static const digit_draw_info_t small_digit_draw_info[7] = {
    {DIGIT_HORIZONTAL, { .x = 5, .y = 0}},
    {DIGIT_VERTICAL, { .x = 0, .y = 5}},
    {DIGIT_VERTICAL, { .x = 35, .y = 5}},
    {DIGIT_HORIZONTAL, { .x = 5, .y = 35}},
    {DIGIT_VERTICAL, { .x = 0, .y = 40}},
    {DIGIT_VERTICAL, { .x = 35, .y = 40}},
    {DIGIT_HORIZONTAL, { .x = 5, .y = 70}}
};

static void handle_layer_update(struct Layer* layer, GContext* ctx);
static void digit_layer_check_timer_status(struct Layer* layer);

static void handle_layer_update(struct Layer* layer, GContext* ctx)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    GRect layer_bounds = layer_get_bounds(layer);
    graphics_context_set_compositing_mode(ctx, GCompOpOr);
    // Draw digit
    const digit_draw_info_t* digit_draw_info = (info->size == DIGIT_BIG) ? big_digit_draw_info : small_digit_draw_info;
    const animation_fixed_digits_t* fixed_digits = anim_get_fixed_digits(info->current_anim);
    for (int i = 0; i < 7; ++i) {
        if (!fixed_digits->enabled[i]) {
            continue;
        }
        GRect draw_rect;
        draw_rect.origin = digit_draw_info[i].offset;
        draw_rect.origin.x += layer_bounds.origin.x;
        draw_rect.origin.y += layer_bounds.origin.y;
        GBitmap* digit_bitmap = get_digit_image(digit_draw_info[i].orientation, info->size, &draw_rect.size);
        graphics_draw_bitmap_in_rect(ctx, digit_bitmap, draw_rect);
    }
    // Draw moving parts
    const animation_digit_segment_t* segment_anim = anim_get_segment_anim(info->current_anim);
    for (int i = 0; i < 2; ++i) {
        if (segment_anim->segment_anim[i] != 0) {
            const animated_segment_info_t* segment_info = anim_segment_get(segment_anim->segment_anim[i], info->current_anim_position, info->size);
            GRect draw_rect;
            draw_rect.origin = segment_info->offset;
            draw_rect.origin.x += layer_bounds.origin.x;
            draw_rect.origin.y += layer_bounds.origin.y;
            GBitmap* digit_bitmap = get_anim_image(info->size, segment_info->segment_angle, &draw_rect.size);
            graphics_draw_bitmap_in_rect(ctx, digit_bitmap, draw_rect);
        }
    }
}

static void digit_layer_anim_timer(void* data)
{
    DigitLayer* data_cast = (DigitLayer*) data;
    digit_info_t* info = (digit_info_t*) layer_get_data(data_cast);
    info->anim_timer = NULL;
    digit_layer_animate(data_cast);
    digit_layer_check_timer_status(data_cast);
}

static void digit_layer_check_timer_status(struct Layer* layer)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    if (info->current_number != info->target_number) {
        if (info->anim_timer == NULL) {
            info->anim_timer = app_timer_register(info->animate_speed, digit_layer_anim_timer, layer);
        }
    }
}

DigitLayer* digit_layer_create(digit_size_t size, GPoint offset, int max_number)
{
    GRect layer_rect;
    layer_rect.origin = offset;
    switch (size) {
        case DIGIT_BIG:
        layer_rect.size.w = BIG_WIDTH;
        layer_rect.size.h = BIG_HEIGHT;
        break;
        case DIGIT_SMALL:
        layer_rect.size.w = SMALL_WIDTH;
        layer_rect.size.h = SMALL_HEIGHT;
        break;
    }
    DigitLayer* result = layer_create_with_data(layer_rect, sizeof(digit_info_t));
    digit_info_t* info = (digit_info_t*) layer_get_data(result);
    info->size = size;
    info->current_number = 0;
    info->target_number = 0;
    info->current_anim = -1;
    info->current_anim_position = 0;
    info->anim_timer = NULL;
    info->max_number = max_number;
    info->animate_speed = DEFAULT_SPEED;
    info->quick_wrap = false;
    layer_set_update_proc(result, handle_layer_update);
    return result;
}

void digit_layer_set_quick_wrap(DigitLayer* layer, bool quick_wrap)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    info->quick_wrap = quick_wrap;
}

void digit_layer_set_animate_speed(DigitLayer* layer, int animate_interval)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    if (animate_interval == 0) {
        info->animate_speed = DEFAULT_SPEED;
    } else {
        info->animate_speed = animate_interval;
    }
}

void digit_layer_set_number(DigitLayer* layer, int target_number, bool animate)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    info->target_number = target_number % 10;
    if (!animate) {
        info->current_number = info->target_number;
        info->current_anim = -info->current_number - 1;
        info->current_anim_position = 0;
    }
    digit_layer_check_timer_status(layer);
}

bool digit_layer_need_animation(DigitLayer* layer)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    return info->current_number != info->target_number;
}

void digit_layer_animate(DigitLayer* layer)
{
    digit_info_t* info = (digit_info_t*) layer_get_data(layer);
    if (info->current_number == info->target_number) {
        return;
    }
    // Negative animation have only one frame (the static digit)
    if (++info->current_anim_position == 9 || info->current_anim < 0) {
        // Animation complete, go to the next one
        info->current_anim_position = 0;
        if (info->quick_wrap && info->current_number == info->max_number) {
            // Don't switch to the next animation but use the quick back to 0 one
            info->current_anim = anim_get_next_quick_anim(info->current_anim);
        } else {
            info->current_anim = anim_get_next_anim(info->current_anim);
            if (info->current_anim < 0) {
                info->current_number = -info->current_anim - 1;
            }
        }
    }
    layer_mark_dirty(layer);
}

void digit_layer_destroy(DigitLayer* layer)
{
    layer_destroy(layer);
}
