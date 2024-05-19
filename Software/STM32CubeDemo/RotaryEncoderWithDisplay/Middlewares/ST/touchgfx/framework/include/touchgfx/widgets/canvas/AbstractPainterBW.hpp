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
 * @file touchgfx/widgets/canvas/AbstractPainterBW.hpp
 *
 * Declares the touchgfx::AbstractPainterBW class.
 */
#ifndef TOUCHGFX_ABSTRACTPAINTERBW_HPP
#define TOUCHGFX_ABSTRACTPAINTERBW_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

namespace touchgfx
{
/**
 * The AbstractPainterBW class is an abstract class for creating a painter to draw on a BW
 * display using CanvasWidgetRenderer. Pixels are either set or removed.
 *
 * @see AbstractPainter
 */
class AbstractPainterBW : public AbstractPainter
{
public:
    AbstractPainterBW()
        : AbstractPainter()
    {
        assert(compatibleFramebuffer(Bitmap::BW) && "The chosen painter only works with BW displays");
    }
};

} // namespace touchgfx

#endif // TOUCHGFX_ABSTRACTPAINTERBW_HPP
