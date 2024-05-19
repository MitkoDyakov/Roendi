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
#include <touchgfx/widgets/canvas/PainterXRGB8888.hpp>

namespace touchgfx
{
void PainterXRGB8888::paint(uint8_t* destination, int16_t offset, int16_t /*widgetX*/, int16_t /*widgetY*/, int16_t count, uint8_t alpha) const
{
    uint32_t* RESTRICT framebuffer = reinterpret_cast<uint32_t*>(destination) + offset;
    const uint32_t* const lineEnd = framebuffer + count;

    if (alpha == 0xFF)
    {
        do
        {
            *framebuffer = painterColor;
        } while (++framebuffer < lineEnd);
    }
    else
    {
        do
        {
            const uint8_t ialpha = 0xFF - alpha;
            const uint32_t rgbBg = *framebuffer;
            *framebuffer = (LCD::div255(painterBlue * alpha + (rgbBg & 0xFF) * ialpha)) |
                           (LCD::div255(painterGreen * alpha + ((rgbBg >> 8) & 0xFF) * ialpha) << 8) |
                           (LCD::div255(painterRed * alpha + ((rgbBg >> 16) & 0xFF) * ialpha) << 16);
        } while (++framebuffer < lineEnd);
    }
}
} // namespace touchgfx
