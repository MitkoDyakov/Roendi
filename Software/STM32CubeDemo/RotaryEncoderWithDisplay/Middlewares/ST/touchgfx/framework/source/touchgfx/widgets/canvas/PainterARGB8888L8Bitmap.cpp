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
#include <touchgfx/widgets/canvas/PainterARGB8888L8Bitmap.hpp>

namespace touchgfx
{
void PainterARGB8888L8Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::L8) && "The chosen painter only works with L8 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterARGB8888L8Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
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
    switch ((Bitmap::ClutFormat)((const uint16_t*)bitmapExtraPointer)[-2])
    {
    case Bitmap::CLUT_FORMAT_L8_RGB565:
        if (widgetAlpha == 0xFF)
        {
            do
            {
                const uint16_t srcpix = ((const uint16_t*)bitmapExtraPointer)[*bitmapPointer++];
                const uint8_t alpha = *covers++;
                const uint8_t red = Color::getRedFromRGB565(srcpix);
                const uint8_t green = Color::getGreenFromRGB565(srcpix);
                const uint8_t blue = Color::getBlueFromRGB565(srcpix);
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = (uint8_t)blue;
                    *p++ = (uint8_t)green;
                    *p++ = (uint8_t)red;
                    *p++ = 0xff;
                }
                else
                {
                    // Non-Transparent pixel
                    const uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255(blue * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(green * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(red * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
            } while (p < p_lineend);
        }
        else
        {
            do
            {
                const uint16_t srcpix = ((const uint16_t*)bitmapExtraPointer)[*bitmapPointer++];
                const uint8_t alpha = LCD::div255((*covers++) * widgetAlpha);
                const uint8_t red = Color::getRedFromRGB565(srcpix);
                const uint8_t green = Color::getGreenFromRGB565(srcpix);
                const uint8_t blue = Color::getBlueFromRGB565(srcpix);
                if (alpha)
                {
                    const uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255(blue * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(green * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(red * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
                else
                {
                    p += 4;
                }
            } while (p < p_lineend);
        }
        break;
    case Bitmap::CLUT_FORMAT_L8_RGB888:
        if (widgetAlpha == 0xFF)
        {
            do
            {
                const uint8_t* src = bitmapExtraPointer + *bitmapPointer++ * 3;
                const uint8_t alpha = *covers++;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = *src++; // Blue
                    *p++ = *src++; // Green
                    *p++ = *src;   // Red
                    *p++ = 0xff;   // Alpha
                }
                else
                {
                    // Non-Transparent pixel
                    const uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
            } while (p < p_lineend);
        }
        else
        {
            do
            {
                const uint8_t alpha = LCD::div255((*covers++) * widgetAlpha);
                if (alpha)
                {
                    const uint8_t* src = bitmapExtraPointer + *bitmapPointer++ * 3;
                    const uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
                else
                {
                    p += 4;
                }
            } while (p < p_lineend);
        }
        break;
    case Bitmap::CLUT_FORMAT_L8_ARGB8888:
        if (widgetAlpha == 0xFF)
        {
            do
            {
                uint32_t src = ((const uint32_t*)bitmapExtraPointer)[*bitmapPointer++];
                const uint8_t srcAlpha = src >> 24;
                const uint8_t alpha = LCD::div255((*covers++) * srcAlpha);
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = src;       // Blue
                    *p++ = src >> 8;  // Green
                    *p++ = src >> 16; // Red
                    *p++ = 0xff;      // Alpha
                }
                else
                {
                    // Non-Transparent pixel
                    const uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255((src & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(((src >> 8) & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(((src >> 16) & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
            } while (p < p_lineend);
        }
        else
        {
            do
            {
                uint32_t src = ((const uint32_t*)bitmapExtraPointer)[*bitmapPointer++];
                const uint8_t srcAlpha = src >> 24;
                const uint8_t alpha = LCD::div255((*covers++) * LCD::div255(srcAlpha * widgetAlpha));
                if (alpha)
                {
                    const uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255((src & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(((src >> 8) & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(((src >> 16) & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
                else
                {
                    p += 4;
                }
            } while (p < p_lineend);
        }
        break;
    }
}

bool PainterARGB8888L8Bitmap::renderInit()
{
    bitmapPointer = 0;
    bitmapExtraPointer = 0;

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

    if (bitmap.getFormat() == Bitmap::L8)
    {
        bitmapPointer = bitmap.getData();
        if (!bitmapPointer)
        {
            return false;
        }
        bitmapPointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        bitmapExtraPointer = bitmap.getExtraData();
        assert((bitmapExtraPointer != 0 && (*(const uint16_t*)bitmapExtraPointer == Bitmap::CLUT_FORMAT_L8_RGB565 || *(const uint16_t*)bitmapExtraPointer == Bitmap::CLUT_FORMAT_L8_RGB888 || *(const uint16_t*)bitmapExtraPointer == Bitmap::CLUT_FORMAT_L8_ARGB8888)));
        bitmapExtraPointer += 4; // Skip header
        return true;
    }

    return false;
}

bool PainterARGB8888L8Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }

    switch ((Bitmap::ClutFormat)((const uint16_t*)bitmapExtraPointer)[-2])
    {
    case Bitmap::CLUT_FORMAT_L8_RGB565:
        {
            uint16_t srcpix = ((const uint16_t*)bitmapExtraPointer)[*bitmapPointer++];
            red = (srcpix & 0xF800) >> 11;
            green = (srcpix & 0x07E0) >> 5;
            blue = srcpix & 0x001F;
            red = (red * 527 + 23) >> 6;
            green = (green * 259 + 33) >> 6;
            blue = (blue * 527 + 23) >> 6;
            alpha = 0xFF;
        }
        break;
    case Bitmap::CLUT_FORMAT_L8_RGB888:
        {
            const uint8_t* clut = bitmapExtraPointer + *bitmapPointer++ * 3;
            blue = *clut++;
            green = *clut++;
            red = *clut;
            alpha = 0xFF;
        }
        break;
    case Bitmap::CLUT_FORMAT_L8_ARGB8888:
        {
            uint32_t argb8888 = ((const uint32_t*)bitmapExtraPointer)[*bitmapPointer++];
            alpha = (argb8888 >> 24) & 0xFF;
            red = (argb8888 >> 16) & 0xFF;
            green = (argb8888 >> 8) & 0xFF;
            blue = argb8888 & 0xFF;
        }
        break;
    }

    return true;
}
} // namespace touchgfx
