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
#include <touchgfx/Color.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterARGB8888Bitmap.hpp>

namespace touchgfx
{
void PainterARGB8888Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::RGB565 || bitmap.getFormat() == Bitmap::RGB888 || bitmap.getFormat() == Bitmap::ARGB8888) && "The chosen painter only works with RGB565, RGB888 and ARGB8888 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterARGB8888Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
{
    uint8_t* RESTRICT p = ptr + (x + xAdjust) * 4;

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

    const uint8_t* const p_lineend = p + 4 * count;
    if (bitmap.getFormat() == Bitmap::ARGB8888)
    {
        const uint32_t* RESTRICT src = bitmapARGB8888Pointer;
        do
        {
            const uint8_t srcAlpha = (*src) >> 24;
            const uint8_t alphaFg = LCD::div255((*covers++) * LCD::div255(srcAlpha * widgetAlpha));
            const uint8_t alphaBg = p[3];
            if (0 /*alphaFg == 255 || alphaBg == 0*/)
            {
                const uint8_t blueFg = *src;
                *p++ = blueFg;
                const uint8_t greenFg = (*src) >> 8;
                *p++ = greenFg;
                const uint8_t redFg = (*src) >> 16;
                *p++ = redFg;
                *p++ = alphaFg;
            }
            else if (alphaFg)
            {
                const uint8_t alphaMult = LCD::div255(alphaFg * alphaBg);
                const uint8_t alphaOut = alphaFg + alphaBg - alphaMult;

                const uint8_t blueBg = *p;
                const uint8_t blueFg = *src;
                *p++ = (blueFg * alphaFg + blueBg * (alphaBg - alphaMult)) / alphaOut;
                const uint8_t greenBg = *p;
                const uint8_t greenFg = (*src) >> 8;
                *p++ = (greenFg * alphaFg + greenBg * (alphaBg - alphaMult)) / alphaOut;
                const uint8_t redBg = *p;
                const uint8_t redFg = (*src) >> 16;
                *p++ = (redFg * alphaFg + redBg * (alphaBg - alphaMult)) / alphaOut;
                *p++ = alphaOut;
            }
            else
            {
                p += 4;
            }
            src++;
        } while (p < p_lineend);
    }
    else if (bitmap.getFormat() == Bitmap::RGB888)
    {
        const uint8_t* RESTRICT src = bitmapRGB888Pointer;
        do
        {
            const uint8_t alphaFg = LCD::div255((*covers++) * widgetAlpha);
            if (alphaFg)
            {
                const uint8_t alphaBg = p[3];
                const uint8_t alphaMult = LCD::div255(alphaFg * alphaBg);
                const uint8_t alphaOut = alphaFg + alphaBg - alphaMult;

                const uint8_t blueBg = *p;
                const uint8_t blueFg = *src++;
                *p++ = (blueFg * alphaFg + blueBg * alphaBg - blueBg * alphaMult) / alphaOut;
                const uint8_t greenBg = *p;
                const uint8_t greenFg = *src++;
                *p++ = (greenFg * alphaFg + greenBg * alphaBg - greenBg * alphaMult) / alphaOut;
                const uint8_t redBg = *p;
                const uint8_t redFg = *src++;
                *p++ = (redFg * alphaFg + redBg * alphaBg - redBg * alphaMult) / alphaOut;
                *p++ = alphaOut;
            }
            else
            {
                src += 3;
                p += 4;
            }
        } while (p < p_lineend);
    }
    else if (bitmap.getFormat() == Bitmap::RGB565)
    {
        const uint16_t* RESTRICT src = bitmapRGB565Pointer;
        if (bitmapRGB565AlphaPointer == 0)
        {
            do
            {
                const uint8_t alphaFg = LCD::div255((*covers++) * widgetAlpha);
                if (alphaFg)
                {
                    const uint8_t alphaBg = p[3];
                    const uint8_t alphaMult = LCD::div255(alphaFg * alphaBg);
                    const uint8_t alphaOut = alphaFg + alphaBg - alphaMult;

                    const uint8_t blueBg = *p;
                    const uint8_t blueFg = Color::getBlueFromRGB565(*src);
                    *p++ = (blueFg * alphaFg + blueBg * alphaBg - blueBg * alphaMult) / alphaOut;
                    const uint8_t greenBg = *p;
                    const uint8_t greenFg = Color::getGreenFromRGB565(*src);
                    *p++ = (greenFg * alphaFg + greenBg * alphaBg - greenBg * alphaMult) / alphaOut;
                    const uint8_t redBg = *p;
                    const uint8_t redFg = Color::getRedFromRGB565(*src);
                    *p++ = (redFg * alphaFg + redBg * alphaBg - redBg * alphaMult) / alphaOut;
                    *p++ = alphaOut;
                }
                else
                {
                    src++;
                    p += 4;
                }
                src++;
            } while (p < p_lineend);
        }
        else
        {
            const uint8_t* RESTRICT srcAlphaPtr = bitmapRGB565AlphaPointer;
            do
            {
                const uint8_t srcAlpha = *srcAlphaPtr++;
                const uint8_t alphaFg = LCD::div255((*covers++) * LCD::div255(srcAlpha * widgetAlpha));
                if (alphaFg)
                {
                    const uint8_t alphaBg = p[3];
                    const uint8_t alphaMult = LCD::div255(alphaFg * alphaBg);
                    const uint8_t alphaOut = alphaFg + alphaBg - alphaMult;

                    const uint8_t blueBg = *p;
                    const uint8_t blueFg = Color::getBlueFromRGB565(*src);
                    *p++ = (blueFg * alphaFg + blueBg * alphaBg - blueBg * alphaMult) / alphaOut;
                    const uint8_t greenBg = *p;
                    const uint8_t greenFg = Color::getGreenFromRGB565(*src);
                    *p++ = (greenFg * alphaFg + greenBg * alphaBg - greenBg * alphaMult) / alphaOut;
                    const uint8_t redBg = *p;
                    const uint8_t redFg = Color::getRedFromRGB565(*src);
                    *p++ = (redFg * alphaFg + redBg * alphaBg - redBg * alphaMult) / alphaOut;
                    *p++ = alphaOut;
                }
                else
                {
                    src++;
                    p += 4;
                }
                src++;
            } while (p < p_lineend);
        }
    }
}

