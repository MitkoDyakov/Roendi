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
 * @file touchgfx/widgets/Widget.hpp
 *
 * Declares the touchgfx::Widget class.
 */
#ifndef TOUCHGFX_WIDGET_HPP
#define TOUCHGFX_WIDGET_HPP

#include <touchgfx/Drawable.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * A Widget is an element which can be displayed (drawn) in the framebuffer. Hence a Widget is a
 * subclass of Drawable. It implements getLastChild(), but leaves the implementation of
 * draw() and getSolidRect() to subclasses of Widget, so it is still an abstract class.
 *
 * If a Widget contains more than one logical element, consider implementing several
 * subclasses of Widget and create a Container with the Widgets.
 *
 * @see Drawable
 */
class Widget : public Drawable
{
public:
    /**
     * Since a Widget is only one Drawable, Widget::getLastChild simply yields itself as
     * result, but only if the Widget isVisible and isTouchable.
     *
     * @param [out] last Result, the address of the actual instance of the Widget.
     */
    virtual void getLastChild(int16_t /*x*/, int16_t /*y*/, Drawable** last)
    {
        if (isVisible() && isTouchable())
        {
            *last = this;
        }
    }
};

} // namespace touchgfx

#endif // TOUCHGFX_WIDGET_HPP
