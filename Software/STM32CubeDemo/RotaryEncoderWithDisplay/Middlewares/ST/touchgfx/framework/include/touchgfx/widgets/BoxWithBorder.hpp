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
 * @file touchgfx/widgets/BoxWithBorder.hpp
 *
 * Declares the touchgfx::BoxWithBorder class.
 */
#ifndef TOUCHGFX_BOXWITHBORDER_HPP
#define TOUCHGFX_BOXWITHBORDER_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Box.hpp>

namespace touchgfx
{
/**
 * The BoxWithBorder class is used to create objects that can draw a box with a border on the
 * display. The width of the border can be specified. If the border width is 0 the
 * BoxWithBorder will function just like a Box.
 */
class BoxWithBorder : public Box
{
public:
    BoxWithBorder()
        : Box(), borderColor(0), borderSize(0)
    {
    }

    /**
     * Constructor that allows specification of dimensions and colors of the BoxWithBorder.
     *
     * @param  width       The width.
     * @param  height      The height.
     * @param  colorMiddle The color of the middle of the box.
     * @param  colorBorder The border color.
     * @param  borderWidth Size (width) of the border.
     * @param  boxAlpha    (Optional) The alpha of the box and the border.
     */
    BoxWithBorder(uint16_t width, uint16_t height, colortype colorMiddle, colortype colorBorder, uint16_t borderWidth, uint8_t boxAlpha = 255)
        : Box(width, height, colorMiddle, boxAlpha), borderColor(colorBorder), borderSize(borderWidth)
    {
    }

    /**
     * Sets the color of the border drawn along the edge of the BoxWithBorder.
     *
     * @param  colorBorder The color of the border.
     *
     * @see setColor, getBorderColor, Color::getColorFromRGB
     */
    void setBorderColor(colortype colorBorder)
    {
        borderColor = colorBorder;
    }

    /**
     * Gets the color of the border drawn along the edge of the BoxWithBorder.
     *
     * @return The color of the border.
     * @see setBorderColor, getColor, Color::getRed, Color::getGreen, Color::getRed
     */
    FORCE_INLINE_FUNCTION colortype getBorderColor() const
    {
        return borderColor;
    }

    /**
     * Sets the width of the border. If the width is set to 0, the BoxWithBorder will look
     * exactly like a Box.
     *
     * @param  size The width of the border.
     *
     * @see getBorderSize
     */
    void setBorderSize(uint16_t size)
    {
        borderSize = size;
    }

    /**
     * Gets the width of the border.
     *
     * @return The width of the border.
     *
     * @see setBorderSize
     */
    FORCE_INLINE_FUNCTION uint16_t getBorderSize() const
    {
        return borderSize;
    }

    virtual void draw(const Rect& area) const;

protected:
    colortype borderColor; ///< The color of the border along the edge
    uint16_t borderSize;   ///< Width of the border along the edge
};

} // namespace touchgfx

#endif // TOUCHGFX_BOXWITHBORDER_HPP
