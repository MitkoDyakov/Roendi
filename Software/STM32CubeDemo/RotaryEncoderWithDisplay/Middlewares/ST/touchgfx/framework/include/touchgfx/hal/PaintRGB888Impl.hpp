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
 * @file touchgfx/hal/PaintRGB888Impl.hpp
 *
 * Implements RGB888 software painter functions for widgets
 */
#ifndef TOUCHGFX_PAINTRGB888IMPL_HPP
#define TOUCHGFX_PAINTRGB888IMPL_HPP

#include <touchgfx/Color.hpp>
#include <touchgfx/hal/Paint.hpp>
#include <touchgfx/hal/PaintImpl.hpp>

namespace touchgfx
{
namespace paint
{
namespace rgb888
{
void lineFromColor(uint8_t* const ptr, const unsigned count, const uint32_t color, const uint8_t alpha)
{
    uint8_t* framebuffer = ptr;
    const uint8_t* const lineEnd = framebuffer + count * 3;
    const uint8_t painterRed = Color::getRed(color);
    const uint8_t painterGreen = Color::getGreen(color);
    const uint8_t painterBlue = Color::getBlue(color);

    if (alpha == 0xFF)
    {
        do
        {
            *framebuffer++ = painterBlue;
            *framebuffer++ = painterGreen;
            *framebuffer++ = painterRed;
        } while (framebuffer < lineEnd);
    }
    else
    {
        do
        {
            const uint8_t ialpha = 0xFF - alpha;
            uint8_t pByte = *framebuffer;
            *framebuffer++ = LCD::div255(painterBlue * alpha + pByte * ialpha);
            pByte = *framebuffer;
            *framebuffer++ = LCD::div255(painterGreen * alpha + pByte * ialpha);
            pByte = *framebuffer;
            *framebuffer++ = LCD::div255(painterRed * alpha + pByte * ialpha);
        } while (framebuffer < lineEnd);
    }
}

void lineFromRGB888(uint8_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha)
{
    uint8_t* framebuffer = ptr;
    const uint8_t* bitmapPointer = data;
    const uint8_t* const chunkend = framebuffer + count * 3;
    if (alpha == 0xFF)
    {
        do
        {
            *framebuffer++ = *bitmapPointer++;
            *framebuffer++ = *bitmapPointer++;
            *framebuffer++ = *bitmapPointer++;
        } while (framebuffer < chunkend);
    }
    else
    {
        do
        {
            const uint8_t ialpha = 0xFF - alpha;
            *framebuffer = LCD::div255(*bitmapPointer++ * alpha + *framebuffer * ialpha);
            framebuffer++;
            *framebuffer = LCD::div255(*bitmapPointer++ * alpha + *framebuffer * ialpha);
            framebuffer++;
            *framebuffer = LCD::div255(*bitmapPointer++ * alpha + *framebuffer * ialpha);
            framebuffer++;
        } while (framebuffer < chunkend);
    }
}

void lineFromARGB8888(uint8_t* const ptr, const uint32_t* const data, const unsigned count, const uint8_t alpha)
{
    uint8_t* framebuffer = ptr;
    const uint32_t* bitmapPointer = data;
    const uint8_t* const chunkend = framebuffer + count * 3;
    do
    {
        const uint32_t argb = *bitmapPointer++;
        const uint8_t a = LCD::div255(alpha * (argb >> 24));
        if (a == 0xFF)
        {
            *framebuffer++ = argb;       // Blue
            *framebuffer++ = argb >> 8;  // Green
            *framebuffer++ = argb >> 16; // Red
        }
        else
        {
            const uint8_t ialpha = 0xFF - a;
            uint8_t cByte = argb;
            *framebuffer = LCD::div255(cByte * a + *framebuffer * ialpha);
            framebuffer++;
            cByte = argb >> 8;
            *framebuffer = LCD::div255(cByte * a + *framebuffer * ialpha);
            framebuffer++;
            cByte = argb >> 16;
            *framebuffer = LCD::div255(cByte * a + *framebuffer * ialpha);
            framebuffer++;
        }
    } while (framebuffer < chunkend);
}

void lineFromL8RGB888(uint8_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha)
{
    uint8_t* framebuffer = ptr;
    const uint8_t* bitmapPointer = data;
    const uint8_t* const chunkend = framebuffer + count * 3;
    if (alpha == 0xFF)
    {
        do
        {
            const uint8_t* src = &blendL8CLUT[*bitmapPointer++ * 3];
            *framebuffer++ = *src++;
            *framebuffer++ = *src++;
            *framebuffer++ = *src;
        } while (framebuffer < chunkend);
    }
    else
    {
        do
        {
            const uint8_t* src = &blendL8CLUT[*bitmapPointer++ * 3];
            const uint8_t ialpha = 0xFF - alpha;
            *framebuffer = LCD::div255(*src++ * alpha + *framebuffer * ialpha);
            framebuffer++;
            *framebuffer = LCD::div255(*src++ * alpha + *framebuffer * ialpha);
            framebuffer++;
            *framebuffer = LCD::div255(*src * alpha + *framebuffer * ialpha);
            framebuffer++;
        } while (framebuffer < chunkend);
    }
}

void lineFromL8ARGB8888(uint8_t* const ptr, const uint8_t* const data, const unsigned count, const uint8_t alpha)
{
    uint8_t* framebuffer = ptr;
    const uint8_t* bitmapPointer = data;
    const uint8_t* const chunkend = framebuffer + count * 3;
    do
    {
        uint32_t src = reinterpret_cast<const uint32_t*>(blendL8CLUT)[*bitmapPointer++];
        const uint8_t srcAlpha = src >> 24;
        const uint8_t a = LCD::div255(alpha * srcAlpha);
        if (a == 0xFF)
        {
            *framebuffer++ = src;       // Blue
            *framebuffer++ = src >> 8;  // Green
            *framebuffer++ = src >> 16; // Red
        }
        else
        {
            const uint8_t ialpha = 0xFF - a;
            *framebuffer = LCD::div255((src & 0xFF) * a + *framebuffer * ialpha);
            framebuffer++;
            *framebuffer = LCD::div255(((src >> 8) & 0xFF) * a + *framebuffer * ialpha);
            framebuffer++;
            *framebuffer = LCD::div255(((src >> 16) & 0xFF) * a + *framebuffer * ialpha);
            framebuffer++;
        }
    } while (framebuffer < chunkend);
}

} // namespace rgb888
} // namespace paint
} // namespace touchgfx

#endif // TOUCHGFX_PAINTRGB888IMPL_HPP
