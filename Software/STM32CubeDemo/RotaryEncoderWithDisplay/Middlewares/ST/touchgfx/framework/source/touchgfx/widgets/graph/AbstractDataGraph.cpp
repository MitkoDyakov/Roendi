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

#include <touchgfx/Callback.hpp>
#include <touchgfx/Drawable.hpp>
#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>
#include <touchgfx/widgets/graph/GraphElements.hpp>
#include <touchgfx/widgets/graph/GraphLabels.hpp>

namespace touchgfx
{
AbstractDataGraph::AbstractDataGraph(int16_t capacity)
    : Container(),
      alpha(255), xScale(1), yScale(1),
      graphArea(), leftArea(), rightArea(), topArea(), bottomArea(),
      topPadding(0), leftPadding(0), rightPadding(0), bottomPadding(0),
      maxCapacity(capacity), usedCapacity(0), gapBeforeIndex(0), clickAction(0), dragAction(0),
      graphRangeMinX(0), graphRangeMaxX(0), graphRangeMinY(0), graphRangeMaxY(0)
{
    assert(capacity > 0);
    AbstractDataGraph::add(graphArea);
    AbstractDataGraph::add(topArea);
    AbstractDataGraph::add(leftArea);
    AbstractDataGraph::add(rightArea);
    AbstractDataGraph::add(bottomArea);
    // Place all areas properly:
    setGraphAreaMargin(0, 0, 0, 0);
    setTouchable(true);
}

void AbstractDataGraph::setWidth(int16_t width)
{
    topArea.setWidth(width);
    bottomArea.setWidth(width);
    rightArea.setX(width - rightArea.getWidth());
    graphArea.setWidth(width - (leftArea.getWidth() + rightArea.getWidth()));
    Container::setWidth(width);
    updateAreasPosition();
}

void AbstractDataGraph::setHeight(int16_t height)
{
    leftArea.setHeight(height);
    rightArea.setHeight(height);
    bottomArea.setY(height - bottomArea.getHeight());
    graphArea.setHeight(height - (topArea.getHeight() + bottomArea.getHeight()));
    Container::setHeight(height);
    updateAreasPosition();
}

void AbstractDataGraph::setGraphAreaMargin(int16_t top, int16_t left, int16_t right, int16_t bottom)
{
    graphArea.setPosition(left, top, getWidth() - (left + right), getHeight() - (top + bottom));
    topArea.setPosition(0, 0, getWidth(), top);
    leftArea.setPosition(0, 0, left, getHeight());
    rightArea.setPosition(getWidth() - right, 0, right, getHeight());
    bottomArea.setPosition(0, getHeight() - bottom, getWidth(), bottom);
    updateAreasPosition();
}

void AbstractDataGraph::setGraphAreaPadding(int16_t top, int16_t left, int16_t right, int16_t bottom)
{
    topPadding = top;
    leftPadding = left;
    rightPadding = right;
    bottomPadding = bottom;
}

void AbstractDataGraph::clear()
{
    usedCapacity = 0;
    invalidateGraphArea();
}

void AbstractDataGraph::addGraphElement(AbstractGraphElement& d)
{
    graphArea.add(d);
    d.setPosition(0, 0, graphArea.getWidth(), graphArea.getHeight());
}

void AbstractDataGraph::addTopElement(AbstractGraphDecoration& d)
{
    topArea.add(d);
    d.setPosition(0, 0, topArea.getWidth(), topArea.getHeight());
}

void AbstractDataGraph::addLeftElement(AbstractGraphDecoration& d)
{
    leftArea.add(d);
    d.setPosition(0, 0, leftArea.getWidth(), leftArea.getHeight());
}

void AbstractDataGraph::addRightElement(AbstractGraphDecoration& d)
{
    rightArea.add(d);
    d.setPosition(0, 0, rightArea.getWidth(), rightArea.getHeight());
}

void AbstractDataGraph::addBottomElement(AbstractGraphDecoration& d)
{
    bottomArea.add(d);
    d.setPosition(0, 0, bottomArea.getWidth(), bottomArea.getHeight());
}

bool AbstractDataGraph::getNearestIndexForScreenXY(int16_t x, int16_t y, int16_t& index)
{
    if (usedCapacity == 0)
    {
        return false;
    }
    uint32_t bestDist = 0xFFFFFFFF;
    for (int16_t ix = 0; ix < usedCapacity; ix++)
    {
        const int16_t xDist = indexToScreenX(ix) - x;
        const int16_t yDist = indexToScreenY(ix) - y;
        const uint32_t dist = xDist * xDist + yDist * yDist;
        if (dist < bestDist)
        {
            index = ix;
            bestDist = dist;
        }
    }
    return true;
}

bool AbstractDataGraph::getNearestIndexForScreenX(int16_t x, int16_t& index) const
{
    index = 0;
    if (usedCapacity == 0)
    {
        return false;
    }
    uint32_t bestDist = abs(indexToScreenX(index) - x);
    for (int16_t ix = 1; ix < usedCapacity; ix++)
    {
        const uint32_t dist = abs(indexToScreenX(ix) - x);
        if (dist < bestDist)
        {
            index = ix;
            bestDist = dist;
        }
    }
    return true;
}

void AbstractDataGraph::handleClickEvent(const ClickEvent& event)
{
    if (event.getType() == ClickEvent::CANCEL)
    {
        return;
    }
    const int16_t x = event.getX() - graphArea.getX();
    int16_t index;
    if (getNearestIndexForScreenX(x, index))
    {
        if (event.getType() == ClickEvent::PRESSED || event.getType() == ClickEvent::RELEASED)
        {
            if (clickAction && clickAction->isValid())
            {
                const GraphClickEvent graphClickEvent(index, event);
                clickAction->execute(*this, graphClickEvent);
            }
        }
    }
}

void AbstractDataGraph::handleDragEvent(const DragEvent& event)
{
    const int16_t x = event.getNewX() - graphArea.getX();
    int16_t index;
    if (getNearestIndexForScreenX(x, index))
    {
        if (event.getType() == DragEvent::DRAGGED)
        {
            if (dragAction && dragAction->isValid())
            {
                const GraphDragEvent graphDragEvent(index, event);
                dragAction->execute(*this, graphDragEvent);
            }
        }
    }
}

void AbstractDataGraph::invalidateGraphPointAt(int16_t index)
{
    if (index < usedCapacity)
    {
        AbstractGraphElement* d = (AbstractGraphElement*)graphArea.getFirstChild();
        while (d)
        {
            d->invalidateGraphPointAt(index);
            d = (AbstractGraphElement*)d->getNextSibling();
        }
    }
}

void AbstractDataGraph::invalidateXAxisPointAt(int16_t index)
{
    AbstractGraphElement* d = (AbstractGraphElement*)topArea.getFirstChild();
    while (d)
    {
        d->invalidateGraphPointAt(index);
        d = (AbstractGraphElement*)d->getNextSibling();
    }
    d = (AbstractGraphElement*)bottomArea.getFirstChild();
    while (d)
    {
        d->invalidateGraphPointAt(index);
        d = (AbstractGraphElement*)d->getNextSibling();
    }
}

void AbstractDataGraph::invalidateAllXAxisPoints()
{
    for (int index = 0; index <= usedCapacity; index++)
    {
        invalidateXAxisPointAt(index);
    }
    topArea.invalidate();
    bottomArea.invalidate();
}

void AbstractDataGraph::updateAreasPosition()
{
    Drawable* d = graphArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, graphArea.getWidth(), graphArea.getHeight());
        d = d->getNextSibling();
    }

    d = topArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, topArea.getWidth(), topArea.getHeight());
        d = d->getNextSibling();
    }

    d = leftArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, leftArea.getWidth(), leftArea.getHeight());
        d = d->getNextSibling();
    }

    d = rightArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, rightArea.getWidth(), rightArea.getHeight());
        d = d->getNextSibling();
    }

    d = bottomArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, bottomArea.getWidth(), bottomArea.getHeight());
        d = d->getNextSibling();
    }

    invalidate();
}

