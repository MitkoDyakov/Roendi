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

#include <platform/driver/lcd/LCD8bpp_ARGB2222.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterARGB2222Bitmap.hpp>

namespace touchgfx
{
void PainterARGB2222Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmapFormat == Bitmap::ARGB2222) && "PainterARGB2222Bitmap only works with ARGB2222 bitmaps");
    assert(bitmap.getId() == BITMAP_INVALID || bitmapData);
}

void PainterARGB2222Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    uint8_t* RESTRICT framebuffer = destination + offset;
    const uint8_t* const lineEnd = framebuffer + count;
    const int32_t rowSkip = widgetY * bitmapRect.width;
    const uint8_t* const bitmapLineStart = bitmapData + rowSkip;
    const uint8_t* bitmapPointer = bitmapLineStart + widgetX;
    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    do
    {
        const int16_t length = MIN(bitmapAvailable, count);
        const uint8_t* const chunkend = framebuffer + length;
        count -= length;
        do
        {
            const uint8_t argb = *bitmapPointer++;
            const uint8_t srcAlpha = (argb >> 6) * 0x55;
            const uint8_t a = LCD::div255(alpha * srcAlpha);
            if (a == 0xFF)
            {
                *framebuffer = argb;
            }
            else if (a)
            {
                *framebuffer = alphaBlend(argb, *framebuffer, a);
            }
        } while (++framebuffer < chunkend);
        bitmapPointer = bitmapLineStart;
        bitmapAvailable = bitmapRect.width;
    } while (framebuffer < lineEnd);
}
} // namespace touchgfx
