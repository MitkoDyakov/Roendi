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

/**
 * @file touchgfx/widgets/canvas/PainterBGRA2222.hpp
 *
 * Declares the touchgfx::PainterBGRA2222 class.
 */
#ifndef TOUCHGFX_PAINTERBGRA2222_HPP
#define TOUCHGFX_PAINTERBGRA2222_HPP

#include <platform/driver/lcd/LCD8bpp_BGRA2222.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterBGRA2222.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterColor.hpp>

namespace touchgfx
{
/**
 * The PainterBGRA2222 class allows a shape to be filled with a given color
 * value. This allows anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterBGRA2222 : public AbstractPainterBGRA2222, public AbstractPainterColor
{
public:
    /**
     * Initializes a new instance of the PainterBGRA2222 class.
     *
     * @param  color (Optional) the color, default is black.
     */
    PainterBGRA2222(colortype color = 0)
        : AbstractPainterBGRA2222(), AbstractPainterColor(color)
    {
    }

    virtual void setColor(colortype color)
    {
        AbstractPainterColor::setColor(color);
        painterRed = Color::getRed(color);
        painterGreen = Color::getGreen(color);
        painterBlue = Color::getBlue(color);
        color8 = LCD8bpp_BGRA2222::getNativeColor(painterColor);
    }

    virtual void paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const;

protected:
    uint8_t painterRed;   ///< The red part of the color, used when alpha blending
    uint8_t painterGreen; ///< The green part of the color, used when alpha blending
    uint8_t painterBlue;  ///< The blue part of the color, used when alpha blending
    uint8_t color8;       ///< Pre-calculated native color
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERBGRA2222_HPP
