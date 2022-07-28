/******************************************************************************
* Copyright (c) 2018(-2022) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.19.1 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

#include <touchgfx/Font.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

namespace touchgfx
{
void TextAreaWithWildcardBase::draw(const Rect& area) const
{
    if (typedText.hasValidId())
    {
        const Font* fontToDraw = typedText.getFont();
        if (fontToDraw != 0)
        {
            LCD::StringVisuals visuals(fontToDraw, color, alpha, typedText.getAlignment(), linespace, rotation, typedText.getTextDirection(), indentation, wideTextAction);
            HAL::lcd().drawString(getAbsoluteRect(), area, visuals, typedText.getText(), getWildcard1(), getWildcard2());
        }
    }
}

void TextAreaWithWildcardBase::invalidateContent() const
{
    if (alpha == 0 || !typedText.hasValidId() || rect.isEmpty())
    {
        return;
    }

    if (wideTextAction != WIDE_TEXT_NONE)
    {
        // Possibly more than one line, simply invalidate the entire TextArea
        invalidate();
        return;
    }

    // Check if it is a single line of text
    const Font* fontToDraw = typedText.getFont();
    const Unicode::UnicodeChar* textToDraw = typedText.getText();
    TextProvider textProvider;
    textProvider.initialize(textToDraw, fontToDraw->getGSUBTable(), fontToDraw->getContextualFormsTable(), getWildcard1(), getWildcard2());

    Unicode::UnicodeChar ch = textProvider.getNextChar();
    do
    {
        if (ch == newLine)
        {
            // More than one line, simply invalidate the entire TextArea
            invalidate();
            return;
        }
        ch = textProvider.getNextChar();
    } while (ch != 0);

    // Do not call our calculateBoundingArea() [it returns an invalid bounding rect],
    // but TextArea::calculateBoundingArea() that actually measures the size.
    Rect rectToInvalidate(TextArea::calculateBoundingArea().getRect());
    invalidateRect(rectToInvalidate);
}

} // namespace touchgfx
