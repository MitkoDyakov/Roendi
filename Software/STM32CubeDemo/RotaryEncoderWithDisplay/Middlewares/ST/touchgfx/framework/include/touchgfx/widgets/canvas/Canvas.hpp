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
 * @file touchgfx/widgets/canvas/Canvas.hpp
 *
 * Declares the touchgfx::Canvas class.
 */
#ifndef TOUCHGFX_CANVAS_HPP
#define TOUCHGFX_CANVAS_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/CWRUtil.hpp>
#include <touchgfx/widgets/canvas/CanvasWidget.hpp>

namespace touchgfx
{
/**
 * Class for easy rendering using CanvasWidgetRenderer.
 *
 * The Canvas class will make implementation of a new CanvasWidget very easy. The few simple
 * primitives allows moving a "pen" and drawing the outline of a shape which can then be
 * rendered.
 *
 * The Canvas class has been optimized to eliminate drawing unnecessary lines outside the
 * currently invalidated rectangle.
 */
class Canvas
{
public:
    /**
     * Canvas Constructor. Locks the framebuffer and prepares for drawing only in the allowed area
     * which has been invalidated. The color depth of the LCD is taken into account.
     *
     * @param  painter              The painter used for drawing in the canvas.
     * @param  canvasAreaAbs        The canvas dimensions in absolute coordinates.
     * @param  invalidatedAreaRel   The area which should be updated in relative coordinates to the canvas area.
     * @param  globalAlpha          The alpha value to use when drawing in the canvas.
     *
     * @note Locks the framebuffer.
     */
    Canvas(const AbstractPainter* const painter, const Rect& canvasAreaAbs, const Rect& invalidatedAreaRel, uint8_t globalAlpha);

    /**
     * Finalizes an instance of the Canvas class.
     *
     * @note Unlocks the framebuffer.
     */
    virtual ~Canvas()
    {
    }

    /**
     * Sets the filling rule to be used when rendering the outline.
     *
     * @param  rule The filling rule.
     *
     * @see getFillingRule
     */
    void setFillingRule(Rasterizer::FillingRule rule)
    {
        rasterizer.setFillingRule(rule);
    }

    /**
     * Gets the filling rule being used when rendering the outline.
     *
     * @return The filling rule.
     *
     * @see setFillingRule
     */
    Rasterizer::FillingRule getFillingRule() const
    {
        return rasterizer.getFillingRule();
    }

    /**
     * Move the current pen position to (x, y). If the pen is outside the drawing area, nothing is
     * done, but the coordinates are saved in case the next operation is lineTo a coordinate which
     * is inside (or on the opposite side of) the drawing area.
     *
     * @param  x The x coordinate for the pen position in CWRUtil::Q5 format.
     * @param  y The y coordinate for the pen position in CWRUtil::Q5 format.
     */
    void moveTo(CWRUtil::Q5 x, CWRUtil::Q5 y);

    /**
     * Draw line from the current (x, y) to the new (x, y) as part of the shape being drawn. As for
     * moveTo, lineTo commands completely outside the drawing are are discarded.
     *
     * @param  x The x coordinate for the pen position in CWRUtil::Q5 format.
     * @param  y The y coordinate for the pen position in CWRUtil::Q5 format.
     *
     * @see CWRUtil::Q5, moveTo
     */
    virtual void lineTo(CWRUtil::Q5 x, CWRUtil::Q5 y);

    /**
     * Draw a Quadratic Bezier curve via x1,y1 to x2,y2.
     *
     * @param  x0 The start x coordinate.
     * @param  y0 The start y coordinate.
     * @param  x1 The 'via' x coordinate.
     * @param  y1 The 'via' y coordinate.
     * @param  x  The end x coordinate.
     * @param  y  The end y coordinate.
     */
    void quadraticBezierTo(float x0, float y0, const float x1, const float y1, const float x, const float y)
    {
        recursiveQuadraticBezier(x0, y0, x1, y1, x, y, 0);
        lineTo(CWRUtil::toQ5<float>(x), CWRUtil::toQ5<float>(y));
    }

