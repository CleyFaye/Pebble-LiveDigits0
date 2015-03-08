/** @file
 * Code to load/use mono-color bitmaps with aliasing.
 *
 * Here, a mono-color bitmap is a bitmap with one dominant color. It can have
 * three shades of said color, gradually going from a "front" color to a
 * "background" color. It can also have transparent pixels.
 *
 * Such bitmaps (nicknammed MBP) can be created with the python script found in
 * the extra directory called "pngtombp.py". More details about the data format
 * are found in this script.
 *
 * @author Cley Faye
 * Licensing informations in LICENSE.md file.
 */

#ifndef INCL_MONOBITMAP_H
#define INCL_MONOBITMAP_H

// =======
// TYPES =
// =======

/** Available colors for mono bitmaps */
typedef enum {
    MONO_BLACK,
    MONO_WHITE,
    MONO_RED,
    MONO_ORANGE,
    MONO_YELLOW,
    MONO_GREEN,
    MONO_DARKGREEN,
    MONO_CYAN,
    MONO_BLUE,
    MONO_DEEPBLUE,
    MONO_PURPLE,
    MONO_MAGENTA
} MonoColor;

/** Opaque handle for a mono bitmap */
typedef struct monobitmap_s* MonoBitmap;

// ===============================
// PUBLIC FUNCTIONS DECLARATIONS =
// ===============================

/** Return the primary GColor associated with a value of MonoColor */
GColor monobitmap_color_from_monocolor(MonoColor color);

/** Create a new mono bitmap from a resource.
 *
 * The resulting bitmap will transition from @a frontColor to @a backColor using
 * 4 steps, the fourth one being transparent.
 *
 * There is a caching mechanism. Calling this function multiple time with the
 * same parameters will return the same object. Each call to
 * monobitmap_create_with_resource() must correspond to a call to
 * monobitmap_destroy().
 *
 * On BW display, only the front color is visible. Everything else is
 * transparent.
 *
 * @param frontColor The main color of the bitmap
 * @param backColor The background color
 */
MonoBitmap
monobitmap_create_with_resource(uint32_t resource_id,
                                MonoColor frontColor,
                                MonoColor backColor,
                                bool vertical_flip,
                                bool horizontal_flip);

/** Return the size of a monobitmap */
GSize
monobitmap_get_size(const MonoBitmap bitmap);

/** Draw a mono bitmap into a graphic context.
 *
 * Note that although the bitmap can show "anti aliasing" in itself, overlaping
 * bitmaps are drawn independently and will not blend well.
 */
void
monobitmap_draw_bitmap_in_rect(GContext* ctx,
                               const MonoBitmap bitmap,
                               GRect rect);

/** Draw a mono bitmap into a graphic context. */
static inline
void
monobitmap_draw_bitmap(GContext* ctx,
                       const MonoBitmap bitmap,
                       GPoint offset)
{
    monobitmap_draw_bitmap_in_rect(ctx,
                                   bitmap,
    (GRect) {
        offset, monobitmap_get_size(bitmap)
    });
}

/** Release a monobitmap resource. */
void
monobitmap_destroy(MonoBitmap bitmap);

/** Commit resource release.
 *
 * When monobitmap_destroy() is called, the resources behind the handler are not
 * immediately discarded. Instead, the actual free operation happens when you
 * call monobitmap_commit_destroy().
 */
void
monobitmap_commit_destroy(void);

#endif

