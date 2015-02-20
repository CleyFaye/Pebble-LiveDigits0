/** @file
 * General info about digits.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

// GSize
// ResourceId
#include <pebble.h>
// Associated header
#include "digit_info.h"

// ===================
// PRIVATE VARIABLES =
// ===================

/** Segment resource identifiers for big digits */
const ResourceId big_segment_res_ids[SEGMENTS_ORIENTATION_COUNT] = {
    RESOURCE_ID_BIGDIGIT_VERTICAL,
    RESOURCE_ID_SEGMENT_BIG_0,
    RESOURCE_ID_SEGMENT_BIG_1,
    RESOURCE_ID_SEGMENT_BIG_2,
    RESOURCE_ID_SEGMENT_BIG_3,
    RESOURCE_ID_SEGMENT_BIG_4,
    RESOURCE_ID_SEGMENT_BIG_5,
    RESOURCE_ID_SEGMENT_BIG_6,
    RESOURCE_ID_SEGMENT_BIG_7,
    RESOURCE_ID_SEGMENT_BIG_8,
    RESOURCE_ID_BIGDIGIT_HORIZONTAL,
    RESOURCE_ID_SEGMENT_BIG_9,
    RESOURCE_ID_SEGMENT_BIG_10,
    RESOURCE_ID_SEGMENT_BIG_11,
    RESOURCE_ID_SEGMENT_BIG_12,
    RESOURCE_ID_SEGMENT_BIG_13,
    RESOURCE_ID_SEGMENT_BIG_14,
    RESOURCE_ID_SEGMENT_BIG_15,
    RESOURCE_ID_SEGMENT_BIG_16,
    RESOURCE_ID_SEGMENT_BIG_17
};

/** Segment resource identifiers for medium digits */
const ResourceId medium_segment_res_ids[SEGMENTS_ORIENTATION_COUNT] = {
    RESOURCE_ID_MEDIUMDIGIT_VERTICAL,
    RESOURCE_ID_SEGMENT_MEDIUM_0,
    RESOURCE_ID_SEGMENT_MEDIUM_1,
    RESOURCE_ID_SEGMENT_MEDIUM_2,
    RESOURCE_ID_SEGMENT_MEDIUM_3,
    RESOURCE_ID_SEGMENT_MEDIUM_4,
    RESOURCE_ID_SEGMENT_MEDIUM_5,
    RESOURCE_ID_SEGMENT_MEDIUM_6,
    RESOURCE_ID_SEGMENT_MEDIUM_7,
    RESOURCE_ID_SEGMENT_MEDIUM_8,
    RESOURCE_ID_MEDIUMDIGIT_HORIZONTAL,
    RESOURCE_ID_SEGMENT_MEDIUM_9,
    RESOURCE_ID_SEGMENT_MEDIUM_10,
    RESOURCE_ID_SEGMENT_MEDIUM_11,
    RESOURCE_ID_SEGMENT_MEDIUM_12,
    RESOURCE_ID_SEGMENT_MEDIUM_13,
    RESOURCE_ID_SEGMENT_MEDIUM_14,
    RESOURCE_ID_SEGMENT_MEDIUM_15,
    RESOURCE_ID_SEGMENT_MEDIUM_16,
    RESOURCE_ID_SEGMENT_MEDIUM_17
};

// ==================
// PUBLIC VARIABLES =
// ==================

const GSize digit_dimensions[DIGITS_SIZE_COUNT] = {
    { .w = 46, .h = 83 },
    { .w = 42, .h = 77 }
};

const ResourceId* const segment_res_ids[DIGITS_SIZE_COUNT] = {
    big_segment_res_ids,
    medium_segment_res_ids
};

