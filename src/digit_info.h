/** @file
 * Digit properties.
 *
 * Shared properties for digits.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_DIGIT_INFO_H
#define INCL_DIGIT_INFO_H

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

#endif

