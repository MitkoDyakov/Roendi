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

#include <platform/driver/lcd/LCD2bpp.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterGRAY2Bitmap.hpp>

namespace touchgfx
{
void PainterGRAY2Bitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::GRAY2) && "PainterGRAY2Bitmap only works with GRAY2 bitmaps");
    assert(bitmap.getId() == BITMAP_INVALID || bitmapData);
    bitmapExtraData = bitmap.getExtraData();
}

void PainterGRAY2Bitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    const int32_t rowSkip = widgetY * ((bitmapRect.width + 3) / 4);
    const uint8_t* bitmapPointer = bitmapData + rowSkip;
    if (bitmapExtraData)
    {
        const uint8_t* alphaPointer = bitmapExtraData + rowSkip;
        do
        {
            const uint8_t bitmapGray = LCD2bpp::getPixel(bitmapPointer, widgetX);
            const uint8_t a = LCD::div255(alpha * (LCD2bpp::getPixel(alphaPointer, widgetX) * 0x55));

            if (a == 0xFF)
            {
                LCD2bpp::setPixel(destination, offset, bitmapGray);
            }
            else
            {
                const uint8_t framebufferGray = LCD2bpp::getPixel(destination, offset);
                LCD2bpp::setPixel(destination, offset, LCD::div255((bitmapGray * a + framebufferGray * (0xFF - a)) * 0x55) >> 6);
            }
            widgetX++;
            widgetX %= bitmapRect.width;
            offset++;
        } while (--count != 0);
    }
    else if (alpha == 0xFF)
    {
        do
        {
            const uint8_t bitmapGray = LCD2bpp::getPixel(bitmapPointer, widgetX);
            LCD2bpp::setPixel(destination, offset, bitmapGray);
            widgetX++;
            widgetX %= bitmapRect.width;
            offset++;
        } while (--count != 0);
    }
    else
    {
        do
        {
            const uint8_t bitmapGray = LCD2bpp::getPixel(bitmapPointer, widgetX);
            const uint8_t framebufferGray = LCD2bpp::getPixel(destination, offset);
            LCD2bpp::setPixel(destination, offset, LCD::div255((bitmapGray * alpha + framebufferGray * (0xFF - alpha)) * 0x55) >> 6);
            widgetX++;
            widgetX %= bitmapRect.width;
            offset++;
        } while (--count != 0);
    }
}
} // namespace touchgfx
