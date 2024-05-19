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
 * @file touchgfx/canvas_widget_renderer/Cell.hpp
 *
 * Declares the touchgfx::Cell struct. Used internally by CanvasWidgetRenderer.
 */
#ifndef TOUCHGFX_CELL_HPP
#define TOUCHGFX_CELL_HPP

#include <touchgfx/hal/Types.hpp>

/// @cond
namespace touchgfx
{
/**
 * A pixel cell. There are no constructors defined and it was done intentionally in order to
 * avoid extra overhead when allocating an array of cells.
 */
struct Cell
{
    int16_t x;     ///< The x coordinate
    int16_t y;     ///< The y coordinate
    int16_t cover; ///< The cover (see http://projects.tuxee.net/cl-vectors/section-the-cl-aa-algorithm for further information).
    int16_t area;  ///< The area (see http://projects.tuxee.net/cl-vectors/section-the-cl-aa-algorithm for further information).

    /**
     * Sets all the Cell parameters.
     *
     * @param  cellX     The x coordinate.
     * @param  cellY     The y coordinate.
     * @param  cellCover The cover.
     * @param  cellArea  The area.
     */
    FORCE_INLINE_FUNCTION void set(int cellX, int cellY, int cellCover, int cellArea)
    {
        setCoord(cellX, cellY);
        setCover(cellCover, cellArea);
    }

    /**
     * Sets the coordinate of the Cell.
     *
     * @param  cellX The Cell's x coordinate.
     * @param  cellY The Cell's y coordinate.
     */
    FORCE_INLINE_FUNCTION void setCoord(int cellX, int cellY)
    {
        x = (int16_t)cellX;
        y = (int16_t)cellY;
    }

    /**
     * Sets the cover of area cell.
     *
     * @param  cellCover The cover.
     * @param  cellArea  The area.
     */
    FORCE_INLINE_FUNCTION void setCover(int cellCover, int cellArea)
    {
        cover = cellCover;
        area = cellArea;
    }

    /**
     * Adds a cover to a Cell.
     *
     * @param  cellCover The cover to add to the Cell.
     * @param  cellArea  The area to add to the Cell.
     */
    FORCE_INLINE_FUNCTION void addCover(int cellCover, int cellArea)
    {
        cover += cellCover;
        area += cellArea;
    }
}; // struct Cell

} // namespace touchgfx

/// @endcond

#endif // TOUCHGFX_CELL_HPP
