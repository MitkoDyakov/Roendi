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
 * @file touchgfx/widgets/canvas/PainterRGB888Bitmap.hpp
 *
 * Declares the touchgfx::PainterRGB888Bitmap class.
 */
#ifndef TOUCHGFX_PAINTERRGB888BITMAP_HPP
#define TOUCHGFX_PAINTERRGB888BITMAP_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterBitmap.hpp>
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
class PainterRGB888Bitmap : public AbstractPainterRGB888, public AbstractPainterBitmap
{
public:
    /**
     * Constructor.
     *
     * @param  bmp (Optional) The bitmap to use in the painter.
     */
    PainterRGB888Bitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID))
        : AbstractPainterRGB888(), AbstractPainterBitmap(bmp)
    {
    }

    virtual void setBitmap(const Bitmap& bmp);

    virtual bool setup(const Rect& widgetRect) const
    {
        if (!AbstractPainterRGB888::setup(widgetRect))
        {
            return false;
        }
        updateBitmapOffsets(widgetWidth);
        return bitmap.getId() != BITMAP_INVALID;
    }

    virtual void paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const;

    virtual void tearDown() const;

    virtual HAL::RenderingMethod getRenderingMethod() const
    {
        return HAL::getInstance()->getDMAType() == DMA_TYPE_CHROMART ? HAL::HARDWARE : HAL::SOFTWARE;
    }
};

} // namespace touchgfx

#endif // TOUCHGFX_PAINTERRGB888BITMAP_HPP
