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

#include <platform/driver/lcd/LCD32bpp_XRGB8888.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterXRGB8888Bitmap.hpp>

namespace touchgfx
{
void PainterXRGB8888Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmapFormat == Bitmap::RGB565 || bitmapFormat == Bitmap::RGB888 || bitmapFormat == Bitmap::ARGB8888) && !bitmap.getExtraData() && "PainterXRGB8888Bitmap only works with RGB565 (without alpha), RGB888 and ARGB8888 bitmaps");
    assert(bitmap.getId() == BITMAP_INVALID || bitmapData);
}

void PainterXRGB8888Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    uint32_t* RESTRICT framebuffer = reinterpret_cast<uint32_t*>(destination) + offset;
    const uint32_t* const lineEnd = framebuffer + count;
    const int32_t rowSkip = widgetY * bitmapRect.width;
    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    if (bitmapFormat == Bitmap::ARGB8888)
    {
        const uint32_t* const bitmapLineStart = reinterpret_cast<const uint32_t*>(bitmapData) + rowSkip;
        const uint32_t* bitmapPointer = bitmapLineStart + widgetX;
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            const uint32_t* const chunkend = framebuffer + length;
            count -= length;
            do
            {
                const uint8_t srcAlpha = (*bitmapPointer) >> 24;
                const uint8_t a = LCD::div255(alpha * srcAlpha);
                if (a == 0xFF)
                {
                    *framebuffer = *bitmapPointer;
                }
                else
                {
                    *framebuffer = LCD32bpp_XRGB8888::blendRgb888withXrgb8888(reinterpret_cast<const uint8_t*>(bitmapPointer), *framebuffer, a, 0xFF - a);
                }
                bitmapPointer++;
            } while (++framebuffer < chunkend);
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
    else if (bitmapFormat == Bitmap::RGB888)
    {
        const uint8_t* const bitmapLineStart = reinterpret_cast<const uint8_t*>(bitmapData) + rowSkip * 3;
        const uint8_t* bitmapPointer = bitmapLineStart + widgetX * 3;
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            const uint32_t* const chunkend = framebuffer + length;
            count -= length;
            if (alpha == 0xFF)
            {
                do
                {
                    *framebuffer = LCD32bpp_XRGB8888::rgb888toXrgb8888(bitmapPointer);
                    bitmapPointer += 3;
                } while (++framebuffer < chunkend);
            }
            else
            {
                do
                {
                    *framebuffer = LCD32bpp_XRGB8888::blendRgb888withXrgb8888(bitmapPointer, *framebuffer, alpha, 0xFF - alpha);
                    bitmapPointer += 3;
                } while (++framebuffer < chunkend);
            }
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
    else // if (bitmapFormat == Bitmap::RGB565)
    {
        const uint16_t* const bitmapLineStart = reinterpret_cast<const uint16_t*>(bitmapData) + (widgetY * bitmapRect.width);
        const uint16_t* bitmapPointer = bitmapLineStart + widgetX;
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            const uint32_t* const chunkend = framebuffer + length;
            count -= length;
            if (alpha == 0xFF)
            {
                do
                {
                    *framebuffer = Color::rgb565toXrgb8888(*bitmapPointer++);
                } while (++framebuffer < chunkend);
            }
            else
            {
                do
                {
                    *framebuffer = LCD32bpp_XRGB8888::blendRgb565withXrgb8888(*bitmapPointer++, *framebuffer, alpha, 0xFF - alpha);
                } while (++framebuffer < chunkend);
            }
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
}
} // namespace touchgfx
