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

// GSize
#include <pebble.h>

// =======
// TYPES =
// =======

/** The orientation of a digit segment. */
typedef enum {
    SO_VERTICAL = 0,
    SO_0 = SO_VERTICAL,
    SO_9,
    SO_18,
    SO_27,
    SO_36,
    SO_45,
    SO_54,
    SO_63,
    SO_72,
    SO_81,
    SO_HORIZONTAL,
    SO_90 = SO_HORIZONTAL,
    SO_99,
    SO_108,
    SO_117,
    SO_126,
    SO_135,
    SO_144,
    SO_153,
    SO_162,
    SO_171,
    SEGMENTS_ORIENTATION_COUNT
} segment_orientation_t;

/** The size of the digit */
typedef enum {
    DS_BIG = 0,
    DS_MEDIUM,
    DIGITS_SIZE_COUNT
} digit_size_t;

// =========
// EXTERNS =
// =========

/** The size (in pixel) of digits.
 *
 * Each value correspond to an enum value of digit_size_t
 */
extern const GSize digit_dimensions[];

#endif

