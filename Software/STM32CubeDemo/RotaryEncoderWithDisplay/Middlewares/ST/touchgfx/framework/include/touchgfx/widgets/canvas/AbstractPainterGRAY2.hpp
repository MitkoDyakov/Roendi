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
 * @file touchgfx/widgets/canvas/AbstractPainterGRAY2.hpp
 *
 * Declares the touchgfx::AbstractPainterGRAY2 class.
 */
#ifndef TOUCHGFX_ABSTRACTPAINTERGRAY2_HPP
#define TOUCHGFX_ABSTRACTPAINTERGRAY2_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

namespace touchgfx
{
/**
 * The AbstractPainterGRAY2 class is an abstract class for creating a painter to draw on a
 * GRAY2 display using CanvasWidgetRenderer.
 *
 * @see AbstractPainter
 */
class AbstractPainterGRAY2 : public AbstractPainter
{
public:
    AbstractPainterGRAY2()
        : AbstractPainter()
    {
        assert(compatibleFramebuffer(Bitmap::GRAY2) && "The chosen painter only works with GRAY2 displays");
    }
};

} // namespace touchgfx

#endif // TOUCHGFX_ABSTRACTPAINTERGRAY2_HPP
