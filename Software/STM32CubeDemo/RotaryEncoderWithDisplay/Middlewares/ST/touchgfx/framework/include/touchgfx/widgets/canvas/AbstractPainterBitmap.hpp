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
#ifndef TOUCHGFX_ABSTRACTPAINTERBITMAP_HPP
#define TOUCHGFX_ABSTRACTPAINTERBITMAP_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/Types.hpp>
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
class AbstractPainterBitmap
{
public:
    /**
     * Constructor
     *
     * @param  bmp (Optional) The bitmap to use in the painter.
     */
    AbstractPainterBitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID))
        : bitmap(bmp), bitmapFormat(), bitmapRect(), bitmapData(0), xOffset(0), yOffset(0), isTiled(false)
    {
        setBitmap(bmp);
    }

    /** Destructor */
    virtual ~AbstractPainterBitmap()
    {
    }

    /**
     * Sets a bitmap to be used when drawing the CanvasWidget.
     *
     * @param  bmp The bitmap.
     */
    virtual void setBitmap(const Bitmap& bmp)
    {
        bitmap = bmp;
        bitmapFormat = bitmap.getFormat();
        bitmapRect = bitmap.getRect();
        DisplayTransformation::transformDisplayToFrameBuffer(bitmapRect);
        bitmapData = bitmap.getData();
    }

    /**
     * Gets the bitmap previously set using setBitmap().
     *
     * @return The bitmap.
     */
    Bitmap getBitmap() const
    {
        return bitmap;
    }

    /**
     * Instruct the painter to tile the bitmap specified. The bitmap will be tiled both horizontally
     * and vertically.
     *
     * @param  tiled True if tiled.
     *
     * @see setOffset
     */
    virtual void setTiled(bool tiled)
    {
        isTiled = tiled;
    }

    /**
     * Gets the tiled setting, previously set using setTiled().
     *
     * @return True if the bitmap is tiled, false otherwise.
     */
    bool getTiled() const
    {
        return isTiled;
    }

    /**
     * Sets an offset for the bitmap used. The x and y coordinates specifies how far the bitmap
     * should be moved to the right and down. This works for tiled bitmaps and non-tiled bitmaps.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     *
     * @see setTiled
     */
    virtual void setOffset(int16_t x, int16_t y)
    {
        xOffset = x;
        yOffset = y;
    }

    /**
     * Gets the bitmap offset previously set using setOffset().
     *
     * @param [out] x The x coordinate.
     * @param [out] y The y coordinate.
     */
    void getOffset(int16_t& x, int16_t& y)
    {
        x = xOffset;
        y = yOffset;
    }

protected:
    Bitmap bitmap;                     ///< The bitmap to be used when painting
    Bitmap::BitmapFormat bitmapFormat; ///< Copy of the bitmap format (for speed reasons)
    Rect bitmapRect;                   ///< Bitmap rectangle translated to framebuffer coordinates
    const uint8_t* bitmapData;         ///< Pointer to the bitmap data
    int16_t xOffset;                   ///< The x offset of the bitmap
    int16_t yOffset;                   ///< The y offset of the bitmap
    bool isTiled;                      ///< True if bitmap should be tiled, false if not

    /**
     * Updates the bitmap offsets described by widgetWidth, these offset are used by bitmap painters
     * adjust the coordinate position inside the widget.
     *
     * @param  widgetWidth Width of the widget.
     */
    void updateBitmapOffsets(int16_t widgetWidth) const
    {
        xDelta = (HAL::DISPLAY_ROTATION == rotate0) ? xOffset : yOffset;
        yDelta = (HAL::DISPLAY_ROTATION == rotate0) ? yOffset : -(xOffset + (widgetWidth - bitmap.getWidth()));
    }

    /**
     * Adjust bitmap x and y. For a tiled bitmap painters, the coordinates are fixed to be inside
     * the bitmap, for non-tiled bitmap painters, false is returned if the entire span (considering
     * offset and count) is outside the bitmap - otherwise, the parameters are adjust to make sure
     * that the requested number of pixels can be read from the bitmap at the given coordinate and
     * count pixels ahead.
     *
     * @param [in,out] bitmapX The bitmap x coordinate.
     * @param [in,out] bitmapY The bitmap y coordinate.
     * @param [in,out] offset    The pointer x coordinate.
     * @param [in,out] count   Number of.
     *
     * @returns True if anything should be painted, false if nothing has to be painted.
     */
    bool adjustBitmapXY(int16_t& bitmapX, int16_t& bitmapY, int16_t& offset, int16_t& count) const
    {
        bitmapX += xDelta;
        bitmapY += yDelta;
        if (isTiled)
        {
            bitmapX %= bitmapRect.width;
            if (bitmapX < 0)
            {
                bitmapX += bitmapRect.width;
            }
            bitmapY %= bitmapRect.height;
            if (bitmapY < 0)
            {
                bitmapY += bitmapRect.height;
            }
            return true;
        }

        if (bitmapX < 0)
        {
            if (bitmapX + count <= 0)
            {
                return false;
            }
            count += bitmapX;
            offset -= bitmapX;
            bitmapX = 0;
        }
        if ((bitmapX >= bitmapRect.width) || (bitmapY < 0) || (bitmapY >= bitmapRect.height))
        {
            return false;
        }
        if (bitmapX + (int)count > bitmapRect.width)
        {
            count = bitmapRect.width - bitmapX;
        }
        return true;
    }

private:
    mutable int16_t xDelta;
    mutable int16_t yDelta;
};

} // namespace touchgfx

#endif // TOUCHGFX_ABSTRACTPAINTERBITMAP_HPP
