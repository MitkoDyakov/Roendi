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
 * @file touchgfx/widgets/canvas/AbstractPainterGRAY4.hpp
 *
 * Declares the touchgfx::AbstractPainterGRAY4 class.
 */
#ifndef TOUCHGFX_ABSTRACTPAINTERGRAY4_HPP
#define TOUCHGFX_ABSTRACTPAINTERGRAY4_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

namespace touchgfx
{
/**
 * The AbstractPainterGRAY4 class is an abstract class for creating a painter to draw on a
 * GRAY4 display using CanvasWidgetRenderer.
 *
 * @see AbstractPainter
 */
class AbstractPainterGRAY4 : public AbstractPainter
{
public:
    AbstractPainterGRAY4()
        : AbstractPainter()
    {
        assert(compatibleFramebuffer(Bitmap::GRAY4) && "The chosen painter only works with GRAY4 displays");
    }
};

} // namespace touchgfx

#endif // TOUCHGFX_ABSTRACTPAINTERGRAY4_HPP
