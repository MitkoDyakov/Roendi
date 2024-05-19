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

#include <touchgfx/widgets/graph/GraphScroll.hpp>

namespace touchgfx
{

void GraphScrollData::clear()
{
    DynamicDataGraph::clear();
    current = 0;
}

int32_t GraphScrollData::indexToGlobalIndex(int16_t index) const
{
    if (usedCapacity < maxCapacity)
    {
        return dataIndex(index);
    }
    return (dataCounter - maxCapacity) + index;
}

void GraphScrollData::beforeAddValue()
{
    if (usedCapacity == maxCapacity)
    {
        invalidateAllXAxisPoints();
    }
}

int16_t GraphScrollData::addValue(int value)
{
    const bool graphFull = usedCapacity == maxCapacity;
    const int16_t index = current++;
    current %= maxCapacity;
    if (index == usedCapacity)
    {
        usedCapacity++;
    }
    yValues[index] = value;
    if (graphFull)
    {
        invalidateGraphArea();
        invalidateAllXAxisPoints();
    }
    else
    {
        invalidateGraphPointAt(index);
    }
    return index;
}

} // namespace touchgfx
