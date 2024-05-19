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

#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/PixelDataWidget.hpp>

namespace touchgfx
{
void PixelDataWidget::draw(const Rect& invalidatedArea) const
{
    if (!buffer)
    {
        return;
    }

    // Convert to lcd coordinates.
    const Rect absolute = getAbsoluteRect();
    // Copy to LCD
    HAL::lcd().blitCopy(buffer, format, absolute, invalidatedArea, alpha, false);
}

Rect PixelDataWidget::getSolidRect() const
{
    if (buffer && alpha == 255)
    {
        // There are at least some solid pixels
        switch (format)
        {
        case Bitmap::BW:
        case Bitmap::BW_RLE:
        case Bitmap::GRAY2:
        case Bitmap::GRAY4:
        case Bitmap::RGB565:
        case Bitmap::RGB888:
            // All solid pixels
            return Rect(0, 0, getWidth(), getHeight());
        case Bitmap::ARGB8888:
        case Bitmap::ARGB2222:
        case Bitmap::ABGR2222:
        case Bitmap::RGBA2222:
        case Bitmap::BGRA2222:
        case Bitmap::L8:
        case Bitmap::A4:
        case Bitmap::CUSTOM:
            // No knowledge about solid pixels
            break;
        }
    }
    // Empty rectangle
    return Rect();
}
} // namespace touchgfx
