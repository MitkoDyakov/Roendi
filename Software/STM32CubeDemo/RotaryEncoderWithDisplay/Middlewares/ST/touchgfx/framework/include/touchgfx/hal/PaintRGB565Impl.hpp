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
 * @file touchgfx/hal/PaintRGB565Impl.hpp
 *
 * Implements RGB565 software painter functions for widgets
 */
#ifndef TOUCHGFX_PAINTRGB565IMPL_HPP
#define TOUCHGFX_PAINTRGB565IMPL_HPP

#include <touchgfx/Color.hpp>
#include <touchgfx/hal/Paint.hpp>
#include <touchgfx/hal/PaintImpl.hpp>

namespace touchgfx
{
namespace paint
{
namespace rgb565
{
namespace
{
const uint16_t RMASK = 0xF800; // Mask for red   (1111100000000000)
const uint16_t GMASK = 0x07E0; // Mask for green (0000011111100000)
const uint16_t BMASK = 0x001F; // Mask for blue  (0000000000011111)
} //namespace

/**
 * Mix colors from a new pixel and a buffer pixel with the given alpha applied to the
 * new pixel, and the inverse alpha applied to the buffer pixel.
 *
 * @param  R      The red color (0-31 shifted into RMASK).
 * @param  G      The green color (0-63 shifted into GMASK).
 * @param  B      The blue color (0-31 shifted into BMASK).
 * @param  bufpix The buffer pixel value.
 * @param  alpha  The alpha of the R,G,B.
 *
 * @return The result of blending the two colors into a new color.
 */
FORCE_INLINE_FUNCTION uint16_t alphaBlend(uint16_t R, uint16_t G, uint16_t B, uint16_t bufpix, uint8_t alpha)
{
    const uint8_t ialpha = 0xFF - alpha;
    return (((R * alpha + (bufpix & RMASK) * ialpha) / 255) & RMASK) |
           (((G * alpha + (bufpix & GMASK) * ialpha) / 255) & GMASK) |
           (((B * alpha + (bufpix & BMASK) * ialpha) / 255) & BMASK);
}

/**
 * Mix colors from a new pixel and a buffer pixel with the given alpha applied to the
 * new pixel, and the inverse alpha applied to the buffer pixel.
 *
 * @param  newpix The new pixel value.
 * @param  bufpix The buffer pixel value.
 * @param  alpha  The alpha to apply to the new pixel.
 *
 * @return The result of blending the two colors into a new color.
 */
FORCE_INLINE_FUNCTION uint16_t alphaBlend(uint16_t newpix, uint16_t bufpix, uint8_t alpha)
{
    return alphaBlend(newpix & RMASK, newpix & GMASK, newpix & BMASK, bufpix, alpha);
}

/**
 * Generates a color representation to be used on the LCD, based on 24 bit RGB values.
 *
 * @param  color The color.
 *
 * @return The color representation depending on LCD color format.
 */
FORCE_INLINE_FUNCTION uint16_t getNativeColor(colortype color)
{
    return ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
}

void lineFromColor(uint16_t* const ptr, const unsigned count, const uint32_t color, const uint8_t alpha, const uint32_t color565)
{
    uint16_t* framebuffer = ptr;
    const uint16_t* const lineEnd = framebuffer + count;
    if (alpha == 0xFF)
    {
        do
        {
            *framebuffer = color565;
        } while (++framebuffer < lineEnd);
    }
    else
    {
        do
        {
            *framebuffer = alphaBlend(color565, *framebuffer, alpha);
        } while (++framebuffer < lineEnd);
    }
}

void lineFromRGB565(uint16_t* const ptr, const uint16_t* const data, const unsigned count, const uint8_t alpha)
{
    uint16_t* framebuffer = ptr;
    const uint16_t* bitmapPointer = data;
    const uint16_t* const chunkend = framebuffer + count;

    if (alpha == 0xFF)
    {
        do
        {
            *framebuffer = *bitmapPointer++;
        } while (++framebuffer < chunkend);
    }
    else
    {
        do
        {
            *framebuffer = alphaBlend(*bitmapPointer++, *framebuffer, alpha);
        } while (++framebuffer < chunkend);
    }
}

void lineFromARGB8888(uint16_t* const ptr, const uint32_t* const data, const unsigned count, const uint8_t alpha)
{
    uint16_t* framebuffer = ptr;
    const uint32_t* bitmapPointer = data;
    const uint16_t* const chunkend = framebuffer + count;
    do
    {
        const uint8_t srcAlpha = (*bitmapPointer) >> 24;
        const uint8_t a = LCD::div255(alpha * srcAlpha);
        if (a == 0xFF)
        {
            *framebuffer = getNativeColor(*bitmapPointer);
        }
        else if (a)
        {
            const uint32_t newpix = *bitmapPointer;
            *framebuffer = alphaBlend((newpix >> 8) & RMASK, (newpix >> 5) & GMASK, (newpix >> 3) & BMASK, *framebuffer, a);
        }
        bitmapPointer++;
    } while (++framebuffer < chunkend);
}

void lineFromL8RGB888(uint16_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha)
{
    uint16_t* framebuffer = ptr;
    const uint8_t* bitmapPointer = data;
    const uint16_t* const chunkend = framebuffer + count;
    if (alpha == 0xFF)
    {
        do
        {
            const uint8_t* src = &blendL8CLUT[*bitmapPointer++ * 3];
            // Use alpha from covers directly
            const uint8_t blue = *src++;
            const uint8_t green = *src++;
            const uint8_t red = *src;
            *framebuffer = ((red << 8) & RMASK) | ((green << 3) & GMASK) | ((blue >> 3) & BMASK);
        } while (++framebuffer < chunkend);
    }
    else
    {
        do
        {
            const uint8_t* src = &blendL8CLUT[*bitmapPointer++ * 3];
            // Use alpha from covers directly
            const uint8_t blue = *src++;
            const uint8_t green = *src++;
            const uint8_t red = *src;
            const uint8_t ialpha = 0xFF - alpha;
            const uint16_t bufpix = *framebuffer;
            uint8_t fbr = Color::getRedFromRGB565(bufpix);
            uint8_t fbg = Color::getGreenFromRGB565(bufpix);
            uint8_t fbb = Color::getBlueFromRGB565(bufpix);
            *framebuffer = ((LCD::div255(red * alpha + fbr * ialpha) << 8) & RMASK) | ((LCD::div255(green * alpha + fbg * ialpha) << 3) & GMASK) | ((LCD::div255(blue * alpha + fbb * ialpha) >> 3) & BMASK);
        } while (++framebuffer < chunkend);
    }
}

void lineFromL8ARGB8888(uint16_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha)
{
    uint16_t* framebuffer = ptr;
    const uint8_t* bitmapPointer = data;
    const uint16_t* const chunkend = framebuffer + count;
    do
    {
        const uint32_t newpix = reinterpret_cast<const uint32_t*>(blendL8CLUT)[*bitmapPointer++];
        const uint8_t srcAlpha = newpix >> 24;
        const uint8_t a = LCD::div255(alpha * srcAlpha);
        if (a == 0xFF)
        {
            *framebuffer = getNativeColor(newpix);
        }
        else if (a)
        {
            *framebuffer = alphaBlend((newpix >> 8) & RMASK, (newpix >> 5) & GMASK, (newpix >> 3) & BMASK, *framebuffer, a);
        }
    } while (++framebuffer < chunkend);
}

} // namespace rgb565
} // namespace paint
} // namespace touchgfx

#endif // TOUCHGFX_PAINTRGB565IMPL_HPP
