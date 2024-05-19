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

#include <touchgfx/Drawable.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/RadioButton.hpp>

namespace touchgfx
{
void RadioButton::draw(const Rect& invalidatedArea) const
{
    const Bitmap bitmap = getCurrentlyDisplayedBitmap();
    if (bitmap.getId() != BITMAP_INVALID)
    {
        Rect meAbs;
        translateRectToAbsolute(meAbs); // To find our x and y coords in absolute.

        // Calculate intersection between bitmap rect and invalidated area.
        const Rect dirtyBitmapArea = bitmap.getRect() & invalidatedArea;

        if (!dirtyBitmapArea.isEmpty())
        {
            HAL::lcd().drawPartialBitmap(bitmap, meAbs.x, meAbs.y, dirtyBitmapArea, alpha);
        }
    }
}

void RadioButton::handleClickEvent(const ClickEvent& event)
{
    const bool wasPressed = pressed;
    pressed = (event.getType() == ClickEvent::PRESSED);
    if ((pressed && !wasPressed) || (!pressed && wasPressed))
    {
        invalidate();
    }
    if (wasPressed && (event.getType() == ClickEvent::RELEASED))
    {
        if (deselectionEnabled)
        {
            setSelected(!getSelected());
        }
        else if (!getSelected())
        {
            setSelected(true);
        }
    }
}

void RadioButton::setBitmaps(const Bitmap& bmpUnselected, const Bitmap& bmpUnselectedPressed, const Bitmap& bmpSelected, const Bitmap& bmpSelectedPressed)
{
    bitmapUnselected = bmpUnselected;
    bitmapUnselectedPressed = bmpUnselectedPressed;
    bitmapSelected = bmpSelected;
    bitmapSelectedPressed = bmpSelectedPressed;

    RadioButton::setWidthHeight(bitmapUnselected);
}

Rect RadioButton::getSolidRect() const
{
    if (alpha < 255)
    {
        return Rect();
    }

    return getCurrentlyDisplayedBitmap().getSolidRect();
}

void RadioButton::setSelected(bool newSelected)
{
    const bool wasSelected = selected;
    selected = newSelected;

    if (wasSelected && !newSelected)
    {
        executeDeselectedAction();
    }

    if (!wasSelected && newSelected)
    {
        executeAction();
    }

    invalidate();
}
} // namespace touchgfx
