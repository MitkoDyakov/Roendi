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
 * @file touchgfx/widgets/Box.hpp
 *
 * Declares the touchgfx::Box class.
 */
#ifndef TOUCHGFX_BOX_HPP
#define TOUCHGFX_BOX_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
/**
 * Simple widget capable of showing a rectangle of a specific color and an optional alpha.
 */
class Box : public Widget
{
public:
    /** Construct a new Box with a default alpha value of 255 (solid) */
    Box()
        : Widget(), alpha(255), color(0)
    {
    }

    /**
     * Construct a Box with the given size and color (and optionally alpha).
     *
     * @param  width    The width of the box.
     * @param  height   The height of the box.
     * @param  boxColor The color of the box.
     * @param  boxAlpha (Optional) The alpha of the box. Default is 255 (solid).
     */
    Box(uint16_t width, uint16_t height, colortype boxColor, uint8_t boxAlpha = 255)
        : Widget(), alpha(boxAlpha), color(boxColor)
    {
        rect.width = width;
        rect.height = height;
    }

    virtual Rect getSolidRect() const;

    /**
     * Sets the color of the Box.
     *
     * @param  newColor The color of the box.
     *
     * @see getColor, Color::getColorFromRGB
     */
    void setColor(colortype newColor)
    {
        color = newColor;
    }

    /**
     * Gets the current color of the Box.
     *
     * @return The current color of the box.
     *
     * @see setColor, Color::getRed, Color::getGreen, Color::getRed
     */
    FORCE_INLINE_FUNCTION colortype getColor() const
    {
        return color;
    }

    /**
     * @copydoc Image::setAlpha
     */
    void setAlpha(uint8_t newAlpha)
    {
        alpha = newAlpha;
    }

    /**
     * @copydoc Image::getAlpha
     */
    FORCE_INLINE_FUNCTION uint8_t getAlpha() const
    {
        return alpha;
    }

    virtual void draw(const Rect& area) const;

    virtual void invalidateContent() const
    {
        if (alpha > 0)
        {
            Widget::invalidateContent();
        }
    }

protected:
    uint8_t alpha;   ///< The alpha value used for this Box.
    colortype color; ///< The fill color for this Box
};

} // namespace touchgfx

#endif // TOUCHGFX_BOX_HPP
