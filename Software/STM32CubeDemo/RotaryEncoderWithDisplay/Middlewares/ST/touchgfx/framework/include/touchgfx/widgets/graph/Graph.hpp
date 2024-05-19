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
 * @file touchgfx/widgets/graph/Graph.hpp
 *
 * Declares the touchgfx::GraphData and touchgfx::Graph classes.
 */
#ifndef TOUCHGFX_GRAPH_HPP
#define TOUCHGFX_GRAPH_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>

namespace touchgfx
{
/**
 * The GraphData will show a graph of data points, drawn from lower x valut to highest x value.
 */
class GraphData : public StaticDataGraph
{
public:
    /**
     * Initializes a new instance of the GraphWrapAndOverwriteData class.
     *
     * @param      capacity The capacity.
     * @param [in] xvalues  Pointer to memory with room for capacity x values.
     * @param [in] yvalues  Pointer to memory with room for capacity y values.
     */
    GraphData(int16_t capacity, int* xvalues, int* yvalues)
        : StaticDataGraph(capacity, xvalues, yvalues)
    {
    }

protected:
    virtual int16_t addValue(int xvalue, int yvalue);
};

/**
 * The Graph will show a graph of data points, drawn from lower x valut to highest x value.
 *
 * @tparam CAPACITY The maximum number of data points on the graph.
 */
template <int16_t CAPACITY>
class Graph : public GraphData
{
public:
    Graph()
        : GraphData(CAPACITY, x, y)
    {
    }

private:
    int x[CAPACITY];
    int y[CAPACITY];
};

} // namespace touchgfx

#endif // TOUCHGFX_GRAPH_HPP