    /**
     * Draw a Cubic Bezier curve via x1,y1 and x2,y2 to x3,y3.
     *
     * @param  x0 The start x coordinate.
     * @param  y0 The start y coordinate.
     * @param  x1 The first 'via' x coordinate.
     * @param  y1 The first 'via' y coordinate.
     * @param  x2 The second 'via' x coordinate.
     * @param  y2 The second 'via' y coordinate.
     * @param  x  The end x coordinate.
     * @param  y  The end y coordinate.
     */
    void cubicBezierTo(float x0, float y0, float x1, float y1, float x2, float y2, float x, float y)
    {
        recursiveCubicBezier(x0, y0, x1, y1, x2, y2, x, y, 0);
        lineTo(CWRUtil::toQ5<float>(x), CWRUtil::toQ5<float>(y));
    }

    /**
     * Closes the current shape so that the inside can be filled using a Painter.
     *
     * @return True if there is enough memory to calculate the shape outline, false if there is too
     *         little memory.
     */
    bool close();

    /**
     * Move the current pen position to (x, y). If the pen is outside (above or below)
     * the drawing area, nothing is done, but the coordinates are saved in case the next operation
     * is lineTo a coordinate which is inside (or on the opposite side of) the drawing area.
     *
     * @tparam T Either int or float.
     * @param  x The x coordinate for the pen position.
     * @param  y The y coordinate for the pen position.
     */
    template <typename T>
    void moveTo(T x, T y)
    {
        moveTo(CWRUtil::toQ5<T>(x), CWRUtil::toQ5<T>(y));
    }

    /**
     * Draw line from the current (x, y) to the new (x, y) as part of the shape being drawn. As for
     * moveTo, lineTo commands completely outside the drawing are are discarded.
     *
     * @tparam T either int or float.
     * @param  x The x coordinate for the pen position.
     * @param  y The y coordinate for the pen position.
     */
    template <typename T>
    void lineTo(T x, T y)
    {
        lineTo(CWRUtil::toQ5<T>(x), CWRUtil::toQ5<T>(y));
    }

    /**
     * Render the graphical shape drawn using moveTo() and lineTo() with the given Painter. The
     * shape is automatically closed, i.e. a lineTo() is automatically inserted connecting the
     * current pen position with the initial pen position given in the first moveTo() command.
     *
     * @param  customAlpha (Optional) Alpha to apply to the entire canvas. Useful if the canvas is
     *                     part of a more complex container setup that needs to be faded. Default is
     *                     solid.
     *
     * @return true if the widget was rendered, false if insufficient memory was available to render
     *         the widget.
     */
    bool render(uint8_t customAlpha = 255);

    /**
     * Determines if we the outline was too complex to draw completely.
     *
     * @return True if it was too complex, false if not.
     */
    FORCE_INLINE_FUNCTION bool wasOutlineTooComplex()
    {
        return rasterizer.wasOutlineTooComplex();
    }

private:
    // For drawing
    const AbstractPainter* const canvasPainter;
    int16_t canvasAreaWidth;
    uint8_t canvasAlpha;
    Rect dirtyAreaAbsolute;
    Rasterizer rasterizer;

    // Invalidate area in Q5 coordinates
    CWRUtil::Q5 invalidatedAreaX;
    CWRUtil::Q5 invalidatedAreaY;
    CWRUtil::Q5 invalidatedAreaWidth;
    CWRUtil::Q5 invalidatedAreaHeight;

    // Used for optimization of drawing algorithm
    bool isPenDown;
    bool wasPenDown;
    CWRUtil::Q5 previousX;
    CWRUtil::Q5 previousY;
    uint8_t previousOutside;
    uint8_t penDownOutside;
    CWRUtil::Q5 initialMoveToX;
    CWRUtil::Q5 initialMoveToY;

    enum
    {
        POINT_IS_ABOVE = 1 << 0,
        POINT_IS_BELOW = 1 << 1,
        POINT_IS_LEFT = 1 << 2,
        POINT_IS_RIGHT = 1 << 3
    };

    FORCE_INLINE_FUNCTION uint8_t isOutside(const CWRUtil::Q5& x, const CWRUtil::Q5& y, const CWRUtil::Q5& width, const CWRUtil::Q5& height) const
    {
        return ((y < 0) ? (POINT_IS_ABOVE) : (y >= height ? POINT_IS_BELOW : 0)) |
               ((x < 0) ? (POINT_IS_LEFT) : (x >= width ? POINT_IS_RIGHT : 0));
    }

    void transformFrameBufferToDisplay(CWRUtil::Q5& x, CWRUtil::Q5& y) const;

    void recursiveQuadraticBezier(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const unsigned level);
    void recursiveCubicBezier(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4, const unsigned level);
};

} // namespace touchgfx

#endif // TOUCHGFX_CANVAS_HPP
