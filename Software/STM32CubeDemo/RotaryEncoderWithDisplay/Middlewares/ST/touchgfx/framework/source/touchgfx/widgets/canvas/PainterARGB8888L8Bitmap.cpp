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

#include <touchgfx/Color.hpp>
#include <touchgfx/hal/Paint.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterARGB8888L8Bitmap.hpp>

namespace touchgfx
{
void PainterARGB8888L8Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::L8) && "PainterARGB8888L8Bitmap only works with L8 bitmaps");
    if (bitmap.getId() != BITMAP_INVALID)
    {
        bitmapCLUT = bitmap.getExtraData();
        assert(bitmapData && bitmapCLUT && "The bitmap does not have any data or any CLUT");
        l8format = static_cast<Bitmap::ClutFormat>(*reinterpret_cast<const uint16_t*>(bitmapCLUT));
        assert((l8format == Bitmap::CLUT_FORMAT_L8_RGB565 || l8format == Bitmap::CLUT_FORMAT_L8_ARGB8888 || l8format == Bitmap::CLUT_FORMAT_L8_RGB888) && "The palette is not in the right format");
        bitmapCLUT += 4; // Skip header
    }
}

void PainterARGB8888L8Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    uint8_t* RESTRICT framebuffer = destination + offset * 4;
    const uint8_t* const lineEnd = framebuffer + count * 4;
    const int32_t rowSkip = widgetY * bitmapRect.width;
    const uint8_t* const bitmapLineStart = bitmapData + rowSkip;
    const uint8_t* bitmapPointer = bitmapLineStart + widgetX;
    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    if (l8format == Bitmap::CLUT_FORMAT_L8_RGB565)
    {
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            const uint8_t* const chunkend = framebuffer + length * 4;
            count -= length;
            if (alpha == 0xFF)
            {
                do
                {
                    const uint16_t srcpix = reinterpret_cast<const uint16_t*>(bitmapCLUT)[*bitmapPointer++];
                    *framebuffer++ = Color::getBlueFromRGB565(srcpix);
                    *framebuffer++ = Color::getGreenFromRGB565(srcpix);
                    *framebuffer++ = Color::getRedFromRGB565(srcpix);
                    *framebuffer++ = 0xFF;
                } while (framebuffer < chunkend);
            }
            else
            {
                do
                {
                    const uint16_t srcpix = reinterpret_cast<const uint16_t*>(bitmapCLUT)[*bitmapPointer++];
                    const uint8_t ialpha = 0xFF - alpha;
                    *framebuffer = LCD::div255(Color::getBlueFromRGB565(srcpix) * alpha + *framebuffer * ialpha);
                    framebuffer++;
                    *framebuffer = LCD::div255(Color::getGreenFromRGB565(srcpix) * alpha + *framebuffer * ialpha);
                    framebuffer++;
                    *framebuffer = LCD::div255(Color::getRedFromRGB565(srcpix) * alpha + *framebuffer * ialpha);
                    framebuffer++;
                    *framebuffer = *framebuffer + alpha - LCD::div255(*framebuffer * alpha);
                    framebuffer++;
                } while (framebuffer < chunkend);
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
            paint::argb8888::lineFromL8RGB888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += length * 4;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
    else // if (l8format == Bitmap::CLUT_FORMAT_L8_ARGB8888)
    {
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            count -= length;
            paint::argb8888::lineFromL8ARGB8888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += length * 4;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
}

bool PainterARGB8888L8Bitmap::setup(const Rect& widgetRect) const
{
    if (!AbstractPainterARGB8888::setup(widgetRect))
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

void PainterARGB8888L8Bitmap::tearDown() const
{
    paint::tearDown();
}
} // namespace touchgfx
