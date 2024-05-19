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

#include <touchgfx/hal/DMA.hpp>
#include <touchgfx/hal/Paint.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888Bitmap.hpp>

namespace touchgfx
{
void PainterRGB888Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmapFormat == Bitmap::RGB888 || bitmapFormat == Bitmap::ARGB8888) && "PainterRGB888Bitmap only works with RGB888 and ARGB8888 bitmaps");
    assert(bitmap.getId() == BITMAP_INVALID || bitmapData);
}

void PainterRGB888Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    uint8_t* RESTRICT framebuffer = destination + offset * 3;
    const uint8_t* const lineEnd = framebuffer + count * 3;
    const int32_t rowSkip = widgetY * bitmapRect.width;
    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    if (bitmapFormat == Bitmap::RGB888)
    {
        const uint8_t* const bitmapLineStart = reinterpret_cast<const uint8_t*>(bitmapData) + rowSkip * 3;
        const uint8_t* bitmapPointer = bitmapLineStart + widgetX * 3;
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            count -= length;
            paint::rgb888::lineFromRGB888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += length * 3;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
    else // if (bitmapFormat == Bitmap::ARGB8888)
    {
        const uint32_t* const bitmapLineStart = reinterpret_cast<const uint32_t*>(bitmapData) + rowSkip;
        const uint32_t* bitmapPointer = bitmapLineStart + widgetX;
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            count -= length;
            paint::rgb888::lineFromARGB8888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += 3 * length;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
}

void PainterRGB888Bitmap::tearDown() const
{
    paint::tearDown();
}
} // namespace touchgfx
