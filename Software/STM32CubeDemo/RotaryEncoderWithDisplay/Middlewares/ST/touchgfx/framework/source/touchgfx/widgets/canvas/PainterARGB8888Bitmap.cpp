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
#include <touchgfx/widgets/canvas/PainterARGB8888Bitmap.hpp>

namespace touchgfx
{
void PainterARGB8888Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmapFormat == Bitmap::RGB565 || bitmapFormat == Bitmap::RGB888 || bitmapFormat == Bitmap::ARGB8888) && "PainterARGB8888Bitmap only works with RGB565, RGB888 and ARGB8888 bitmaps");
    assert(bitmap.getId() == BITMAP_INVALID || bitmapData);
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getExtraData() == 0) && "PainterARGB8888Bitmap does not work with RGB565 with alphas");
}

void PainterARGB8888Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    uint8_t* RESTRICT framebuffer = destination + offset * 4;
    const uint8_t* const lineEnd = framebuffer + count * 4;
    const int32_t rowSkip = widgetY * bitmapRect.width;
    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    if (bitmapFormat == Bitmap::ARGB8888)
    {
        const uint32_t* const bitmapLineStart = reinterpret_cast<const uint32_t*>(bitmapData) + rowSkip;
        const uint32_t* bitmapPointer = bitmapLineStart + widgetX;
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            count -= length;
            paint::argb8888::lineFromARGB8888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += length * 4;
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
            count -= length;
            paint::argb8888::lineFromRGB888(framebuffer, bitmapPointer, length, alpha);
            framebuffer += length * 4;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
    else
    {
        const uint16_t* const bitmapLineStart = reinterpret_cast<const uint16_t*>(bitmapData) + rowSkip;
        const uint16_t* bitmapPointer = bitmapLineStart + widgetX;
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            count -= length;

            paint::argb8888::lineFromRGB565(framebuffer, bitmapPointer, length, alpha);

            framebuffer += length * 4;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
}

void PainterARGB8888Bitmap::tearDown() const
{
    paint::tearDown();
}
} // namespace touchgfx
