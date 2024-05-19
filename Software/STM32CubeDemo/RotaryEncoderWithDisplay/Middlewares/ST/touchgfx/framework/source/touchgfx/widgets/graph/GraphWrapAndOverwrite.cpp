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

#include <touchgfx/widgets/graph/GraphWrapAndOverwrite.hpp>

namespace touchgfx
{

void GraphWrapAndOverwriteData::clear()
{
    DynamicDataGraph::clear();
    current = 0;
}

int32_t GraphWrapAndOverwriteData::indexToGlobalIndex(int16_t index) const
{
    if (this->usedCapacity < this->maxCapacity)
    {
        return index;
    }
    const int16_t gapIndex = this->getGapBeforeIndex();
    if (index < gapIndex)
    {
        return (this->dataCounter - gapIndex) + index;
    }
    return ((this->dataCounter - gapIndex) - this->maxCapacity) + index;
}

void GraphWrapAndOverwriteData::beforeAddValue()
{
    if (current == 0 && usedCapacity >= maxCapacity)
    {
        const int xMin = getGraphRangeXMin();
        const int xMax = getGraphRangeXMax();
        for (int i = xMin; i < 0; i++)
        {
            invalidateXAxisPointAt(i);
        }
        for (int i = maxCapacity; i <= xMax; i++)
        {
            invalidateXAxisPointAt(i);
        }
    }
    if (usedCapacity >= maxCapacity)
    {
        invalidateGraphPointAt(current);
        invalidateXAxisPointAt(current);
    }
}

int16_t GraphWrapAndOverwriteData::addValue(int value)
{
    const int16_t index = current++;
    current %= maxCapacity;
    if (index == usedCapacity)
    {
        usedCapacity++;
    }
    yValues[dataIndex(index)] = value;
    setGapBeforeIndex(index + 1);
    invalidateGraphPointAt(index);
    if (usedCapacity >= maxCapacity)
    {
        invalidateXAxisPointAt(index);
    }
    if (index == 0 && usedCapacity >= maxCapacity)
    {
        const int xMin = getGraphRangeXMin();
        const int xMax = getGraphRangeXMax();
        for (int i = xMin; i < 0; i++)
        {
            invalidateXAxisPointAt(i);
        }
        for (int i = maxCapacity; i <= xMax; i++)
        {
            invalidateXAxisPointAt(i);
        }
    }
    return index;
}

} // namespace touchgfx
