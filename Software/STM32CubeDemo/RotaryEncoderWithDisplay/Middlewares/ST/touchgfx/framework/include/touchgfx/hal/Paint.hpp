/******************************************************************************
* Copyright (c) 2018(-2024) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.23.2 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

/**
 * @file touchgfx/hal/Paint.hpp
 *
 * Declares paint functions for widgets
 */
#ifndef TOUCHGFX_PAINT_HPP
#define TOUCHGFX_PAINT_HPP

#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
namespace paint
{
/**
 * Sets L8 palette to be used by the painter. The content pointed to be parameter data depends
 * on the actual L8 format. L8RGB8888 assumes data points to three bytes per palette index,. L8ARGB8888 assumes four bytes per palette index.
 *
 * @param  data The palette data.
 */
void setL8Palette(const uint8_t* const data);

/** Tear down painter - wait for pending draw operations to finish. */
void tearDown(void);

namespace rgb565
{
/**
 * Draw a horizontal line (one pixel high) using the given color.
 *
 * @param [in] ptr      The pointer to the position in the framebuffer.
 * @param      count    Number of pixels to draw.
 * @param      color    The color.
 * @param      alpha    The alpha.
 * @param      color565 The color565 (same as 'color' but in native format for speed reasons).
 */
void lineFromColor(uint16_t* const ptr, const unsigned count, const uint32_t color, const uint8_t alpha, const uint32_t color565);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (RGB565
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The RGB565 data.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromRGB565(uint16_t* const ptr, const uint16_t* const data, const unsigned count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (ARGB8888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The ARGB8888 data.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromARGB8888(uint16_t* const ptr, const uint32_t* const data, const unsigned count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (L8RGB888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The palette indices.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromL8RGB888(uint16_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (L8ARGB8888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The palette indices.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromL8ARGB8888(uint16_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha);
} // namespace rgb565

namespace rgb888
{
/**
 * Draw a horizontal line (one pixel high) using the given color.
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      count Number of pixels to draw.
 * @param      color The color.
 * @param      alpha The alpha.
 */
void lineFromColor(uint8_t* const ptr, const unsigned count, const uint32_t color, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (RGB888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The RGB888 data.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromRGB888(uint8_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (ARGB8888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The ARGB8888 data.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromARGB8888(uint8_t* const ptr, const uint32_t* const data, const unsigned count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (L8RGB888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The palette indices.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromL8RGB888(uint8_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (L8ARGB8888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The palette indices.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromL8ARGB8888(uint8_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha);

} // namespace rgb888

namespace argb8888
{
/**
 * Draw a horizontal line (one pixel high) using the given color.
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      count Number of pixels to draw.
 * @param      color The color.
 * @param      alpha The alpha.
 */
void lineFromColor(uint32_t* const ptr, const int16_t count, const uint32_t color, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (RGB888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The RGB888 data.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromRGB888(uint8_t* const ptr, const uint8_t* const data, const int16_t count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (RGB565
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The RGB565 data.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromRGB565(uint8_t* const ptr, const uint16_t* const data, const int16_t count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (ARGB8888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The ARGB8888 data.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromARGB8888(uint8_t* const ptr, const uint32_t* const data, const int16_t count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (L8RGB888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The palette indices.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromL8RGB888(uint8_t* const ptr, const uint8_t* const data, const int16_t count, const uint8_t alpha);

/**
 * Draw a horizontal line (one pixel high) using pixels from the given data pointer (L8ARGB8888
 * data).
 *
 * @param [in] ptr   The pointer to the position in the framebuffer.
 * @param      data  The palette indices.
 * @param      count Number of pixels to draw.
 * @param      alpha The alpha.
 */
void lineFromL8ARGB8888(uint8_t* const ptr, const uint8_t* const data, const int16_t count, const uint8_t alpha);

} // namespace argb8888
} // namespace paint
} // namespace touchgfx

#endif // TOUCHGFX_PAINT_HPP
