/** @file
 * Helper code to read a resource bit by bit.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_BITRES_H
#define INCL_BITRES_H

#include <pebble.h>

// =======
// TYPES =
// =======

/** Opaque handler to access the resource bitstream */
typedef struct bitres_s* bitres_t;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Open a resource bitstream.
 *
 * Note: no boundary checks are performed on the resource. Don't overread.
 *
 * @return A handler that must be closed with a call to bitres_close()
 */
bitres_t
bitres_open(uint32_t resource_id);

/** Read the next bit of a bitstream. */
bool
bitres_read(bitres_t bitres);

/** Read a byte from a bitstream.
 *
 * This will read the requested number of bits and return a byte corresponding
 * to the reading. Reading more than 8 bits is possible but useless as the final
 * value is truncated.
 *
 * @param bit_count The number of bits to read
 * @return The byte representation of the read bits.
 */
uint8_t
bitres_readbyte(bitres_t bitres, size_t bit_count);

/** Close a bitstream. */
static inline
void
bitres_close(bitres_t bitres)
{
    free(bitres);
}

#endif

