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

#include <stdarg.h>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/TextArea.hpp>

namespace touchgfx
{

Alignment TextArea::getAlignment() const
{
    if (typedText.hasValidId())
    {
        return typedText.getAlignment();
    }
    return LEFT;
}

int16_t TextArea::getTextHeight() const
{
    return typedText.hasValidId() ? calculateTextHeight(typedText.getText(), getWildcard1(), getWildcard2()) : 0;
}

uint16_t TextArea::getTextWidth() const
{
    return typedText.hasValidId() ? typedText.getFont()->getStringWidth(typedText.getTextDirection(), typedText.getText(), getWildcard1(), getWildcard2()) : 0;
}

void TextArea::draw(const Rect& area) const
{
    if (typedText.hasValidId())
    {
        Rect rectToDraw = area;
        if (typedText.hasValidId() && boundingArea.isValid(typedText.getText()))
        {
            rectToDraw &= boundingArea.getRect();
        }
        if (!rectToDraw.isEmpty())
        {
            const Font* fontToDraw = typedText.getFont();
            if (fontToDraw != 0)
            {
                const LCD::StringVisuals visuals(fontToDraw, color, alpha, getAlignment(), linespace, rotation, typedText.getTextDirection(), indentation, wideTextAction);
                HAL::lcd().drawString(getAbsoluteRect(), rectToDraw, visuals, typedText.getText(), getWildcard1(), getWildcard2());
            }
        }
    }
}

void TextArea::setTypedText(const TypedText& t)
{
    typedText = t;
    // If this TextArea does not yet have a width and height,
    // just assign the smallest possible size to fit current text.
    if (getWidth() == 0 && getHeight() == 0)
    {
        resizeToCurrentText();
    }
    boundingArea = calculateBoundingArea();
}

void TextArea::resizeToCurrentText()
{
    if (typedText.hasValidId())
    {
        const uint16_t w = getTextWidth();
        const uint16_t h = getTextHeight();
        if (rotation == TEXT_ROTATE_0 || rotation == TEXT_ROTATE_180)
        {
            setWidthHeight(w, h);
        }
        else
        {
            setWidthHeight(h, w);
        }
    }
}

void TextArea::resizeToCurrentTextWithAlignment()
{
    if (typedText.hasValidId())
    {
        const Alignment alignment = getAlignment();
        const uint16_t text_width = getTextWidth();
        const uint16_t text_height = getTextHeight();
        if (rotation == TEXT_ROTATE_0 || rotation == TEXT_ROTATE_180)
        {
            // (rotate-0 && left-align) or (rotate-180 && right-align) places text to the left
            if (!((rotation == TEXT_ROTATE_0 && alignment == LEFT) || (rotation == TEXT_ROTATE_180 && alignment == RIGHT)))
            {
                const uint16_t old_width = getWidth();
                const uint16_t old_x = getX();
                if (alignment == CENTER)
                {
                    setX(old_x + (old_width - text_width) / 2);
                }
                else
                {
                    setX(old_x + (old_width - text_width));
                }
            }
            if (rotation == TEXT_ROTATE_180)
            {
                const uint16_t old_height = getHeight();
                const uint16_t old_y = getY();
                setY(old_y + (old_height - text_height));
            }
            setWidthHeight(text_width, text_height);
        }
        else
        {
            // 90+left or 270+right places text at the same y coordinate
            if (!((rotation == TEXT_ROTATE_90 && alignment == LEFT) || (rotation == TEXT_ROTATE_270 && alignment == RIGHT)))
            {
                const uint16_t old_height = getHeight();
                const uint16_t old_y = getY();
                if (alignment == CENTER)
                {
                    setY(old_y + (old_height - text_width) / 2);
                }
                else
                {
                    setY(old_y + (old_height - text_width));
                }
            }
            if (rotation == TEXT_ROTATE_90)
            {
                const uint16_t old_width = getWidth();
                const uint16_t old_x = getX();
                setX(old_x + (old_width - text_height));
            }
            setWidthHeight(text_height, text_width);
        }
    }
}

void TextArea::resizeHeightToCurrentText()
{
    if (typedText.hasValidId())
    {
        const uint16_t h = getTextHeight();
        if (rotation == TEXT_ROTATE_0 || rotation == TEXT_ROTATE_180)
        {
            setHeight(h);
        }
        else
        {
            setWidth(h);
        }
    }
}

void TextArea::resizeHeightToCurrentTextWithRotation()
{
    if (typedText.hasValidId())
    {
        const uint16_t h = getTextHeight();
        switch (rotation)
        {
        case TEXT_ROTATE_0:
            setHeight(h);
            break;
        case TEXT_ROTATE_90:
            setX(rect.right() - h);
            setWidth(h);
            break;
        case TEXT_ROTATE_180:
            setY(rect.bottom() - h);
            setHeight(h);
            break;
        case TEXT_ROTATE_270:
            setWidth(h);
            break;
        }
    }
}

int16_t TextArea::calculateTextHeight(const Unicode::UnicodeChar* format, ...) const
{
    if (!typedText.hasValidId())
    {
        return 0;
    }

    va_list pArg;
    va_start(pArg, format);

    const Font* fontToDraw = typedText.getFont();
    const int16_t textHeight = fontToDraw->getHeight();

    TextProvider textProvider;
    textProvider.initialize(format, pArg, fontToDraw->getGSUBTable(), fontToDraw->getContextualFormsTable());

    const int16_t numLines = LCD::getNumLines(textProvider, wideTextAction, typedText.getTextDirection(), typedText.getFont(), getWidth() - indentation);

    va_end(pArg);
    return textHeight + linespace > 0 ? numLines * textHeight + (numLines - 1) * linespace : (numLines > 0 ? textHeight : 0);
}

void TextArea::invalidateContent() const
{
    if (alpha == 0 || !typedText.hasValidId() || rect.isEmpty())
    {
        return;
    }
    if (boundingArea.isValid(typedText.getText()))
    {
        Rect boundingRect = boundingArea.getRect();
        invalidateRect(boundingRect);
        return;
    }
    invalidate();
}

TextArea::BoundingArea TextArea::calculateBoundingArea() const
{
    if (!typedText.hasValidId())
    {
        return TextArea::BoundingArea(); // Return Invalid BoundingArea
    }

    const Font* fontToDraw = typedText.getFont();
    const Unicode::UnicodeChar* textToDraw = typedText.getText();
    const int16_t fontHeight = fontToDraw->getHeight();
    const int16_t lineHeight = fontHeight + linespace;
    int16_t width = 0;
    uint16_t numberOfLines = 0;

    if (wideTextAction == WIDE_TEXT_NONE)
    {
        TextProvider textProvider;
        textProvider.initialize(textToDraw, fontToDraw->getGSUBTable(), fontToDraw->getContextualFormsTable(), getWildcard1(), getWildcard2());

        int16_t widgetRectHeight = (rotation == TEXT_ROTATE_0 || rotation == TEXT_ROTATE_180) ? getHeight() : getWidth();

        // Iterate through each line, find the longest line width and sum up the total height of the bounding rectangle
        do
        {
            const uint16_t lineWidth = LCD::stringWidth(textProvider, *(fontToDraw), 0x7FFF, typedText.getTextDirection());
            width = MAX(width, lineWidth);
            numberOfLines++;
            widgetRectHeight -= lineHeight;
            // Keep reading until end of string or next line completely invisible.
        } while (!textProvider.endOfString() && widgetRectHeight + fontToDraw->getPixelsAboveTop() > 0);
    }
    else
    {
        TextProvider wideTextProvider;
        wideTextProvider.initialize(textToDraw, fontToDraw->getGSUBTable(), fontToDraw->getContextualFormsTable(), getWildcard1(), getWildcard2());

        const int16_t widgetRectWidth = (rotation == TEXT_ROTATE_0 || rotation == TEXT_ROTATE_180) ? getWidth() : getHeight();
        int16_t widgetRectHeight = (rotation == TEXT_ROTATE_0 || rotation == TEXT_ROTATE_180) ? getHeight() : getWidth();
        LCD::WideTextInternalStruct wtis(wideTextProvider, widgetRectWidth - indentation, widgetRectHeight, typedText.getTextDirection(), fontToDraw, linespace, wideTextAction);

        // Iterate through each line, find the longest line width and sum up the total height of the bounding rectangle
        do
        {
            wtis.scanStringLengthForLine();
            const uint16_t lineWidth = wtis.getLineWidth();
            width = MAX(width, lineWidth);
            numberOfLines++;
            widgetRectHeight -= lineHeight;
            // Keep reading until end of string, ellipsis inserted or next line completely invisible.
        } while (wtis.getCurrChar() != 0 && !wtis.ellipsisAtEndOfLine() && widgetRectHeight + fontToDraw->getPixelsAboveTop() > 0);
    }
    int16_t height = (numberOfLines * lineHeight) - linespace;
    height = MAX(height, fontHeight) + fontToDraw->getPixelsBelowBottom();

    Rect boundingRect(0, 0, width, height);

    // Adjust for alignment
    const int16_t areaWidth = (rotation == TEXT_ROTATE_0 || rotation == TEXT_ROTATE_180) ? getWidth() : getHeight();
    switch (getAlignment())
    {
    default:
    case LEFT:
        boundingRect.x = indentation;
        break;
    case CENTER:
        boundingRect.x = ((areaWidth - boundingRect.width) / 2);
        break;
    case RIGHT:
        boundingRect.x = areaWidth - (boundingRect.width + indentation);
        break;
    }

    // Adjust for left and right pixels offsets
    const uint8_t maxPixelsLeft = fontToDraw->getMaxPixelsLeft();
    const uint8_t maxPixelsRight = fontToDraw->getMaxPixelsRight();
    boundingRect.x -= maxPixelsLeft;
    boundingRect.width += (maxPixelsLeft + maxPixelsRight);

    // Adjust for rotation
    switch (rotation)
    {
    case TEXT_ROTATE_0:
        break;
    case TEXT_ROTATE_90:
        boundingRect = Rect(getWidth() - boundingRect.bottom(), boundingRect.x, boundingRect.height, boundingRect.width);
        break;
    case TEXT_ROTATE_180:
        boundingRect = Rect(getWidth() - boundingRect.right(), getHeight() - boundingRect.bottom(), boundingRect.width, boundingRect.height);
        break;
    case TEXT_ROTATE_270:
        boundingRect = Rect(boundingRect.y, getHeight() - boundingRect.right(), boundingRect.height, boundingRect.width);
        break;
    }

    return TextArea::BoundingArea(boundingRect, typedText.getText());
}

} // namespace touchgfx
