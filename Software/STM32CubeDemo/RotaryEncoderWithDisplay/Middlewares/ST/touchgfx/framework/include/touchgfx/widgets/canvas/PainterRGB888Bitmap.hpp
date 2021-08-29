/******************************************************************************
* Copyright (c) 2018(-2021) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.17.0 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

/**
 * @file touchgfx/widgets/canvas/PainterRGB888Bitmap.hpp
 *
 * Declares the touchgfx::PainterRGB888Bitmap class.
 */
#ifndef TOUCHGFX_PAINTERRGB888BITMAP_HPP
#define TOUCHGFX_PAINTERRGB888BITMAP_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterRGB888.hpp>

namespace touchgfx
{
/**
 * PainterRGB888Bitmap will take the color for a given point in the shape from a bitmap.
 * Please be aware, the the bitmap is used by the CanvasWidgetRenderer (not Shape), so
 * any rotation you might specify for a Canvas Widget (e.g. Shape) is not applied to the
 * bitmap as CWR is not aware of this rotation.
 *
 * @see AbstractPainter
 */
class PainterRGB888Bitmap : public AbstractPainterRGB888
{
public:
    /**
     * Initializes a new instance of the PainterRGB888Bitmap class.
     *
     * @param  bmp   (Optional) The bitmap, default is #BITMAP_INVALID.
     */
    PainterRGB888Bitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID))
        : AbstractPainterRGB888(), bitmapARGB8888Pointer(0), bitmapRGB888Pointer(0), bitmap(), bitmapRectToFrameBuffer()
    {
        setBitmap(bmp);
    }

    /**
     * Sets a bitmap to be used when drawing the CanvasWidget.
     *
     * @param  bmp The bitmap.
     */
    void setBitmap(const Bitmap& bmp);

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

protected:
    virtual bool renderInit();

    virtual bool renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha);

    const uint32_t* bitmapARGB8888Pointer; ///< Pointer to the bitmap (ARGB8888)
    const uint8_t* bitmapRGB888Pointer;    ///< Pointer to the bitmap (RGB888)

    Bitmap bitmap;                ///< The bitmap to be used when painting
    Rect bitmapRectToFrameBuffer; ///< Bitmap rectangle translated to framebuffer coordinates
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERRGB888BITMAP_HPP
