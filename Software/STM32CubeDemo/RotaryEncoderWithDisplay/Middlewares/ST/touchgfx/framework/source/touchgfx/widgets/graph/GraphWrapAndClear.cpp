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

#include <touchgfx/widgets/graph/GraphWrapAndClear.hpp>

namespace touchgfx
{

void GraphWrapAndClearData::clear()
{
    invalidateAllXAxisPoints();
    DynamicDataGraph::clear();
}

void GraphWrapAndClearData::beforeAddValue()
{
    if (usedCapacity >= maxCapacity)
    {
        clear();
    }
}

int16_t GraphWrapAndClearData::addValue(int value)
{
    const bool clearGraph = (usedCapacity == 0);
    const int16_t index = usedCapacity;
    usedCapacity++;
    yValues[dataIndex(index)] = value;
    if (clearGraph)
    {
        // Label sizes might have grown, also invalidate new sizes
        invalidateAllXAxisPoints();
    }
    invalidateGraphPointAt(index);
    return index;
}

} // namespace touchgfx
