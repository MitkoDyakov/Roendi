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
#include <touchgfx/Utils.hpp>
#include <touchgfx/containers/Slider.hpp>

namespace touchgfx
{
Slider::Slider()
    : Container(),
      sliderOrientation(HORIZONTAL),
      currentValue(0),
      valueRangeMin(0),
      valueRangeMax(100),
      background(),
      backgroundSelected(),
      indicator(),
      backgroundSelectedViewPort(),
      indicatorMinPosition(0),
      indicatorMaxPosition(1),
      startValueCallback(0),
      stopValueCallback(0),
      newValueCallback(0)
{
    setTouchable(true);

    // The backgroundSelectedViewPort is a container into which the bitmap for the "filled" background
    // is placed. Containers are viewports, so the dimensions of this container controls how
    // much of the filled background is visible.
    backgroundSelectedViewPort.add(backgroundSelected);

    Container::add(background);
    Container::add(backgroundSelectedViewPort);
    Container::add(indicator);

    // Default value range
    Slider::updateIndicatorPosition(indicatorMinPosition);
}

void Slider::setBitmaps(const Bitmap& sliderBackground, const Bitmap& sliderBackgroundSelected, const Bitmap& indicatorBitmap)
{
    assert(sliderBackground.getWidth() == sliderBackgroundSelected.getWidth() &&
           sliderBackground.getHeight() == sliderBackgroundSelected.getHeight() &&
           "Slider::setBitmaps - background and backgroundFilled must have same dimensions");

    // Invalidate before and after in case size changes from previous bitmaps
    background.invalidate(); // Redraw slider
    indicator.invalidate();  // Redraw indicator

    background.setBitmap(sliderBackground);
    backgroundSelected.setBitmap(sliderBackgroundSelected);
    indicator.setBitmap(indicatorBitmap);
    backgroundSelectedViewPort.setWidthHeight(backgroundSelected);

    setValue(currentValue); // Update size of backgroundSelectedViewPort

    background.invalidate(); // Redraw slider
    indicator.invalidate();  // Redraw indicator
}

void Slider::setBitmaps(const BitmapId sliderBackground, const BitmapId sliderBackgroundSelected, const BitmapId indicatorBitmap)
{
    setBitmaps(Bitmap(sliderBackground), Bitmap(sliderBackgroundSelected), Bitmap(indicatorBitmap));
}

void Slider::setupHorizontalSlider(int16_t backgroundX, int16_t backgroundY, int16_t indicatorY, int16_t indicatorMinX, int16_t indicatorMaxX)
{
    assert(indicatorMinX < indicatorMaxX && "Slider::setupHorizontalSlider - indicatorMinX must be smaller than indicatorMaxX");

    sliderOrientation = HORIZONTAL;

    background.setXY(backgroundX, backgroundY);
    backgroundSelectedViewPort.setPosition(background);
    backgroundSelected.setXY(0, 0);
    indicator.setY(indicatorY);

    const int16_t backgroundWidth = backgroundX + background.getWidth();
    const int16_t indicatorWidth = indicatorMaxX + indicator.getWidth();
    const int16_t newWidth = MAX(backgroundWidth, indicatorWidth);

    const int16_t backgroundHeight = backgroundY + background.getHeight();
    const int16_t indicatorHeight = indicatorY + indicator.getHeight();
    const int16_t newHeight = MAX(backgroundHeight, indicatorHeight);

    indicatorMinPosition = indicatorMinX;
    indicatorMaxPosition = indicatorMaxX;

    setWidthHeight(newWidth, newHeight);

    setValue(currentValue);
}

void Slider::setupVerticalSlider(int16_t backgroundX, int16_t backgroundY, int16_t indicatorX, int16_t indicatorMinY, int16_t indicatorMaxY)
{
    assert(indicatorMinY < indicatorMaxY && "Slider::setupVerticalSlider - indicatorMinY must be smaller than indicatorMaxY");

    sliderOrientation = VERTICAL;

    background.setXY(backgroundX, backgroundY);
    backgroundSelectedViewPort.setPosition(background);
    indicator.setX(indicatorX);

    const int16_t backgroundWidth = backgroundX + background.getWidth();
    const int16_t indicatorWidth = indicatorX + indicator.getWidth();
    const int16_t newWidth = MAX(backgroundWidth, indicatorWidth);

    const int16_t backgroundHeight = backgroundY + background.getHeight();
    const int16_t indicatorHeight = indicatorMaxY + indicator.getHeight();
    const int16_t newHeight = MAX(backgroundHeight, indicatorHeight);

    indicatorMinPosition = indicatorMinY;
    indicatorMaxPosition = indicatorMaxY;

    setWidthHeight(newWidth, newHeight);

    setValue(currentValue);
}

void Slider::setValue(int16_t value)
{
    updateIndicatorPosition(valueToPosition(value));
}

void Slider::handleClickEvent(const ClickEvent& event)
{
    if ((event.getType() == ClickEvent::PRESSED) || (event.getType() == ClickEvent::RELEASED))
    {
        // Communicate the start value if a listener is registered
        if ((event.getType() == ClickEvent::PRESSED) && (startValueCallback != 0) && startValueCallback->isValid())
        {
            startValueCallback->execute(*this, currentValue);
        }

        updateIndicatorPosition((sliderOrientation == HORIZONTAL ? event.getX() : event.getY()) - getIndicatorRadius());

        // Communicate the stop value if a listener is registered
        if ((event.getType() == ClickEvent::RELEASED) && (stopValueCallback != 0) && stopValueCallback->isValid())
        {
            stopValueCallback->execute(*this, currentValue);
        }
    }
}

void Slider::handleDragEvent(const DragEvent& event)
{
    updateIndicatorPosition((sliderOrientation == HORIZONTAL ? event.getNewX() : event.getNewY()) - getIndicatorRadius());
}

int16_t Slider::valueToPosition(int value) const
{
    value = MIN(valueRangeMax, value);
    value = MAX(value, valueRangeMin);
    const int16_t relativePosition = muldiv(value - valueRangeMin, getIndicatorPositionRangeSize(), getValueRangeSize());
    return sliderOrientation == VERTICAL ? indicatorMaxPosition - relativePosition : indicatorMinPosition + relativePosition;
}

int Slider::positionToValue(int16_t position) const
{
    position = MIN(indicatorMaxPosition, position);
    position = MAX(position, indicatorMinPosition);
    const int16_t relativePosition = sliderOrientation == VERTICAL ? indicatorMaxPosition - position : position - indicatorMinPosition;
    return valueRangeMin + muldiv(relativePosition, getValueRangeSize(), getIndicatorPositionRangeSize());
}

void Slider::updateIndicatorPosition(int16_t position)
{
    // Cut off positions outside the slider area
    position = MAX(position, indicatorMinPosition);
    position = MIN(position, indicatorMaxPosition);

    if (sliderOrientation == HORIZONTAL)
    {
        indicator.moveTo(position, indicator.getY());

        backgroundSelectedViewPort.invalidate();
        backgroundSelectedViewPort.setWidth((position - backgroundSelectedViewPort.getX()) + getIndicatorRadius());
        backgroundSelectedViewPort.invalidate();
    }
    else
    {
        indicator.moveTo(indicator.getX(), position);

        backgroundSelectedViewPort.invalidate();
        const int16_t newViewPortHeight = background.getRect().bottom() - (position + getIndicatorRadius());
        backgroundSelectedViewPort.setPosition(backgroundSelectedViewPort.getX(), position + getIndicatorRadius(), backgroundSelectedViewPort.getWidth(), newViewPortHeight);
        backgroundSelected.setY(-(backgroundSelected.getHeight() - newViewPortHeight));
        backgroundSelectedViewPort.invalidate();
    }

    currentValue = positionToValue(position);

    // Communicate the new value if a listener is registered
    if ((newValueCallback != 0) && newValueCallback->isValid())
    {
        newValueCallback->execute(*this, currentValue);
    }
}

int16_t Slider::getIndicatorRadius() const
{
    return (sliderOrientation == HORIZONTAL ? indicator.getWidth() : indicator.getHeight()) / 2;
}

void Slider::setValueRange(int16_t minValue, int16_t maxValue, int16_t newValue)
{
    assert(minValue < maxValue && "Slider::setValueRange - minValue must be smaller than maxValue");

    valueRangeMin = minValue;
    valueRangeMax = maxValue;

    setValue(newValue);
}

void Slider::setValueRange(int16_t minValue, int16_t maxValue)
{
    const int16_t newValue = currentValue < minValue ? minValue : (currentValue > maxValue ? maxValue : currentValue);
    setValueRange(minValue, maxValue, newValue);
}

int Slider::getIndicatorPositionRangeSize() const
{
    return indicatorMaxPosition - indicatorMinPosition;
}

int Slider::getValueRangeSize() const
{
    return valueRangeMax - valueRangeMin;
}
} // namespace touchgfx
