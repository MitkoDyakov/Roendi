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
 * @file touchgfx/widgets/canvas/PainterGRAY2Bitmap.hpp
 *
 * Declares the touchgfx::PainterGRAY2Bitmap class.
 */
#ifndef TOUCHGFX_PAINTERGRAY2BITMAP_HPP
#define TOUCHGFX_PAINTERGRAY2BITMAP_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterBitmap.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterGRAY2.hpp>

namespace touchgfx
{
/**
 * PainterGRAY2Bitmap will take the color for a given point in the shape from a bitmap. Please
 * be aware, the the bitmap is used by the CanvasWidgetRenderer (not Shape), so any
 * rotation you might specify for a Canvas Widget (e.g. Shape) is not applied to the
 * bitmap as CWR is not aware of this rotation.
 *
 * @see AbstractPainter
 */
class PainterGRAY2Bitmap : public AbstractPainterGRAY2, public AbstractPainterBitmap
{
public:
    /**
     * Constructor.
     *
     * @param  bmp (Optional) The bitmap to use in the painter.
     */
    PainterGRAY2Bitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID))
        : AbstractPainterGRAY2(), AbstractPainterBitmap(bmp)
    {
    }

    virtual void setBitmap(const Bitmap& bmp);

    virtual bool setup(const Rect& widgetRect) const
    {
        if (!AbstractPainterGRAY2::setup(widgetRect))
        {
            return false;
        }
        updateBitmapOffsets(widgetWidth);
        return bitmap.getId() != BITMAP_INVALID;
    }

    virtual void paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const;

protected:
    const uint8_t* bitmapExtraData; ///< Pointer to the bitmap extra data
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERGRAY2BITMAP_HPP