void AbstractDataGraph::setGraphRangeXScaled(int min, int max)
{
    assert(min != max);
    if (max < min)
    {
        const int tmp = min;
        min = max;
        max = tmp;
    }
    if (min != graphRangeMinX || max != graphRangeMaxX)
    {
        graphRangeMinX = min;
        graphRangeMaxX = max;
        topArea.invalidate();
        graphArea.invalidate();
        bottomArea.invalidate();
    }
}

void AbstractDataGraph::setGraphRangeYScaled(int min, int max)
{
    assert(min != max);
    if (max < min)
    {
        const int tmp = min;
        min = max;
        max = tmp;
    }
    if (min != graphRangeMinY || max != graphRangeMaxY)
    {
        graphRangeMinY = min;
        graphRangeMaxY = max;
        leftArea.invalidate();
        graphArea.invalidate();
        rightArea.invalidate();
    }
}

int AbstractDataGraph::convertToNewScale(int value, int oldScale, int newScale)
{
    if (oldScale == newScale)
    {
        return value;
    }
    return muldiv(value, newScale, oldScale);
}

int16_t DynamicDataGraph::addDataPointScaled(int y)
{
    beforeAddValue();
    dataCounter++;
    return addValue(y);
}

void DynamicDataGraph::setGraphRangeYAutoScaled(bool showXaxis, int margin)
{
    if (usedCapacity == 0)
    {
        return;
    }
    int16_t indexMin = (int16_t)getGraphRangeXMin();
    int16_t indexMax = (int16_t)getGraphRangeXMax();
    if (indexMin > indexMax)
    {
        const int tmp = indexMin;
        indexMin = indexMax;
        indexMax = tmp;
    }
    indexMin = MAX(indexMin, 0);
    indexMax = MIN(indexMax, usedCapacity);
    if (indexMin < usedCapacity && indexMax >= 0)
    {
        int yMin = showXaxis ? margin : yValues[indexMin];
        int yMax = showXaxis ? -margin : yValues[indexMin];
        for (int i = indexMin; i < indexMax; i++)
        {
            const int y = yValues[i];
            if (yMin > y)
            {
                yMin = y;
            }
            if (yMax < y)
            {
                yMax = y;
            }
        }
        yMin -= margin;
        yMax += margin;
        if (yMin != yMax)
        {
            setGraphRangeYScaled(yMin, yMax);
        }
    }
}

