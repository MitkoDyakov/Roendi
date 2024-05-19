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

#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/canvas/PainterGRAY2.hpp>

namespace touchgfx
{
void PainterGRAY2::paint(uint8_t* destination, int16_t offset, int16_t /*widgetX*/, int16_t /*widgetY*/, int16_t count, uint8_t alpha) const
{
    if (alpha == 0xFF)
    {
        do
        {
            LCD2bpp::setPixel(destination, offset, painterGray);
            offset++;
        } while (--count != 0);
    }
    else
    {
        do
        {
            const uint8_t framebufferGray = LCD2bpp::getPixel(destination, offset);
            LCD2bpp::setPixel(destination, offset, LCD::div255((painterGray * alpha + framebufferGray * (0xFF - alpha)) * 0x55) >> 6);
            offset++;
        } while (--count != 0);
    }
}
} // namespace touchgfx
