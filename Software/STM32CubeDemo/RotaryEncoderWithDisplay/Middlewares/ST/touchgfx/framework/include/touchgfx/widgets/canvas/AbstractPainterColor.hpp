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
 * @file touchgfx/widgets/canvas/AbstractPainterBitmap.hpp
 *
 * Declares the touchgfx::AbstractPainterBitmap class.
 */
#ifndef TOUCHGFX_ABSTRACTPAINTERCOLOR_HPP
#define TOUCHGFX_ABSTRACTPAINTERCOLOR_HPP

#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * An abstract class for creating painter classes for drawing canvas widgets. All canvas widgets
 * need a painter to fill the shape drawn with a CanvasWidgetRenderer. The painter must provide the
 * color of a pixel on a given coordinate, which will the be blended into the framebuffer depending
 * on the position of the canvas widget and the transparency of the given pixel.
 *
 * The AbstractPainter also implements a function which will blend each pixel in a scanline snippet
 * into the framebuffer, but for better performance, the function should be reimplemented in each
 * painter.
 */
class AbstractPainterColor
{
public:
    /**
     * Constructor.
     *
     * @param  color (Optional) The color to use in the painter.
     */
    AbstractPainterColor(colortype color = 0)
    {
        setColor(color);
    }

    /** Destructor */
    virtual ~AbstractPainterColor()
    {
    }

    /**
     * Sets color to use when drawing the CanvasWidget.
     *
     * @param  color The color.
     */
    virtual void setColor(colortype color)
    {
        painterColor = color;
    }

    /**
     * Gets the current color.
     *
     * @return The color.
     */
    colortype getColor() const
    {
        return painterColor;
    }

protected:
    colortype painterColor; ///< The color
};

} // namespace touchgfx

#endif // TOUCHGFX_ABSTRACTPAINTERCOLOR_HPP
