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

#include <touchgfx/widgets/graph/Graph.hpp>

namespace touchgfx
{

int16_t GraphData::addValue(int xvalue, int yvalue)
{
    int16_t index = 0;
    if (usedCapacity == 0 || xValues[usedCapacity - 1] < xvalue)
    {
        index = usedCapacity;
    }
    else
    {
        while (index < usedCapacity && xValues[index] < xvalue)
        {
            index++;
        }
        if (index < usedCapacity && xValues[index] == xvalue) // Pointing to an element to replace
        {
            // Replace point
            invalidateGraphPointAt(index);
            yValues[index] = yvalue;
            invalidateGraphPointAt(index);
            return index;
        }
    }
    if ((index == usedCapacity || xValues[index] > xvalue) && usedCapacity == maxCapacity)
    {
        // Should add at end OR insert in the middle, but no room for more data
        return -1;
    }
    // Insert at 'index', move following points up the array
    for (int16_t i = usedCapacity; i > index; i--)
    {
        xValues[i] = xValues[i - 1];
        yValues[i] = yValues[i - 1];
    }
    usedCapacity++;
    xValues[index] = xvalue;
    yValues[index] = yvalue;
    invalidateGraphPointAt(index);
    return index;
}

} // namespace touchgfx
