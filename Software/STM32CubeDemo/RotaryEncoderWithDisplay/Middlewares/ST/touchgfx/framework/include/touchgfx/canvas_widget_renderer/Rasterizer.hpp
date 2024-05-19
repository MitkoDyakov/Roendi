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
 * @file touchgfx/canvas_widget_renderer/Rasterizer.hpp
 *
 * Declares the touchgfx::Rasterizer class. Used internally by CanvasWidgetRenderer.
 */
#ifndef TOUCHGFX_RASTERIZER_HPP
#define TOUCHGFX_RASTERIZER_HPP

#include <touchgfx/canvas_widget_renderer/Outline.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

/// @cond
namespace touchgfx
{
/**
 * Polygon Rasterizer that is used to render filled polygons with high-quality Anti- Aliasing.
 *
 * Polygon Rasterizer that is used to render filled polygons with high-quality Anti-
 * Aliasing. Internally, by default, the class uses integer coordinates in format 24.8,
 * i.e. 24 bits for integer part and 8 bits for fractional - see POLY_BASE_SHIFT. This
 * class can be used in the following  way:
 *
 * 1. setFillingRule(FillingRule fr) - optional.
 *
 * 2. reset()
 *
 * 3. moveTo(x, y) / lineTo(x, y) - make the polygon. One can create
 *    more than one contour, but each contour must consist of at least 3 vertices, i.e.
 *    moveTo(x1, y1); lineTo(x2, y2); lineTo(x3, y3);
 *    is the absolute minimum of vertices that define a triangle. The algorithm does not
 *    check either the number of vertices nor coincidence of their coordinates, but in
 *    the worst case it just won't draw anything. The order of the vertices (clockwise
 *    or counterclockwise)
 *    is important when using the non-zero filling rule (fill_non_zero). In this case
 *    the vertex order of all the contours must be the same if you want your
 *    intersecting polygons to be without "holes". You actually can use different
 *    vertices order. If the contours do not intersect each other the order is not
 *    important anyway. If they do, contours with the same vertex order will be rendered
 *    without "holes" while the intersecting contours with different orders will have
 *    "holes".
 *
 * setFillingRule() can be called anytime before "sweeping".
 */
class Rasterizer
{
public:
    /**
     * Determine the sub pixel accuracy, to be more precise, the number of bits of the
     * fractional part of the coordinates.
     */
    enum
    {
        POLY_BASE_SHIFT = 5,                   ///< Number of bits reserved for fraction part
        POLY_BASE_SIZE = 1 << POLY_BASE_SHIFT, ///< The value to divide or multiply with to convert to / from this format
        POLY_BASE_MASK = POLY_BASE_SIZE - 1    ///< The value used to mask the fraction
    };

    /**
     * Determine the area accuracy, to be more precise, the number of bits of the fractional
     * part of the areas when calculating scan lines.
     */
    enum
    {
        AA_SHIFT = 8,           ///< Number of bits reserved for fraction part when calculating the area
        AA_NUM = 1 << AA_SHIFT, ///< The value to divide or multiply with to convert to / from this format
        AA_MASK = AA_NUM - 1,   ///< The value used to mask the fraction
        AA_2NUM = AA_NUM * 2,   ///< Number of fraction bits when multiplying two area numbers
        AA_2MASK = AA_2NUM - 1  ///< Mask for fraction bits when multiplying two area numbers
    };

    /** Values that represent filling rules. */
    enum FillingRule
    {
        FILL_NON_ZERO, ///< Filling rule to fill anything inside the outermost border of the outline.
        FILL_EVEN_ODD  ///< Filling rule to fill using xor rule inside the outline.
    };

    /** Initializes a new instance of the Rasterizer class. */
    Rasterizer()
        : outline(), fillingRule(FILL_NON_ZERO), offsetX(0), offsetY(0), width(0)
    {
    }

    /** Resets this object. Basically this is done by resetting the the Outline. */
    void reset(int16_t offset_x, int16_t offset_y)
    {
        offsetX = offset_x;
        offsetY = offset_y;
        outline.reset();
    }

    /**
     * Sets the filling rule to be used when rendering the outline.
     *
     * @param  rule The filling rule.
     *
     * @see getFillingRule
     */
    void setFillingRule(FillingRule rule)
    {
        fillingRule = rule;
    }

    /**
     * Gets the filling rule being used when rendering the outline.
     *
     * @return The filling rule.
     *
     * @see setFillingRule
     */
    FillingRule getFillingRule() const
    {
        return fillingRule;
    }

    /**
     * Move to.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     */
    void moveTo(int x, int y)
    {
#ifndef SIMULATOR
        if (!outline.wasOutlineTooComplex())
#endif
        {
            outline.moveTo(x, y);
        }
    }

