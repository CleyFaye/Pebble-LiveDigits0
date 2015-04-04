/** @file
 * Digit properties.
 *
 * Shared properties for digits.
 *
 * Segments are identified by a numeric identifier ranging from 0 to 6 in this
 * fashion:
 *   000000
 * 1  0000  2
 * 11      22
 * 11      22
 * 11      22
 * 1  3333  2
 *   333333
 * 4  3333  5
 * 44      55
 * 44      55
 * 44      55
 * 4  6666  4
 *   666666
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DIGIT_INFO_H
#define INCL_DIGIT_INFO_H

#include <pebble.h>

// =======
// TYPES =
// =======

/** The size of the digit */
typedef enum {
    DS_BIG = 0,
    DS_MEDIUM,
    DS_SMALL,
    DIGITS_SIZE_COUNT
} digit_size_t;

/** The digit animation speed. */
typedef enum {
    /** Slow, show every steps */
    SLOW_SEPARATE,
    /** Slow, show every other steps when two digit animation steps are required
     */
    SLOW_MERGED,
    /** Fast, show every steps */
    FAST_SEPARATE,
    /** Fast, show every other steps when two digit animation steps are
     * required.
     *
     * By design, the FAST_MERGED animations will always fit in slightly less
     * than a second for transition between two consecutive numbers.
     */
    FAST_MERGED
} animation_speed_t;

// =========
// EXTERNS =
// =========

/** The size (in pixel) of a digit.
 *
 * Each value correspond to an enum value of digit_size_t
 */
extern
const GSize digit_dimensions[];

/** Spacing between digits for each digit size. */
extern
const unsigned digit_spacing[DIGITS_SIZE_COUNT];

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Get a path for a given segment size.
 *
 * The GPath object is shared.
 */
GPath*
poly_aquire(digit_size_t digit_size);

/** Release a path for a given segment size. */
void
poly_release(digit_size_t digit_size);

#endif

