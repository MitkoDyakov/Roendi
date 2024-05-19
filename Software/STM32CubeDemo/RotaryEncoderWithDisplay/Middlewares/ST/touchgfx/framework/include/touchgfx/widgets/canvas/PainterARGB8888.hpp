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
 * @file touchgfx/widgets/canvas/PainterARGB8888.hpp
 *
 * Declares the touchgfx::PainterARGB8888 class.
 */
#ifndef TOUCHGFX_PAINTERARGB8888_HPP
#define TOUCHGFX_PAINTERARGB8888_HPP

#include <touchgfx/Color.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterARGB8888.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterColor.hpp>

namespace touchgfx
{
/**
 * The PainterARGB8888 class allows a shape to be filled with a given color
 * value. This allows anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterARGB8888 : public AbstractPainterARGB8888, public AbstractPainterColor
{
public:
    /**
     * Initializes a new instance of the PainterARGB8888 class.
     *
     * @param  color (Optional) the color, default is black.
     */
    PainterARGB8888(colortype color = 0)
        : AbstractPainterARGB8888(), AbstractPainterColor(color)
    {
    }

    virtual void setColor(colortype color)
    {
        AbstractPainterColor::setColor(color);
        painterRed = Color::getRed(color);
        painterGreen = Color::getGreen(color);
        painterBlue = Color::getBlue(color);
    }

    virtual void paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const;

    virtual void tearDown() const;

protected:
    uint8_t painterRed;   ///< The red part of the color
    uint8_t painterGreen; ///< The green part of the color
    uint8_t painterBlue;  ///< The blue part of the color
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERARGB8888_HPP
