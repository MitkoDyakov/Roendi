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
 * @file touchgfx/widgets/canvas/PainterRGB888.hpp
 *
 * Declares the touchgfx::PainterRGB888 class.
 */
#ifndef TOUCHGFX_PAINTERRGB888_HPP
#define TOUCHGFX_PAINTERRGB888_HPP

#include <touchgfx/Color.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterColor.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterRGB888.hpp>

namespace touchgfx
{
/**
 * The PainterRGB888 class allows a shape to be filled with a given color
 * value. This allows anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterRGB888 : public AbstractPainterRGB888, public AbstractPainterColor
{
public:
    /**
     * Initializes a new instance of the PainterRGB888 class.
     *
     * @param  color (Optional) the color, default is black.
     */
    PainterRGB888(colortype color = 0)
        : AbstractPainterRGB888(), AbstractPainterColor(color)
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

    virtual HAL::RenderingMethod getRenderingMethod() const
    {
        return HAL::getInstance()->getDMAType() == DMA_TYPE_CHROMART ? HAL::HARDWARE : HAL::SOFTWARE;
    }

protected:
    uint8_t painterRed;   ///< The red part of the color
    uint8_t painterGreen; ///< The green part of the color
    uint8_t painterBlue;  ///< The blue part of the color
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERRGB888_HPP
