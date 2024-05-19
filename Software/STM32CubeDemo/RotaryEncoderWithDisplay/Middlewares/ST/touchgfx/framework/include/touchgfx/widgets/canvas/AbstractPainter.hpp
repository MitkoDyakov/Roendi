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
 * @file touchgfx/widgets/canvas/AbstractPainter.hpp
 *
 * Declares the touchgfx::AbstractPainter class.
 */
#ifndef TOUCHGFX_ABSTRACTPAINTER_HPP
#define TOUCHGFX_ABSTRACTPAINTER_HPP

#include <string.h>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>

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
class AbstractPainter
{
public:
    /** Initializes a new instance of the AbstractPainter class. */
    AbstractPainter()
        : widgetWidth(0)
    {
    }

    /** Finalizes an instance of the AbstractPainter class. */
    virtual ~AbstractPainter()
    {
    }

    /**
     * Return the applicable rendering method of the painter.
     * HARDWARE if the painter uses hardware based drawing,
     * SOFTWARE otherwise. Relevant on platforms using DCACHE.
     *
     * @return HARDWARE if painter uses hardware based drawing, SOFTWARE otherwise.
     */
    virtual HAL::RenderingMethod getRenderingMethod() const
    {
        return HAL::SOFTWARE;
    }

    /**
     * This function is called before any actual paint operation is started. This allows the painter
     * to initialize variables and optional dma queues. This happens before the first scan line
     * segment is being drawn. If the painter is unable to draw, e.g. no bitmap assigned to a bitmap
     * painter, 'false' should be returned.
     *
     * @param  widgetRect The widget rectangle.
     *
     * @return True if the painter is ready to paint, false otherwise.
     *
     * @see AbstractPainter::paint, AbstractPainter::tearDown
     */
    virtual bool setup(const Rect& widgetRect) const
    {
        widgetWidth = widgetRect.width;
        return true;
    }

    /**
     * This function is called after all paint operation have finished. This allows the painter to
     * close down dma queues etc. This happens after the last scan line segment has been drawn.
     *
     * @see AbstractPainter::paint, AbstractPainter::setup
     */
    virtual void tearDown() const
    {
        return;
    }

    /**
     * Convert Framebuffer widget coordinates to display widget coordinates. This is handy when
     * creating a custom painter and the X,Y is needed in the same coordinate system as the widget
     * on the display.
     *
     * @param [in,out] widgetX The widget x coordinate.
     * @param [in,out] widgetY The widget y coordinate.
     *
     * @see AbstractPainter::paint
     */
    void framebufferToDisplay(int16_t& widgetX, int16_t& widgetY) const
    {
        if (HAL::DISPLAY_ROTATION == rotate0)
        {
            return;
        }
        const int16_t tmpX = widgetX;
        widgetX = (widgetWidth - 1) - widgetY;
        widgetY = tmpX;
    }

    /**
     * Paints a streak of pixels (all with the same alpha) in the framebuffer. The first pixel to
     * paint is at 'offset' relative to 'destination' (to be able to support 1bpp, 2bpp and 4bpp).
     * The first pixel to paint is at the given 'widgetX', 'widgetY' coordinate. The number of
     * pixels to paint is 'count' and the alpha to apply is 'alpha'.
     *
     * Note that the widgetX, widgetY is relative to the widget in the framebuffer, not the display.
     * To convert the coordinate to display widget coordinates, use framebufferToDisplay().
     *
     * @param [in] destination If non-null, the pointer.
     * @param      offset      The offset to add to the destination.
     * @param      widgetX     The widget x coordinate.
     * @param      widgetY     The widget y coordinate.
     * @param      count       Number of pixels.
     * @param      alpha       The alpha of the pixels.
     *
     * @see AbstractPainter::setup, AbstractPainter::tearDown, AbstractPainter::framebufferToDisplay
     */
    virtual void paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const = 0;

protected:
    /**
     * Helper function to check if the provided bitmap format matches the current
     * framebuffer format.
     *
     * @param  format A bitmap format.
     *
     * @return True if the format matches the framebuffer format, false otherwise.
     */
    FORCE_INLINE_FUNCTION static bool compatibleFramebuffer(Bitmap::BitmapFormat format)
    {
        bool compat = HAL::lcd().framebufferFormat() == format;
        if (HAL::getInstance()->getAuxiliaryLCD())
        {
            compat |= HAL::getInstance()->getAuxiliaryLCD()->framebufferFormat() == format;
        }
        return compat;
    }

    mutable int16_t widgetWidth; ///< The width of the widget on screen, used by framebufferToDisplay()
};

} // namespace touchgfx

#endif // TOUCHGFX_ABSTRACTPAINTER_HPP
