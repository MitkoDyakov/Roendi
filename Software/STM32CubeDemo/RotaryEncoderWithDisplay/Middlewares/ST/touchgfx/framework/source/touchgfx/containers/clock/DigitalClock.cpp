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

#include <touchgfx/Unicode.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/containers/clock/DigitalClock.hpp>

namespace touchgfx
{
DigitalClock::DigitalClock()
    : AbstractClock(),
      displayMode(DISPLAY_24_HOUR),
      useLeadingZeroForHourIndicator(false),
      text()
{
    buffer[0] = '\0';
    text.setXY(0, 0);
    text.setWildcard(buffer);
    AbstractClock::add(text);
}

void DigitalClock::setWidth(int16_t width)
{
    AbstractClock::setWidth(width);
    text.setWidth(width);
}

void DigitalClock::setHeight(int16_t height)
{
    AbstractClock::setHeight(height);
    text.setHeight(height);
}

void DigitalClock::setBaselineY(int16_t baselineY)
{
    if (text.getTypedText().hasValidId())
    {
        moveTo(getX(), baselineY - text.getTypedText().getFont()->getBaseline());
    }
}

void DigitalClock::displayLeadingZeroForHourIndicator(bool displayLeadingZero)
{
    useLeadingZeroForHourIndicator = displayLeadingZero;
}

void DigitalClock::setAlpha(uint8_t newAlpha)
{
    text.setAlpha(newAlpha);
}

uint8_t DigitalClock::getAlpha() const
{
    return text.getAlpha();
}

void DigitalClock::setTypedText(TypedText typedText)
{
    // Do invalidateContent before and after in case the size of the text changes
    text.invalidateContent();
    text.setTypedText(typedText);
    text.invalidateContent();
}

void DigitalClock::setColor(colortype color)
{
    // Do invlidateContent only once since the size does not change
    text.setColor(color);
    text.invalidateContent();
}

colortype DigitalClock::getColor() const
{
    return text.getColor();
}

void DigitalClock::updateClock()
{
    text.invalidateContent();
    if (displayMode == DISPLAY_12_HOUR_NO_SECONDS)
    {
        const char* format = useLeadingZeroForHourIndicator ? "%02d:%02d %cM" : "%d:%02d %cM";
        Unicode::snprintf(buffer, BUFFER_SIZE, format, getCurrentHour12(), getCurrentMinute(), getCurrentAM() ? 'A' : 'P');
    }
    else if (displayMode == DISPLAY_24_HOUR_NO_SECONDS)
    {
        const char* format = useLeadingZeroForHourIndicator ? "%02d:%02d" : "%d:%02d";
        Unicode::snprintf(buffer, BUFFER_SIZE, format, getCurrentHour24(), getCurrentMinute());
    }
    else if (displayMode == DISPLAY_12_HOUR)
    {
        const char* format = useLeadingZeroForHourIndicator ? "%02d:%02d:%02d %cM" : "%d:%02d:%02d %cM";
        Unicode::snprintf(buffer, BUFFER_SIZE, format, getCurrentHour12(), getCurrentMinute(), getCurrentSecond(), getCurrentAM() ? 'A' : 'P');
    }
    else if (displayMode == DISPLAY_24_HOUR)
    {
        const char* format = useLeadingZeroForHourIndicator ? "%02d:%02d:%02d" : "%d:%02d:%02d";
        Unicode::snprintf(buffer, BUFFER_SIZE, format, getCurrentHour24(), getCurrentMinute(), getCurrentSecond());
    }
    text.invalidateContent();
}
} // namespace touchgfx
