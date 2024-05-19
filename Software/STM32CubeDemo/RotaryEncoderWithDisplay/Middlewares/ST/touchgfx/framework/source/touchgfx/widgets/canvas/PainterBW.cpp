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

#include <touchgfx/widgets/canvas/PainterBW.hpp>

namespace touchgfx
{
void PainterBW::paint(uint8_t* destination, int16_t offset, int16_t /*widgetX*/, int16_t /*widgetY*/, int16_t count, uint8_t alpha) const
{
    uint8_t* RESTRICT framebuffer = destination + (offset / 8);

    if (alpha >= 0x80)
    {
        do
        {
            const unsigned pixel = 1 << (7 - (offset % 8));
            if (painterBW)
            {
                *framebuffer |= pixel;
            }
            else
            {
                *framebuffer &= ~pixel;
            }
            if (((++offset) % 8) == 0)
            {
                framebuffer++;
            }
        } while (--count);
    }
}
} // namespace touchgfx
