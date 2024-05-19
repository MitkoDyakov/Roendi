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

/**
 * @file touchgfx/widgets/graph/GraphWrapAndOverwrite.hpp
 *
 * Declares the touchgfx::GraphWrapAndOverwriteData and touchgfx::GraphWrapAndOverwrite classes.
 */
#ifndef TOUCHGFX_GRAPHWRAPANDOVERWRITE_HPP
#define TOUCHGFX_GRAPHWRAPANDOVERWRITE_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>

namespace touchgfx
{
/**
 * A continuous data graph which will fill the graph with elements, and overwrite the first
 * elements with new values after the graph has filled. There will be a gap between the newly
 * inserted element and the element after. This similar behavior to a heart beat monitor.
 */
class GraphWrapAndOverwriteData : public DynamicDataGraph
{
public:
    /**
     * Initializes a new instance of the GraphWrapAndOverwriteData class.
     *
     * @param      capacity The capacity.
     * @param [in] values   Pointer to memory with room for capacity elements of type T.
     */
    GraphWrapAndOverwriteData(int16_t capacity, int* values)
        : DynamicDataGraph(capacity, values), current(0)
    {
    }

    virtual void clear();

    virtual int32_t indexToGlobalIndex(int16_t index) const;

protected:
    int16_t current; ///< The current index (used to keep track of where to insert new data point in value array)

    virtual void beforeAddValue();

    virtual int16_t addValue(int value);
};

/**
 * A Continuous graph. A quick way to create a GraphWrapAndOverwriteData.
 *
 * @tparam CAPACITY The maximum number of data points on the graph.
 */
template <int16_t CAPACITY>
class GraphWrapAndOverwrite : public GraphWrapAndOverwriteData
{
public:
    GraphWrapAndOverwrite()
        : GraphWrapAndOverwriteData(CAPACITY, y)
    {
    }

private:
    int y[CAPACITY];
};

} // namespace touchgfx

#endif // TOUCHGFX_GRAPHWRAPANDOVERWRITE_HPP
