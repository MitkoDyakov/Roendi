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
 * @file touchgfx/widgets/canvas/AbstractPainterARGB8888.hpp
 *
 * Declares the touchgfx::AbstractPainterARGB8888 class.
 */
#ifndef TOUCHGFX_ABSTRACTPAINTERARGB8888_HPP
#define TOUCHGFX_ABSTRACTPAINTERARGB8888_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

namespace touchgfx
{
/**
 * The AbstractPainterARGB8888 class is an abstract class for creating a painter to draw on a
 * ARGB8888 display using CanvasWidgetRenderer.
 *
 * @see AbstractPainter
 */
class AbstractPainterARGB8888 : public AbstractPainter
{
public:
    AbstractPainterARGB8888()
        : AbstractPainter()
    {
        assert(compatibleFramebuffer(Bitmap::ARGB8888) && "The chosen painter only works with ARGB8888 displays");
    }
};

} // namespace touchgfx

#endif // TOUCHGFX_ABSTRACTPAINTERARGB8888_HPP