bool DynamicDataGraph::xScreenRangeToIndexRange(int16_t xMin, int16_t xMax, int16_t& indexMin, int16_t& indexMax) const
{
    if (usedCapacity == 0)
    {
        indexMin = indexMax = -1;
        return false;
    }
    if (getGraphAreaWidth() <= 1)
    {
        indexMin = 0;
        indexMax = usedCapacity - 1;
        return true;
    }
    CWRUtil::Q5 xQ5Min = CWRUtil::muldivQ5(CWRUtil::toQ5(xMin - leftPadding), CWRUtil::Q5(scaled2intX(graphRangeMaxX - graphRangeMinX)), CWRUtil::Q5(getGraphAreaWidth() - 1)) + CWRUtil::toQ5(scaled2intX(graphRangeMinX));
    CWRUtil::Q5 xQ5Max = CWRUtil::muldivQ5(CWRUtil::toQ5(xMax - leftPadding), CWRUtil::Q5(scaled2intX(graphRangeMaxX - graphRangeMinX)), CWRUtil::Q5(getGraphAreaWidth() - 1)) + CWRUtil::toQ5(scaled2intX(graphRangeMinX));
    if (xQ5Min > xQ5Max)
    {
        const CWRUtil::Q5 tmp = xQ5Min;
        xQ5Min = xQ5Max;
        xQ5Max = tmp;
    }
    indexMin = xQ5Min.to<int16_t>(); // X is also index in AbstractDataGraphWithY
    indexMax = xQ5Max.ceil();
    if (indexMax < 0)
    {
        indexMin = indexMax = 0;
        return false;
    }
    if (indexMin > usedCapacity - 1)
    {
        indexMin = indexMax = usedCapacity - 1;
        return false;
    }
    indexMin = MAX(0, indexMin);
    indexMax = MIN(usedCapacity - 1, indexMax);
    return true;
}

void DynamicDataGraph::setScaleX(int scale, bool updateData /*= false*/)
{
    const int oldScale = getScaleX();
    if (scale != oldScale)
    {
        setGraphRangeXScaled(convertToNewScale(graphRangeMinX, oldScale, scale), convertToNewScale(graphRangeMaxX, oldScale, scale));
        xAxisFactor = convertToNewScale(xAxisFactor, oldScale, scale);
        xAxisOffset = convertToNewScale(xAxisOffset, oldScale, scale);
        AbstractDataGraph::setScaleX(scale, updateData);
    }
}

void DynamicDataGraph::setScaleY(int scale, bool updateData /*= false*/)
{
    if (updateData)
    {
        const int oldScale = getScaleY();
        for (int16_t index = 0; index < usedCapacity; index++)
        {
            yValues[index] = convertToNewScale(yValues[index], oldScale, scale);
        }
    }
    AbstractDataGraph::setScaleY(scale);
}

void StaticDataGraph::setScaleX(int scale, bool updateData /*= false*/)
{
    if (updateData)
    {
        const int oldScale = getScaleX();
        for (int16_t index = 0; index < usedCapacity; index++)
        {
            xValues[index] = convertToNewScale(xValues[index], oldScale, scale);
        }
    }
    AbstractDataGraph::setScaleX(scale);
}

void StaticDataGraph::setScaleY(int scale, bool updateData /*= false*/)
{
    if (updateData)
    {
        const int oldScale = getScaleY();
        for (int16_t index = 0; index < usedCapacity; index++)
        {
            yValues[index] = convertToNewScale(yValues[index], oldScale, scale);
        }
    }
    AbstractDataGraph::setScaleY(scale);
}