bool PainterARGB8888Bitmap::renderInit()
{
    bitmapARGB8888Pointer = 0;
    bitmapRGB888Pointer = 0;
    bitmapRGB565Pointer = 0;
    bitmapRGB565AlphaPointer = 0;

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
        bitmapRGB888Pointer = (const uint8_t*)bitmap.getData();
        if (!bitmapRGB888Pointer)
        {
            return false;
        }
        bitmapRGB888Pointer += (currentX + currentY * bitmapRectToFrameBuffer.width) * 3;
        return true;
    }

    if (bitmap.getFormat() == Bitmap::RGB565)
    {
        bitmapRGB565Pointer = (const uint16_t*)bitmap.getData();
        if (!bitmapRGB565Pointer)
        {
            return false;
        }
        bitmapRGB565Pointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        bitmapRGB565AlphaPointer = bitmap.getExtraData();
        if (bitmapRGB565AlphaPointer)
        {
            bitmapRGB565AlphaPointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        }
        return true;
    }

    return false;
}

bool PainterARGB8888Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }

    if (bitmapARGB8888Pointer != 0)
    {
        uint32_t argb8888 = *bitmapARGB8888Pointer++;
        alpha = argb8888 >> 24;
        red = argb8888 >> 16;
        green = argb8888 >> 8;
        blue = argb8888;
    }
    else if (bitmapRGB888Pointer != 0)
    {
        blue = *bitmapRGB888Pointer++;
        green = *bitmapRGB888Pointer++;
        red = *bitmapRGB888Pointer++;
        alpha = 0xff;
    }
    else if (bitmapRGB565Pointer != 0)
    {
        uint16_t srcpix = *bitmapRGB565Pointer++;
        red = Color::getRedFromRGB565(srcpix);
        green = Color::getGreenFromRGB565(srcpix);
        blue = Color::getBlueFromRGB565(srcpix);
        if (bitmapRGB565AlphaPointer != 0)
        {
            alpha = *bitmapRGB565AlphaPointer++;
        }
        else
        {
            alpha = 0xff;
        }
    }

    return true;
}
} // namespace touchgfx
