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

#include <touchgfx/widgets/canvas/PainterBGRA2222.hpp>

namespace touchgfx
{
void PainterBGRA2222::paint(uint8_t* destination, int16_t offset, int16_t /*widgetX*/, int16_t /*widgetY*/, int16_t count, uint8_t alpha) const
{
    uint8_t* RESTRICT framebuffer = destination + offset;
    const uint8_t* const lineEnd = framebuffer + count;

    if (alpha == 0xFF)
    {
        do
        {
            *framebuffer = color8;
        } while (++framebuffer < lineEnd);
    }
    else
    {
        do
        {
            *framebuffer = alphaBlend(painterRed, painterGreen, painterBlue, *framebuffer, alpha);
        } while (++framebuffer < lineEnd);
    }
}
} // namespace touchgfx
