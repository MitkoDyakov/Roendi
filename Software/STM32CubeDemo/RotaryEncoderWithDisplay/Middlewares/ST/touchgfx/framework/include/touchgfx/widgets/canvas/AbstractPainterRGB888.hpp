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
 * @file touchgfx/widgets/canvas/AbstractPainterRGB888.hpp
 *
 * Declares the touchgfx::AbstractPainterRGB888 class.
 */
#ifndef TOUCHGFX_ABSTRACTPAINTERRGB888_HPP
#define TOUCHGFX_ABSTRACTPAINTERRGB888_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

namespace touchgfx
{
/**
 * The AbstractPainterRGB888 class is an abstract class for creating a painter to draw on a
 * RGB888 display using CanvasWidgetRenderer.
 *
 * @see AbstractPainter
 */
class AbstractPainterRGB888 : public AbstractPainter
{
public:
    AbstractPainterRGB888()
        : AbstractPainter()
    {
        assert(compatibleFramebuffer(Bitmap::RGB888) && "The chosen painter only works with RGB888 displays");
    }
};

} // namespace touchgfx

#endif // TOUCHGFX_ABSTRACTPAINTERRGB888_HPP
