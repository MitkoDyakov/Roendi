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
#include <touchgfx/widgets/canvas/PainterXRGB8888Bitmap.hpp>
#include <platform/driver/lcd/LCD32bpp_XRGB8888.hpp>

namespace touchgfx
{
void PainterXRGB8888Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::RGB565 || bitmap.getFormat() == Bitmap::RGB888 || bitmap.getFormat() == Bitmap::ARGB8888) && "The chosen painter only works with RGB565, RGB888 and ARGB8888 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterXRGB8888Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
{
    uint32_t* RESTRICT p32 = reinterpret_cast<uint32_t*>(ptr) + (x + xAdjust);

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

    const uint32_t* const p32_lineend = p32 + count;
    if (bitmap.getFormat() == Bitmap::ARGB8888)
    {
        const uint32_t* RESTRICT src = bitmapARGB8888Pointer;
        if (widgetAlpha == 0xFF)
        {
            do
            {
                const uint8_t srcAlpha = (*src) >> 24;
                const uint8_t alpha = LCD::div255((*covers++) * srcAlpha);
                if (alpha == 0xFF)
                {
                    *p32 = *src;
                }
                else
                {
                    *p32 = LCD32bpp_XRGB8888::blendRgb888withXrgb8888(reinterpret_cast<const uint8_t*>(src), *p32, alpha, 0xFF - alpha);
                }
                p32++;
                src++;
            } while (p32 < p32_lineend);
        }
        else
        {
            do
            {
                const uint8_t srcAlpha = (*src) >> 24;
                const uint8_t alpha = LCD::div255((*covers++) * LCD::div255(srcAlpha * widgetAlpha));
                if (alpha)
                {
                    *p32 = LCD32bpp_XRGB8888::blendRgb888withXrgb8888(reinterpret_cast<const uint8_t*>(src), *p32, alpha, 0xFF - alpha);
                }
                p32++;
                src++;
            } while (p32 < p32_lineend);
        }
    }
    else if (bitmap.getFormat() == Bitmap::RGB888)
    {
        const uint8_t* RESTRICT src = bitmapRGB888Pointer;
        if (widgetAlpha == 0xFF)
        {
            do
            {
                const uint8_t alpha = *covers++;
                if (alpha == 0xFF)
                {
                    // Opaque pixel
                    *p32 = LCD32bpp_XRGB8888::rgb888toXrgb8888(src);
                }
                else
                {
                    // Non-Opaque pixel
                    *p32 = LCD32bpp_XRGB8888::blendRgb888withXrgb8888(src, *p32, alpha, 0xFF - alpha);
                }
                p32++;
                src += 3;
            } while (p32 < p32_lineend);
        }
        else
        {
            do
            {
                const uint8_t alpha = LCD::div255((*covers++) * widgetAlpha);
                if (alpha)
                {
                    *p32 = LCD32bpp_XRGB8888::blendRgb888withXrgb8888(src, *p32, alpha, 0xFF - alpha);
                }
                p32++;
                src += 3;
            } while (p32 < p32_lineend);
        }
    }
    else if (bitmap.getFormat() == Bitmap::RGB565)
    {
        const uint16_t* RESTRICT src = bitmapRGB565Pointer;
        if (widgetAlpha == 0xFF)
        {
            do
            {
                const uint8_t alpha = *covers++;
                const uint16_t srcpix = *src++;
                if (alpha == 0xFF)
                {
                    *p32 = Color::rgb565toXrgb8888(srcpix);
                }
                else
                {
                    *p32 = LCD32bpp_XRGB8888::blendRgb565withXrgb8888(srcpix, *p32, alpha, 0xFF - alpha);
                }
                p32++;
            } while (p32 < p32_lineend);
        }
        else
        {
            do
            {
                const uint8_t alpha = LCD::div255((*covers++) * widgetAlpha);
                uint16_t srcpix = *src++;
                if (alpha)
                {
                    *p32 = LCD32bpp_XRGB8888::blendRgb565withXrgb8888(srcpix, *p32, alpha, 0xFF - alpha);
                }
                p32++;
            } while (p32 < p32_lineend);
        }
    }
}

bool PainterXRGB8888Bitmap::renderInit()
{
    bitmapARGB8888Pointer = 0;
    bitmapRGB565Pointer = 0;
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
        return true;
    }

    return false;
}

bool PainterXRGB8888Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
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
        alpha = 0xFF;
    }
    else if (bitmapRGB565Pointer != 0)
    {
        uint16_t srcpix = *bitmapRGB565Pointer++;
        red = (srcpix & 0xF800) >> 11;
        green = (srcpix & 0x07E0) >> 5;
        blue = srcpix & 0x001F;
        red = (red * 527 + 23) >> 6;
        green = (green * 259 + 33) >> 6;
        blue = (blue * 527 + 23) >> 6;
        alpha = 0xFF;
    }
    return true;
}
} // namespace touchgfx
