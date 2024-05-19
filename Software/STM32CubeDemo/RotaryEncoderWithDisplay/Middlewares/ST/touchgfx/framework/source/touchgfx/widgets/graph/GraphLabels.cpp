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
#include <touchgfx/widgets/graph/GraphLabels.hpp>

namespace touchgfx
{

void GraphLabelsBase::draw(const Rect& invalidatedArea) const
{
    if (!labelTypedText.hasValidId())
    {
        return;
    }
    const Font* fontToDraw = labelTypedText.getFont();
    if (!fontToDraw)
    {
        return;
    }

    const AbstractDataGraph* graph = getGraph();
    const uint8_t a = LCD::div255(getAlpha() * graph->getAlpha());
    if (a == 0)
    {
        return;
    }

    const int minorInterval = getCorrectlyScaledLabelInterval(graph);
    const int majorInterval = getCorrectlyScaledMajorInterval(graph);

    if (majorInterval == 0 && minorInterval == 0)
    {
        drawString(invalidatedArea, fontToDraw, graph, 0, 0, a);
    }
    else if (minorInterval > 0)
    {
        int rangeMin = getGraphRangeMinScaled(graph);
        int rangeMax = getGraphRangeMaxScaled(graph);
        if (rangeMin > rangeMax)
        {
            const int tmp = rangeMin;
            rangeMin = rangeMax;
            rangeMax = tmp;
        }
        if ((rangeMax - rangeMin) / minorInterval > 100)
        {
            return; // Too many labels
        }
        drawIndexRange(invalidatedArea, fontToDraw, graph, rangeMin, rangeMax, minorInterval, majorInterval, a);
    }
}

void GraphLabelsBase::drawIndexRange(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, const int rangeMin, const int rangeMax, const int minorInterval, const int majorInterval, const uint8_t a) const
{
    if (minorInterval == 0)
    {
        if ((0 >= rangeMin && 0 <= rangeMax) || (0 >= rangeMax && 0 <= rangeMin))
        {
            drawString(invalidatedArea, fontToDraw, graph, 0, 0, a);
        }
        return;
    }

    const int minorLo = (int)(rangeMin / minorInterval) - 1;
    const int minorHi = (int)(rangeMax / minorInterval) + 1;
    if (majorInterval == 0)
    {
        for (int minorIndex = minorLo; minorIndex != minorHi + 1; minorIndex++)
        {
            const int minorValue = (int)(minorInterval * minorIndex);
            if ((minorValue >= rangeMin && minorValue <= rangeMax) || (minorValue >= rangeMax && minorValue <= rangeMin))
            {
                drawString(invalidatedArea, fontToDraw, graph, minorValue, labelInterval * minorIndex, a);
            }
        }
    }
    else
    {
        const int majorLo = (int)(rangeMin / majorInterval) - 1;
        const int majorHi = (int)(rangeMax / majorInterval) + 1;
        int majorIndex = majorLo;
        int majorValue = majorInterval * majorIndex;
        int minorIndex = minorLo;
        int minorValue = minorInterval * minorIndex;
        for (;;)
        {
            // Draw strings up to the major
            while (minorValue < majorValue)
            {
                if ((minorValue >= rangeMin && minorValue <= rangeMax) || (minorValue >= rangeMax && minorValue <= rangeMin))
                {
                    drawString(invalidatedArea, fontToDraw, graph, minorValue, labelInterval * minorIndex, a);
                }
                minorIndex++;
                minorValue += minorInterval;
            }
            // Advance minor past the major we are about to draw
            while (minorValue <= majorValue)
            {
                minorIndex++;
                minorValue += minorInterval;
            }
            if (majorValue < minorValue)
            {
                majorIndex++;
                if (majorIndex == majorHi + 1)
                {
                    break;
                }
                majorValue += majorInterval;
            }
        }
    }
}

void GraphLabelsBase::formatLabel(Unicode::UnicodeChar* buffer, int16_t bufferSize, int label, int decimals, Unicode::UnicodeChar decimalPoint, int scale) const
{
    int length = 0;
    if (label < 0 && length < bufferSize - 1)
    {
        buffer[length++] = '-';
        label = -label;
    }
    if (decimals == 0)
    {
        Unicode::snprintf(buffer + length, bufferSize - length, "%d", (label + scale / 2) / scale);
    }
    else if (decimals > 0)
    {
        Unicode::snprintf(buffer + length, bufferSize - length, "%d", label / scale);
        length = Unicode::strlen(buffer);
        if (length < bufferSize - 1)
        {
            buffer[length++] = decimalPoint;
            int remainder = label % scale;
            for (int i = 0; i < decimals && length < bufferSize - 1; i++)
            {
                remainder *= 10;
                if (i == decimals - 1 || length == bufferSize - 1)
                {
                    remainder += scale / 2; // Rounding on the last (visible) digit
                }
                const int digit = (remainder / scale);
                buffer[length++] = (Unicode::UnicodeChar)('0' + digit);
                remainder %= scale;
            }
            buffer[length] = (Unicode::UnicodeChar)0;
        }
    }
}

void GraphLabelsX::invalidateGraphPointAt(int16_t index)
{
    if (!labelTypedText.hasValidId())
    {
        return;
    }
    const Font* fontToDraw = labelTypedText.getFont();
    if (!fontToDraw)
    {
        return;
    }

    const AbstractDataGraph* graph = getGraph();
    const uint8_t a = LCD::div255(getAlpha() * graph->getAlpha());
    if (a == 0)
    {
        return;
    }

    const int scaledIndex = graph->int2scaledX(index);
    const int minorInterval = getCorrectlyScaledLabelInterval(graph);
    const int majorInterval = getCorrectlyScaledMajorInterval(graph);

    const bool isOnMinor = (minorInterval > 0 && scaledIndex == minorInterval * (int)(scaledIndex / minorInterval));
    const bool isOnMajor = (majorInterval > 0 && scaledIndex == majorInterval * (int)(scaledIndex / majorInterval));
    if ((majorInterval == 0 && minorInterval == 0 && scaledIndex == 0) || (isOnMinor && !isOnMajor))
    {
        Unicode::UnicodeChar wildcard[20];
        const int labelScaled = (minorInterval == 0) ? 0 : (scaledIndex / minorInterval) * labelInterval;
        formatLabel(wildcard, 20, getIndexToXAxis(graph, scaledIndex, labelScaled), labelDecimals, labelDecimalPoint, dataScale);
        // Adjust to make label centered
        int16_t labelWidth;
        const Unicode::UnicodeChar* text = labelTypedText.getText();
        if (labelRotation == TEXT_ROTATE_0 || labelRotation == TEXT_ROTATE_180)
        {
            labelWidth = fontToDraw->getStringWidth(text, wildcard);
        }
        else
        {
            // Get full height for all lines, except the last line for which we "replace" the part under
            // the baseline with the 'spacing above'.
            labelWidth = fontToDraw->getHeight() * fontToDraw->getNumberOfLines(text, wildcard) + fontToDraw->getSpacingAbove(text, wildcard);
        }
        const int16_t offset = (labelRotation == TEXT_ROTATE_0 || labelRotation == TEXT_ROTATE_90) ? (labelWidth / 2) : ((labelWidth + 1) / 2);
        Rect dirty((graph->getGraphAreaMarginLeft() + valueToScreenXQ5(graph, scaledIndex).round()) - offset, 0, labelWidth, getHeight());
        invalidateRect(dirty);
    }
}

void GraphLabelsX::drawIndexRange(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, const int rangeMin, const int rangeMax, const int minorInterval, const int majorInterval, const uint8_t a) const
{
    const int16_t gapIndex = graph->getGapBeforeIndex();
    if (gapIndex <= 0 || gapIndex <= rangeMin || gapIndex > rangeMax)
    {
        GraphLabelsBase::drawIndexRange(invalidatedArea, fontToDraw, graph, rangeMin, rangeMax, minorInterval, majorInterval, a);
    }
    else
    {
        GraphLabelsBase::drawIndexRange(invalidatedArea, fontToDraw, graph, rangeMin, (int)gapIndex - 1, minorInterval, majorInterval, a);
        GraphLabelsBase::drawIndexRange(invalidatedArea, fontToDraw, graph, (int)gapIndex, rangeMax, minorInterval, majorInterval, a);
    }
}

void GraphLabelsX::drawString(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, const int valueScaled, const int labelScaled, const uint8_t a) const
{
    const int16_t labelX = valueToScreenXQ5(graph, valueScaled).round() - graph->getGraphAreaPaddingLeft();
    if (labelX < 0 || labelX >= graph->getGraphAreaWidth())
    {
        return;
    }

    Unicode::UnicodeChar wildcard[20];
    formatLabel(wildcard, 20, getIndexToXAxis(graph, valueScaled, labelScaled), labelDecimals, labelDecimalPoint, graph->getScaleX());
    // Adjust to make label centered
    int16_t labelWidth;
    const Unicode::UnicodeChar* text = labelTypedText.getText();
    if (labelRotation == TEXT_ROTATE_0 || labelRotation == TEXT_ROTATE_180)
    {
        labelWidth = fontToDraw->getStringWidth(text, wildcard);
    }
    else
    {
        labelWidth = fontToDraw->getHeight() * fontToDraw->getNumberOfLines(text, wildcard) + fontToDraw->getSpacingAbove(text, wildcard);
    }
    const int16_t offset = (labelRotation == TEXT_ROTATE_0 || labelRotation == TEXT_ROTATE_90) ? (labelWidth / 2) : ((labelWidth + 1) / 2);
    Rect labelRect((graph->getGraphAreaMarginLeft() + valueToScreenXQ5(graph, valueScaled).round()) - offset, 0, labelWidth, getHeight());

    Rect dirty = labelRect & invalidatedArea;
    if (!dirty.isEmpty())
    {
        dirty.x -= labelRect.x;
        dirty.y -= labelRect.y;
        translateRectToAbsolute(labelRect);
        const LCD::StringVisuals visuals(fontToDraw, color, a, labelTypedText.getAlignment(), 0, labelRotation, labelTypedText.getTextDirection(), 0, WIDE_TEXT_NONE);
        HAL::lcd().drawString(labelRect, dirty, visuals, text, wildcard, 0);
    }
}

void GraphLabelsY::drawString(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, const int valueScaled, const int labelScaled, const uint8_t a) const
{
    const int16_t labelCoord = valueToScreenYQ5(graph, valueScaled).round() - graph->getGraphAreaPaddingTop();
    if (labelCoord < 0 || labelCoord >= graph->getGraphAreaHeight())
    {
        return;
    }

    Unicode::UnicodeChar wildcard[20];
    formatLabel(wildcard, 20, labelScaled, labelDecimals, labelDecimalPoint, dataScale);

    // Adjust to make label centered
    int16_t labelHeight;
    const Unicode::UnicodeChar* text = labelTypedText.getText();
    if (labelRotation == TEXT_ROTATE_0 || labelRotation == TEXT_ROTATE_180)
    {
        // Get full height for all lines, except the last line for which we "replace" the part under
        // the baseline with the 'spacing above'.
        labelHeight = fontToDraw->getHeight() * fontToDraw->getNumberOfLines(text, wildcard) + fontToDraw->getSpacingAbove(text, wildcard);
    }
    else
    {
        labelHeight = fontToDraw->getStringWidth(text, wildcard);
    }
    const int16_t offset = (labelRotation == TEXT_ROTATE_0 || labelRotation == TEXT_ROTATE_90) ? (labelHeight / 2) : ((labelHeight + 1) / 2);
    Rect labelRect(0, (graph->getGraphAreaMarginTop() + valueToScreenYQ5(graph, valueScaled).round()) - offset, getWidth(), labelHeight);

    Rect dirty = labelRect & invalidatedArea;
    if (!dirty.isEmpty())
    {
        dirty.x -= labelRect.x;
        dirty.y -= labelRect.y;
        translateRectToAbsolute(labelRect);
        const LCD::StringVisuals visuals(fontToDraw, color, a, labelTypedText.getAlignment(), 0, labelRotation, labelTypedText.getTextDirection(), 0, WIDE_TEXT_NONE);
        HAL::lcd().drawString(labelRect, dirty, visuals, text, wildcard, 0);
    }
}

void GraphTitle::draw(const Rect& invalidatedArea) const
{
    if (!titleTypedText.hasValidId())
    {
        return;
    }
    const Font* fontToDraw = titleTypedText.getFont();
    if (!fontToDraw)
    {
        return;
    }

    const uint8_t a = LCD::div255(getAlpha() * getGraph()->getAlpha());
    if (a == 0)
    {
        return;
    }

    const uint16_t lineHeight = fontToDraw->getHeight() * fontToDraw->getNumberOfLines(titleTypedText.getText());

    Rect labelRect(rect);
    // Adjust to make label centered
    if (titleRotation == TEXT_ROTATE_0 || titleRotation == TEXT_ROTATE_180)
    {
        labelRect.y += (labelRect.height - lineHeight) / 2;
        labelRect.height = lineHeight;
    }
    else
    {
        labelRect.x += (labelRect.width - lineHeight) / 2;
        labelRect.width = lineHeight;
    }

    Rect dirty = labelRect & invalidatedArea;
    if (!dirty.isEmpty())
    {
        dirty.x -= labelRect.x;
        dirty.y -= labelRect.y;
        translateRectToAbsolute(labelRect);
        const LCD::StringVisuals visuals(fontToDraw, getColor(), a, titleTypedText.getAlignment(), 0, titleRotation, titleTypedText.getTextDirection(), 0, WIDE_TEXT_NONE);
        HAL::lcd().drawString(labelRect, dirty, visuals, titleTypedText.getText(), 0, 0);
    }
}

} // namespace touchgfx
