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

#include <platform/driver/lcd/LCD16bpp.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/hal/Paint.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565L8Bitmap.hpp>

namespace touchgfx
{
void PainterRGB565L8Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::L8) && "PainterRGB565L8Bitmap only works with appropriate L8 bitmaps");
    if (bitmap.getId() != BITMAP_INVALID)
    {
        bitmapCLUT = bitmap.getExtraData();
        assert(bitmapData && bitmapCLUT && "The bitmap does not have any data or any CLUT");
        l8format = static_cast<Bitmap::ClutFormat>(*reinterpret_cast<const uint16_t*>(bitmapCLUT));
        assert((l8format == Bitmap::CLUT_FORMAT_L8_RGB565 || l8format == Bitmap::CLUT_FORMAT_L8_ARGB8888 || l8format == Bitmap::CLUT_FORMAT_L8_RGB888) && "The palette is not in the right format");
        bitmapCLUT += 4; // Skip header
    }
}

void PainterRGB565L8Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    uint16_t* RESTRICT framebuffer = reinterpret_cast<uint16_t*>(destination) + offset;
    const uint16_t* const lineEnd = framebuffer + count;
    const int32_t rowSkip = widgetY * bitmapRect.width;
    const uint8_t* const bitmapLineStart = bitmapData + rowSkip;
    const uint8_t* bitmapPointer = bitmapLineStart + widgetX;
    // Max number of pixels before we reach end of bitmap row
    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    if (l8format == Bitmap::CLUT_FORMAT_L8_RGB565)
    {
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            const uint16_t* const chunkend = framebuffer + length;
            count -= length;
            if (alpha == 0xFF)
            {
                do
                {
                    *framebuffer = reinterpret_cast<const uint16_t*>(bitmapCLUT)[*bitmapPointer++];
                } while (++framebuffer < chunkend);
            }
            else
            {
                do
                {
                    *framebuffer = alphaBlend(reinterpret_cast<const uint16_t*>(bitmapCLUT)[*bitmapPointer++], *framebuffer, alpha);
                } while (++framebuffer < chunkend);
            }
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
    else if (l8format == Bitmap::CLUT_FORMAT_L8_RGB888)
    {
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            count -= length;
            paint::rgb565::lineFromL8RGB888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += length;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
    else // Bitmap::CLUT_FORMAT_L8_ARGB8888
    {
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            count -= length;
            paint::rgb565::lineFromL8ARGB8888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += length;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
}

bool PainterRGB565L8Bitmap::setup(const Rect& widgetRect) const
{
    if (!AbstractPainterRGB565::setup(widgetRect))
    {
        return false;
    }
    updateBitmapOffsets(widgetWidth);
    if (bitmap.getId() != BITMAP_INVALID)
    {
        paint::setL8Palette(bitmapCLUT);
        return true;
    }
    return false;
}

void PainterRGB565L8Bitmap::tearDown() const
{
    paint::tearDown();
}
} // namespace touchgfx
