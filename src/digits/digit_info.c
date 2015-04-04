/** @file
 * General info about digits.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>

#include "digit_info.h"

// ===================
// PRIVATE VARIABLES =
// ===================

/** Polygon for BIG segment */
static
GPoint big_points[6] = {
    {2, 0},
    {6, -4},
    {31, -4},
    {35, 0},
    {31, 4},
    {6, 4}
};

/** Polygon for MEDIUM segment */
static
GPoint medium_points[6] = {
    {2, 0},
    {5, -3},
    {30, -3},
    {33, 0},
    {30, 3},
    {5, 3}
};

/** Polygon for SMALL segment */
static
GPoint small_points[6] = {
    {1, 0},
    {2, -1},
    {11, -1},
    {12, 0},
    {11, 1},
    {2, 1}
};

/** Polygons for the various segments size.
 *
 * All segment are positioned relative to their "hinge" point in animation.
 */
static
const struct GPathInfo segments_path[DIGITS_SIZE_COUNT] = {
    { .num_points = 6, .points = big_points },
    { .num_points = 6, .points = medium_points },
    { .num_points = 6, .points = small_points }
};

static
GPath* stored_paths[DIGITS_SIZE_COUNT] = {NULL, NULL, NULL};

static
unsigned stored_paths_refcount[DIGITS_SIZE_COUNT] = {0, 0, 0};

// ==================
// PUBLIC VARIABLES =
// ==================

const GSize digit_dimensions[DIGITS_SIZE_COUNT] = {
    { .w = 46, .h = 83 }, { .w = 42, .h = 77 }, { .w = 16, .h = 33 }
};

const unsigned digit_spacing[DIGITS_SIZE_COUNT] = {5, 3, 2};

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

GPath*
poly_aquire(digit_size_t digit_size)
{
    if (stored_paths_refcount[digit_size]++ == 0) {
        stored_paths[digit_size] = gpath_create(segments_path + digit_size);
    }

    return stored_paths[digit_size];
}

void
poly_release(digit_size_t digit_size)
{
    if (--stored_paths_refcount[digit_size] == 0) {
        gpath_destroy(stored_paths[digit_size]);
        stored_paths[digit_size] = NULL;
    }
}

