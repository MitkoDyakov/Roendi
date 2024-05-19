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
#include <touchgfx/widgets/canvas/PainterXRGB8888L8Bitmap.hpp>

namespace touchgfx
{
void PainterXRGB8888L8Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::L8) && "PainterXRGB8888L8Bitmap only works with L8 bitmaps");
    if (bitmap.getId() != BITMAP_INVALID)
    {
        bitmapCLUT = bitmap.getExtraData();
        assert(bitmapData && bitmapCLUT && "The bitmap does not have any data or any CLUT");
        l8format = static_cast<Bitmap::ClutFormat>(*reinterpret_cast<const uint16_t*>(bitmapCLUT));
        assert((l8format == Bitmap::CLUT_FORMAT_L8_RGB565 || l8format == Bitmap::CLUT_FORMAT_L8_ARGB8888 || l8format == Bitmap::CLUT_FORMAT_L8_RGB888) && "The palette is not in the right format");
        bitmapCLUT += 4; // Skip header
    }
}

void PainterXRGB8888L8Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    uint32_t* RESTRICT framebuffer = reinterpret_cast<uint32_t*>(destination) + offset;
    const uint32_t* const lineEnd = framebuffer + count;
    const int32_t rowSkip = widgetY * bitmapRect.width;
    const uint8_t* const bitmapLineStart = bitmapData + rowSkip;
    const uint8_t* bitmapPointer = bitmapLineStart + widgetX;
    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    if (l8format == Bitmap::CLUT_FORMAT_L8_RGB565)
    {
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            const uint32_t* const chunkend = framebuffer + length;
            count -= length;
            if (alpha == 0xFF)
            {
                do
                {
                    const uint16_t srcpix = reinterpret_cast<const uint16_t*>(bitmapCLUT)[*bitmapPointer++];
                    *framebuffer = Color::rgb565toXrgb8888(srcpix);
                } while (++framebuffer < chunkend);
            }
            else
            {
                do
                {
                    const uint16_t srcpix = reinterpret_cast<const uint16_t*>(bitmapCLUT)[*bitmapPointer++];
                    *framebuffer = LCD32bpp_XRGB8888::blendRgb565withXrgb8888(srcpix, *framebuffer, alpha, 0xFF - alpha);
                } while (++framebuffer < chunkend);
            }
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
    else if (l8format == Bitmap::CLUT_FORMAT_L8_RGB888)
    {
        if (alpha == 0xFF)
        {
            do
            {
                const int16_t length = MIN(bitmapAvailable, count);
                const uint32_t* const chunkend = framebuffer + length;
                count -= length;
                do
                {
                    const uint8_t* src = bitmapCLUT + *bitmapPointer++ * 3;
                    *framebuffer = LCD32bpp_XRGB8888::rgb888toXrgb8888(src);
                } while (++framebuffer < chunkend);
                bitmapPointer = bitmapLineStart;
                bitmapAvailable = bitmapRect.width;
            } while (framebuffer < lineEnd);
        }
        else
        {
            do
            {
                const int16_t length = MIN(bitmapAvailable, count);
                const uint32_t* const chunkend = framebuffer + length;
                count -= length;
                do
                {
                    const uint8_t* src = bitmapCLUT + *bitmapPointer++ * 3;
                    *framebuffer = LCD32bpp_XRGB8888::blendRgb888withXrgb8888(src, *framebuffer, alpha, 0xFF - alpha);
                } while (++framebuffer < chunkend);
                bitmapPointer = bitmapLineStart;
                bitmapAvailable = bitmapRect.width;
            } while (framebuffer < lineEnd);
        }
    }
    else // if (l8format == Bitmap::CLUT_FORMAT_L8_ARGB8888)
    {
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            const uint32_t* const chunkend = framebuffer + length;
            count -= length;
            do
            {
                const uint32_t src = reinterpret_cast<const uint32_t*>(bitmapCLUT)[*bitmapPointer++];
                const uint8_t srcAlpha = src >> 24;
                const uint8_t a = LCD::div255(alpha * srcAlpha);
                if (a == 0xFF)
                {
                    *framebuffer = src;
                }
                else
                {
                    *framebuffer = LCD32bpp_XRGB8888::blendXrgb888withXrgb8888(src, *framebuffer, a, 0xFF - a);
                }
                framebuffer++;
            } while (framebuffer < chunkend);
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
}
} // namespace touchgfx
