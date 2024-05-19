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
 * @file touchgfx/widgets/canvas/PainterRGB565.hpp
 *
 * Declares the touchgfx::PainterRGB565 class.
 */
#ifndef TOUCHGFX_PAINTERRGB565_HPP
#define TOUCHGFX_PAINTERRGB565_HPP

#include <platform/driver/lcd/LCD16bpp.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterColor.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterRGB565.hpp>

namespace touchgfx
{
/**
 * The PainterRGB565 class allows a shape to be filled with a given color and alpha
 * value. This allows transparent, anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterRGB565 : public AbstractPainterRGB565, public AbstractPainterColor
{
public:
    /**
     * Initializes a new instance of the PainterRGB565 class.
     *
     * @param  color (Optional) the color, default is black.
     */
    PainterRGB565(colortype color = 0)
        : AbstractPainterRGB565(), AbstractPainterColor(color)
    {
    }

    virtual void setColor(colortype color)
    {
        AbstractPainterColor::setColor(color);
        color565 = LCD16bpp::getNativeColor(painterColor);
    }

    virtual void paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const;

    virtual void tearDown() const;

    virtual HAL::RenderingMethod getRenderingMethod() const
    {
        return HAL::getInstance()->getDMAType() == DMA_TYPE_CHROMART ? HAL::HARDWARE : HAL::SOFTWARE;
    }

protected:
    uint16_t color565; ///< The native color in 565 format (for speed reasons)
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERRGB565_HPP
