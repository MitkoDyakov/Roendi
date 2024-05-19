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
 * @file touchgfx/widgets/canvas/PainterBW.hpp
 *
 * Declares the touchgfx::PainterBW class.
 */
#ifndef TOUCHGFX_PAINTERBW_HPP
#define TOUCHGFX_PAINTERBW_HPP

#include <platform/driver/lcd/LCD1bpp.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterBW.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterColor.hpp>

namespace touchgfx
{
/**
 * PainterBW is used for drawing one 1bpp displays. The color is either on or off. No
 * transparency is supported.
 *
 * @see AbstractPainter
 */
class PainterBW : public AbstractPainterBW, public AbstractPainterColor
{
public:
    /**
     * Constructor.
     *
     * @param  color (Optional) The color.
     */
    PainterBW(colortype color = 0)
        : AbstractPainterBW(), AbstractPainterColor(color)
    {
    }

    virtual void setColor(colortype color)
    {
        AbstractPainterColor::setColor(color);
        painterBW = LCD1bpp::getNativeColor(color);
    }

    virtual void paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const;

protected:
    uint8_t painterBW; ///< The color to use when painting
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERBW_HPP
