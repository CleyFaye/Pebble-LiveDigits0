/** @file
 * Access a resource as a bit stream.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#include <pebble.h>
#include "bitres.h"

// ===============
// PRIVATE TYPES =
// ===============

/** State of the bitstream. */
struct bitres_s {
    /** Next bit to read. */
    unsigned bit_id;
    /** Actual resource data */
    uint8_t buffer[0];
};

// ==============================
// PUBLIC FUNCTIONS DEFINITIONS =
// ==============================

bitres_t
bitres_open(uint32_t resource_id)
{
    ResHandle res_handle = resource_get_handle(resource_id);
    size_t res_size = resource_size(res_handle);
    bitres_t result = malloc(sizeof(struct bitres_s) + res_size);
    result->bit_id = 0;
    resource_load(res_handle,
                  result->buffer,
                  res_size);
    return result;
}

bool
bitres_read(bitres_t bitres)
{
    unsigned byte_id = bitres->bit_id / 8;
    unsigned bit_id = bitres->bit_id % 8;
    ++bitres->bit_id;
    return (bitres->buffer[byte_id] & (1 << bit_id)) != 0;
}

uint8_t
bitres_readbyte(bitres_t bitres,
                size_t bit_count)
{
    uint8_t res = 0;

    for (unsigned i = 0;
         i < bit_count;
         ++i) {
        if (bitres_read(bitres)) {
            res += 1 << i;
        }
    }

    return res;
}

