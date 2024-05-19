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

#include <touchgfx/containers/progress_indicators/TextProgress.hpp>

namespace touchgfx
{
TextProgress::TextProgress()
    : AbstractProgressIndicator(),
      textArea(),
      decimals(0)
{
    textBuffer[0] = 0;
    progressIndicatorContainer.add(textArea);
}

void TextProgress::setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    textArea.setPosition(0, 0, width, height);

    AbstractProgressIndicator::setProgressIndicatorPosition(x, y, width, height);
}

void TextProgress::setTypedText(const TypedText& t)
{
    textArea.setTypedText(t);
}

const TypedText& TextProgress::getTypedText() const
{
    return textArea.getTypedText();
}

void TextProgress::setColor(colortype color)
{
    textArea.setColor(color);
}

colortype TextProgress::getColor() const
{
    return textArea.getColor();
}

void TextProgress::setAlpha(uint8_t newAlpha)
{
    AbstractProgressIndicator::setAlpha(newAlpha);
    textArea.setAlpha(newAlpha);
}

void TextProgress::setValue(int value)
{
    textArea.invalidateContent();
    AbstractProgressIndicator::setValue(value);
    const int range[3] = { 1, 10, 100 };
    const uint16_t progress = AbstractProgressIndicator::getProgress(100 * range[decimals]);
    if (decimals > 0)
    {
        Unicode::snprintf(textBuffer, 8, "%d.%0*d", progress / range[decimals], decimals, progress % range[decimals]);
    }
    else
    {
        Unicode::snprintf(textBuffer, 8, "%d", progress);
    }
    textArea.setWildcard(textBuffer);
    textArea.invalidateContent();
}

void TextProgress::setNumberOfDecimals(uint16_t numberOfDecimals)
{
    decimals = MIN(2, numberOfDecimals);
    setValue(getValue());
}

uint16_t TextProgress::getNumberOfDecimals() const
{
    return decimals;
}
} // namespace touchgfx