    /**
     * Line to.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     */
    void lineTo(int x, int y)
    {
#ifndef SIMULATOR
        if (!outline.wasOutlineTooComplex())
#endif
        {
            outline.lineTo(x, y);
        }
    }

    /**
     * Calculates the alpha.
     *
     * @param  area The area.
     *
     * @return The calculated alpha.
     */
    unsigned calculateAlpha(int area) const
    {
        int cover = area >> (POLY_BASE_SHIFT * 2 + 1 - AA_SHIFT);

        if (cover < 0)
        {
            cover = -cover;
        }
        if (fillingRule == FILL_EVEN_ODD)
        {
            cover &= AA_2MASK;
            if (cover > AA_NUM)
            {
                cover = AA_2NUM - cover;
            }
        }
        if (cover > AA_MASK)
        {
            cover = AA_MASK;
        }
        return cover;
    }

    /**
     * Renders this object.
     *
     * @return true there was enough memory available to draw the outline and render the
     *         graphics, false if there was insufficient memory and nothing was drawn.
     */
    bool render(const AbstractPainter* const painter, uint8_t* buf, int16_t stride, uint8_t xAdjust, uint8_t global_alpha)
    {
        const Cell* curCell = outline.closeOutlineAndSortCells(); // has side effects, so it must be first in this function
        unsigned numCells = outline.getNumCells();
        if (numCells == 0)
        {
            return true;
        }

        // Not enough memory allocated, the outline will look wrong. Do not proceed with the drawing.
        if (outline.wasOutlineTooComplex())
        {
            return false;
        }

        int cover = 0;
        int old_y = curCell->y;
        int widget_y = old_y + offsetY;
        uint8_t* buf_ptr = buf + old_y * stride;
        numCells--;
        for (;;)
        {
            const Cell* startCell = curCell;

            const int start_x = curCell->x;
            int x = start_x;
            const int y = curCell->y;
            if (y != old_y)
            {
                old_y = y;
                widget_y = old_y + offsetY;
                buf_ptr = buf + old_y * stride;
            }

            int area = startCell->area;
            cover += startCell->cover;

            // Accumulate all start cells
            while (numCells-- > 0)
            {
                curCell++;
                if (curCell->x != start_x || curCell->y != y)
                {
                    break;
                }
                area += curCell->area;
                cover += curCell->cover;
            }

            if (area)
            {
                if (x >= 0 && x < width)
                {
                    const int8_t alpha = LCD::div255(calculateAlpha((cover << (POLY_BASE_SHIFT + 1)) - area) * global_alpha);
                    if (alpha)
                    {
                        painter->paint(buf_ptr, x + xAdjust, x + offsetX, widget_y, 1, alpha);
                    }
                }
                x++;
            }

            if (numCells == unsigned(-1))
            {
                break;
            }

            int count = curCell->x - x;
            if (count > 0)
            {
                if (x < 0)
                {
                    count += x;
                    x = 0;
                }
                if (count > 0)
                {
                    if (x + count >= width)
                    {
                        count = width - x;
                    }
                    if (count > 0)
                    {
                        const int8_t alpha = LCD::div255(calculateAlpha(cover << (POLY_BASE_SHIFT + 1)) * global_alpha);
                        if (alpha)
                        {
                            painter->paint(buf_ptr, x + xAdjust, x + offsetX, widget_y, count, alpha);
                        }
                    }
                }
            }
        }
        return true;
    }

    /**
     * Sets maximum render y coordinate. This is passed to the Outline to avoid registering
     * any Cell that has a y coordinate less than zero of higher than the given y.
     *
     * @param  y The max y coordinate to render for the Outline.
     */
    void setMaxRender(int16_t w, int16_t h)
    {
        width = w;
        outline.setMaxRender(w, h);
    }

    /**
     * Determines if we the outline was too complex to draw completely.
     *
     * @return True if it was too complex, false if not.
     */
    FORCE_INLINE_FUNCTION bool wasOutlineTooComplex()
    {
        return outline.wasOutlineTooComplex();
    }

private:
    /**
     * Copy constructor.
     *
     * @param  parameter1 The first parameter.
     */
    Rasterizer(const Rasterizer&);

    /**
     * Assignment operator.
     *
     * @param  parameter1 The first parameter.
     *
     * @return A shallow copy of this object.
     */
    const Rasterizer& operator=(const Rasterizer&);

    Outline outline;         ///< The outline
    FillingRule fillingRule; ///< The filling rule

    int16_t offsetX;
    int16_t offsetY;
    int16_t width;
};

} // namespace touchgfx

/// @endcond

#endif // TOUCHGFX_RASTERIZER_HPP
