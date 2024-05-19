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
 * @file touchgfx/widgets/canvas/PainterGRAY2.hpp
 *
 * Declares the touchgfx::PainterGRAY2 class.
 */
#ifndef TOUCHGFX_PAINTERGRAY2_HPP
#define TOUCHGFX_PAINTERGRAY2_HPP

#include <platform/driver/lcd/LCD2bpp.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterColor.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterGRAY2.hpp>

namespace touchgfx
{
/**
 * The PainterGRAY2 class allows a shape to be filled with a given color
 * value. This allows anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterGRAY2 : public AbstractPainterGRAY2, public AbstractPainterColor
{
public:
    /**
     * Initializes a new instance of the PainterGRAY2 class.
     *
     * @param  color (Optional) the color, default is black.
     */

    PainterGRAY2(colortype color = 0)
        : AbstractPainterGRAY2(), AbstractPainterColor(color)
    {
    }

    virtual void setColor(colortype color)
    {
        AbstractPainterColor::setColor(color);
        painterGray = LCD2bpp::getNativeColor(color);
    }

    virtual void paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const;

protected:
    uint8_t painterGray; ///< The gray color
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERGRAY2_HPP
