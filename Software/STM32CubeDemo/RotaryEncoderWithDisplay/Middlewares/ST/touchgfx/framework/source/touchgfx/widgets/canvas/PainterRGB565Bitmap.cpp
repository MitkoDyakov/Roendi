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
#include <touchgfx/widgets/canvas/PainterRGB565Bitmap.hpp>

namespace touchgfx
{
void PainterRGB565Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmapFormat == Bitmap::RGB565 || bitmapFormat == Bitmap::ARGB8888) && "PainterRGB565Bitmap only works with RGB565 and ARGB8888 bitmaps");
    assert(bitmap.getId() == BITMAP_INVALID || bitmapData);
    bitmapExtraData = bitmap.getExtraData();
}

void PainterRGB565Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    uint16_t* RESTRICT framebuffer = reinterpret_cast<uint16_t*>(destination) + offset;
    const uint16_t* const lineEnd = framebuffer + count;
    const int32_t rowSkip = widgetY * bitmapRect.width;
    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    if (bitmapFormat == Bitmap::RGB565)
    {
        const uint16_t* const bitmapLineStart = reinterpret_cast<const uint16_t*>(bitmapData) + rowSkip;
        const uint16_t* bitmapPointer = bitmapLineStart + widgetX;
        if (bitmapExtraData)
        {
            const uint8_t* const alpha_linestart = bitmapExtraData + rowSkip;
            const uint8_t* alphaPointer = alpha_linestart + widgetX;
            do
            {
                const int16_t length = MIN(bitmapAvailable, count);
                const uint16_t* const chunkend = framebuffer + length;
                count -= length;
                do
                {
                    const uint8_t a = LCD::div255(alpha * (*alphaPointer++));
                    if (a == 0xFF)
                    {
                        *framebuffer = *bitmapPointer;
                    }
                    else if (a)
                    {
                        *framebuffer = alphaBlend(*bitmapPointer, *framebuffer, a);
                    }
                    bitmapPointer++;
                } while (++framebuffer < chunkend);
                bitmapPointer = bitmapLineStart;
                alphaPointer = alpha_linestart;
                bitmapAvailable = bitmapRect.width;
            } while (framebuffer < lineEnd);
        }
        else
        {
            do
            {
                const int16_t length = MIN(bitmapAvailable, count);
                count -= length;

                // Call external (DMA) painter function
                paint::rgb565::lineFromRGB565(framebuffer, bitmapPointer, length, alpha);

                framebuffer += length;
                bitmapPointer = bitmapLineStart;
                bitmapAvailable = bitmapRect.width;
            } while (framebuffer < lineEnd);
        }
    }
    else // if (bitmapFormat == Bitmap::ARGB8888)
    {
        const uint32_t* const bitmapLineStart = reinterpret_cast<const uint32_t*>(bitmapData) + rowSkip;
        const uint32_t* bitmapPointer = bitmapLineStart + widgetX;
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);
            count -= length;

            //Call external (DMA) painter function
            paint::rgb565::lineFromARGB8888(framebuffer, bitmapPointer, length, alpha);

            framebuffer += length;
            bitmapPointer = bitmapLineStart;
            bitmapAvailable = bitmapRect.width;
        } while (framebuffer < lineEnd);
    }
}

void PainterRGB565Bitmap::tearDown() const
{
    paint::tearDown();
}

} // namespace touchgfx
