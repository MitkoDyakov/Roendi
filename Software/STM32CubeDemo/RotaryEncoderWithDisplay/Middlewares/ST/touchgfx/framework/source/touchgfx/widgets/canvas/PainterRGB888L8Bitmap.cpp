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

#include <touchgfx/hal/Paint.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888L8Bitmap.hpp>

namespace touchgfx
{
void PainterRGB888L8Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::L8) && "PainterRGB888L8Bitmap only works with appropriate L8 bitmaps");
    if (bitmap.getId() != BITMAP_INVALID)
    {
        bitmapCLUT = bitmap.getExtraData();
        assert(bitmapData && bitmapCLUT && "The bitmap does not have any data or any CLUT");
        l8format = static_cast<Bitmap::ClutFormat>(*reinterpret_cast<const uint16_t*>(bitmapCLUT));
        assert((l8format == Bitmap::CLUT_FORMAT_L8_RGB565 || l8format == Bitmap::CLUT_FORMAT_L8_ARGB8888 || l8format == Bitmap::CLUT_FORMAT_L8_RGB888) && "The palette is not in the right format");
        bitmapCLUT += 4; // Skip header
    }
}

void PainterRGB888L8Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    uint8_t* RESTRICT framebuffer = destination + offset * 3;
    const uint8_t* const lineEnd = framebuffer + count * 3;
    const int32_t rowSkip = widgetY * bitmapRect.width;
    const uint8_t* const bitmapLineStart = bitmapData + rowSkip;
    const uint8_t* bitmapPointer = bitmapLineStart + widgetX;
    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    if (l8format == Bitmap::CLUT_FORMAT_L8_RGB888)
    {
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            count -= length;
            paint::rgb888::lineFromL8RGB888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += 3 * length;
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
            paint::rgb888::lineFromL8ARGB8888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += 3 * length;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
}

bool PainterRGB888L8Bitmap::setup(const Rect& widgetRect) const
{
    if (!AbstractPainterRGB888::setup(widgetRect))
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

void PainterRGB888L8Bitmap::tearDown() const
{
    paint::tearDown();
}

} // namespace touchgfx
