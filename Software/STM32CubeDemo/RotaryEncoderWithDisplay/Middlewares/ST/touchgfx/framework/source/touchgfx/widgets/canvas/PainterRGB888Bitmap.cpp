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
#include <touchgfx/widgets/canvas/PainterRGB888Bitmap.hpp>

namespace touchgfx
{
void PainterRGB888Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::RGB888 || bitmap.getFormat() == Bitmap::ARGB8888) && "The chosen painter only works with RGB888 and ARGB8888 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterRGB888Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
{
    uint8_t* p = ptr + (x + xAdjust) * 3;

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

    const uint8_t* const p_lineend = p + 3 * count;
    if (bitmapRGB888Pointer)
    {
        const uint8_t* src = bitmapRGB888Pointer;
        uint8_t pByte;
        if (widgetAlpha == 0xFF)
        {
            do
            {
                // Use alpha from covers directly
                const uint8_t alpha = *covers++;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = *src++;
                    *p++ = *src++;
                    *p++ = *src++;
                }
                else
                {
                    const uint8_t ialpha = 0xFF - alpha;
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                }
            } while (p < p_lineend);
        }
        else
        {
            do
            {
                const uint8_t alpha = LCD::div255((*covers++) * widgetAlpha);
                const uint8_t ialpha = 0xFF - alpha;
                pByte = *p;
                *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                pByte = *p;
                *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                pByte = *p;
                *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
            } while (p < p_lineend);
        }
    }
    else if (bitmapARGB8888Pointer)
    {
        const uint32_t* src = bitmapARGB8888Pointer;
        if (widgetAlpha == 0xFF)
        {
            do
            {
                const uint8_t srcAlpha = (*src) >> 24;
                const uint8_t alpha = LCD::div255((*covers++) * srcAlpha);
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = (*src);       // Blue
                    *p++ = (*src) >> 8;  // Green
                    *p++ = (*src) >> 16; // Red
                }
                else
                {
                    // Non-Transparent pixel
                    const uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    uint8_t cByte = (*src);
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = (*src) >> 8;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = (*src) >> 16;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                }
                src++;
            } while (p < p_lineend);
        }
        else
        {
            do
            {
                const uint8_t srcAlpha = (*src) >> 24;
                const uint8_t alpha = LCD::div255((*covers++) * LCD::div255(srcAlpha * widgetAlpha));
                if (alpha)
                {
                    const uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    uint8_t cByte = (*src);
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = (*src) >> 8;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = (*src) >> 16;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                }
                else
                {
                    p += 3;
                }
                src++;
            } while (p < p_lineend);
        }
    }
}

bool PainterRGB888Bitmap::renderInit()
{
    bitmapARGB8888Pointer = 0;
    bitmapRGB888Pointer = 0;

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

    if (bitmap.getFormat() == Bitmap::ARGB8888)
    {
        bitmapARGB8888Pointer = (const uint32_t*)bitmap.getData();
        if (!bitmapARGB8888Pointer)
        {
            return false;
        }
        bitmapARGB8888Pointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        return true;
    }

    if (bitmap.getFormat() == Bitmap::RGB888)
    {
        bitmapRGB888Pointer = bitmap.getData();
        if (!bitmapRGB888Pointer)
        {
            return false;
        }
        bitmapRGB888Pointer += (currentX + currentY * bitmapRectToFrameBuffer.width) * 3;
        return true;
    }

    return false;
}

bool PainterRGB888Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }

    if (bitmapARGB8888Pointer != 0)
    {
        uint32_t argb8888 = *bitmapARGB8888Pointer++;
        alpha = (argb8888 >> 24) & 0xFF;
        red = (argb8888 >> 16) & 0xFF;
        green = (argb8888 >> 8) & 0xFF;
        blue = argb8888 & 0xFF;
    }
    else if (bitmapRGB888Pointer != 0)
    {
        blue = *bitmapRGB888Pointer++;
        green = *bitmapRGB888Pointer++;
        red = *bitmapRGB888Pointer++;
        alpha = 0xFF; // No alpha per pixel in the image, it is solid
    }
    return true;
}
} // namespace touchgfx
