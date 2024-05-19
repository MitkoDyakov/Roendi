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

#include <touchgfx/hal/Paint.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888.hpp>

namespace touchgfx
{
void PainterRGB888::paint(uint8_t* destination, int16_t offset, int16_t /*widgetX*/, int16_t /*widgetY*/, int16_t count, uint8_t alpha) const
{
    uint8_t* RESTRICT framebuffer = destination + offset * 3;
    paint::rgb888::lineFromColor(framebuffer, count, painterColor, alpha);
}

void PainterRGB888::tearDown() const
{
    paint::tearDown();
}
} // namespace touchgfx
