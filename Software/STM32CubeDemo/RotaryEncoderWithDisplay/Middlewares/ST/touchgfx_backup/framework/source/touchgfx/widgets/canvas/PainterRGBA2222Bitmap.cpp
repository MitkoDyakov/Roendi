/******************************************************************************
* Copyright (c) 2018(-2021) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.17.0 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterRGBA2222Bitmap.hpp>
#include <platform/driver/lcd/LCD8bpp_RGBA2222.hpp>

namespace touchgfx
{
void PainterRGBA2222Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::RGBA2222) && "The chosen painter only works with RGBA2222 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterRGBA2222Bitmap::render(uint8_t* ptr,
                                   int x,
                                   int xAdjust,
                                   int y,
                                   unsigned count,
                                   const uint8_t* covers)
{
    uint8_t* p = ptr + (x + xAdjust);

    currentX = x + areaOffsetX;
    currentY = y + areaOffsetY;

    if (!renderInit())
    {
        return;
    }

    if (currentX + (int)count > bitmapRectToFrameBuffer.width)
    {
        count = bitmapRectToFrameBuffer.width - currentX;
    }

    const uint8_t* const p_lineend = p + count;
    const uint8_t* src = bitmapRGBA2222Pointer;
    if (widgetAlpha == 0xFF)
    {
        do
        {
            const uint8_t srcAlpha = ((*src) & 0x03) * 0x55;
            const uint8_t alpha = LCD::div255((*covers++) * srcAlpha);
            if (alpha == 0xFF)
            {
                // Solid pixel
                *p = *src;
            }
            else if (alpha)
            {
                // Non-Transparent pixel
                *p = mixColors(*src, *p, alpha);
            }
            p++;
            src++;
        } while (p < p_lineend);
    }
    else
    {
        do
        {
            const uint8_t srcAlpha = ((*src) & 0x03) * 0x55;
            const uint8_t alpha = LCD::div255((*covers++) * srcAlpha);
            if (alpha) // This can never get to max=0xFF*0xFF as totalAlpha<255
            {
                // Non-Transparent pixel
                *p = mixColors(*src, *p, alpha);
            }
            p++;
            src++;
        } while (p < p_lineend);
    }
}

bool PainterRGBA2222Bitmap::renderInit()
{
    bitmapRGBA2222Pointer = 0;

    if (bitmap.getId() == BITMAP_INVALID)
    {
        return false;
    }

    if ((currentX >= bitmapRectToFrameBuffer.width) || (currentY >= bitmapRectToFrameBuffer.height))
    {
        // Outside bitmap area, do not draw anything
        // Consider the following instead of "return" to get a tiled image:
        //   currentX %= bitmapRectToFrameBuffer.width
        //   currentY %= bitmapRectToFrameBuffer.height
        return false;
    }

    if (bitmap.getFormat() == Bitmap::RGBA2222)
    {
        bitmapRGBA2222Pointer = bitmap.getData();
        if (!bitmapRGBA2222Pointer)
        {
            return false;
        }
        bitmapRGBA2222Pointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        return true;
    }

    return false;
}

bool PainterRGBA2222Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }
    else if (bitmapRGBA2222Pointer != 0)
    {
        uint8_t RGBA2222 = *bitmapRGBA2222Pointer++;
        red = LCD8bpp_RGBA2222::getRedFromNativeColor(RGBA2222);
        green = LCD8bpp_RGBA2222::getGreenFromNativeColor(RGBA2222);
        blue = LCD8bpp_RGBA2222::getBlueFromNativeColor(RGBA2222);
        alpha = LCD8bpp_RGBA2222::getAlphaFromNativeColor(RGBA2222);
    }
    return true;
}
} // namespace touchgfx
