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
 * @file touchgfx/widgets/canvas/PainterGRAY4.hpp
 *
 * Declares the touchgfx::PainterGRAY4 class.
 */
#ifndef TOUCHGFX_PAINTERGRAY4_HPP
#define TOUCHGFX_PAINTERGRAY4_HPP

#include <platform/driver/lcd/LCD4bpp.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterColor.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterGRAY4.hpp>

namespace touchgfx
{
/**
 * The PainterGRAY4 class allows a shape to be filled with a given color and alpha
 * value. This allows transparent, anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterGRAY4 : public AbstractPainterGRAY4, public AbstractPainterColor
{
public:
    /**
     * Initializes a new instance of the PainterGRAY4 class.
     *
     * @param  color (Optional) the color, default is black.
     */
    PainterGRAY4(colortype color = 0)
        : AbstractPainterGRAY4(), AbstractPainterColor(color)
    {
    }

    virtual void setColor(colortype color)
    {
        AbstractPainterColor::setColor(color);
        painterGray = LCD4bpp::getNativeColor(color);
    }

    virtual void paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const;

protected:
    uint8_t painterGray; ///< The gray color
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERGRAY4_HPP