void StaticDataGraph::setGraphRangeXAutoScaled(bool showYaxis, int margin)
{
    if (usedCapacity == 0)
    {
        return;
    }
    int xMin = xValues[0];                // First x value is the lowest
    int xMax = xValues[usedCapacity - 1]; // Last x value is the highest
    if (showYaxis)
    {
        // The y axis (x=0) must be shown, so make sure 0 is included after subtracting/adding the margin
        xMin = MIN(xMin, margin);
        xMax = MAX(xMax, -margin);
    }
    xMin -= margin;
    xMax += margin;
    if (xMin != xMax)
    {
        setGraphRangeXScaled(xMin, xMax);
    }
}

void StaticDataGraph::setGraphRangeYAutoScaled(bool showXaxis /*= true*/, int margin /*= 0*/)
{
    if (usedCapacity == 0)
    {
        return;
    }
    int xValueMin = getGraphRangeXMinScaled(); // Get shown x range
    int xValueMax = getGraphRangeXMaxScaled();
    if (xValueMin > xValueMax)
    {
        // Swap so min<=max.
        const int16_t tmp = xValueMin;
        xValueMin = xValueMax;
        xValueMax = tmp;
    }
    int index = 0;
    while (xValues[index] < xValueMin && index < usedCapacity)
    {
        // Skip all indices that have x outside visible area
        index++;
    }
    if (xValues[index] > xValueMax)
    {
        // No x values are in the visible area
        return;
    }
    int yMin = yValues[index++]; // The index'th value must be visible
    int yMax = yMin;
    // Go through all points where x is in the visible range
    while (xValues[index] < xValueMax && index < usedCapacity)
    {
        const int y = yValues[index++];
        yMin = MIN(yMin, y);
        yMax = MAX(yMax, y);
    }
    if (showXaxis)
    {
        // The x axis (y=0) must be shown, so make sure 0 is included after subtracting/adding the margin
        yMin = MIN(yMin, margin);
        yMax = MAX(yMax, -margin);
    }
    yMin -= margin;
    yMax += margin;
    if (yMin != yMax)
    {
        setGraphRangeYScaled(yMin, yMax);
    }
}

int16_t StaticDataGraph::deleteValue(int xvalue)
{
    for (int index = 0; index < usedCapacity; index++)
    {
        if (xValues[index] == xvalue)
        {
            return deleteIndex(index);
        }
    }
    return -1;
}

int16_t StaticDataGraph::deleteIndex(int index)
{
    assert(index >= 0 && index < usedCapacity);
    invalidateGraphPointAt(index);
    usedCapacity--;
    for (int i = index; i < usedCapacity; i++)
    {
        xValues[i] = xValues[i + 1];
        yValues[i] = yValues[i + 1];
    }
    return index;
}

bool StaticDataGraph::xScreenRangeToIndexRange(int16_t xMin, int16_t xMax, int16_t& indexMin, int16_t& indexMax) const
{
    if (usedCapacity == 0)
    {
        indexMin = indexMax = -1;
        return false;
    }
    if (getGraphAreaWidth() <= 1)
    {
        indexMin = 0;
        indexMax = usedCapacity - 1;
        return true;
    }
    CWRUtil::Q5 xQ5Min = CWRUtil::muldivQ5(CWRUtil::toQ5(xMin - leftPadding), CWRUtil::Q5(graphRangeMaxX - graphRangeMinX), CWRUtil::Q5(getGraphAreaWidth() - 1)) + CWRUtil::toQ5(graphRangeMinX);
    CWRUtil::Q5 xQ5Max = CWRUtil::muldivQ5(CWRUtil::toQ5(xMax - leftPadding), CWRUtil::Q5(graphRangeMaxX - graphRangeMinX), CWRUtil::Q5(getGraphAreaWidth() - 1)) + CWRUtil::toQ5(graphRangeMinX);
    if (xQ5Min > xQ5Max)
    {
        const CWRUtil::Q5 tmp = xQ5Min;
        xQ5Min = xQ5Max;
        xQ5Max = tmp;
    }
    const int xValueMin = xQ5Min.to<int>(); // These are REAL x values
    const int xValueMax = xQ5Max.ceil();
    indexMin = -1;
    while (indexMin < usedCapacity - 1 && xValues[indexMin + 1] <= xValueMin)
    {
        indexMin++;
    }
    indexMax = usedCapacity;
    while (indexMax > 0 && xValues[indexMax - 1] >= xValueMax)
    {
        indexMax--;
    }
    if (indexMax <= 0)
    {
        indexMin = indexMax = 0;
        return false;
    }
    if (indexMin >= usedCapacity - 1)
    {
        indexMin = indexMax = usedCapacity - 1;
        return false;
    }
    indexMin = MAX(0, indexMin);
    indexMax = MIN(usedCapacity - 1, indexMax);
    return true;
}

} // namespace touchgfx
