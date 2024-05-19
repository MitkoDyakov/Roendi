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
 * @file touchgfx/widgets/graph/AbstractDataGraph.hpp
 *
 * Declares classes touchgfx::AbstractDataGraph, touchgfx::AbstractDataGraphWithY and touchgfx::AbstractDataGraphWithXY
 */
#ifndef TOUCHGFX_ABSTRACTDATAGRAPH_HPP
#define TOUCHGFX_ABSTRACTDATAGRAPH_HPP

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/CWRUtil.hpp>

namespace touchgfx
{
class AbstractGraphElement;
class AbstractGraphDecoration;

/**
 * An abstract data graph. This class is the base for other graphs. For specific graphs, please
 * refer to those implementation, e.g. GraphScroll, GraphWrapAndClear, GraphWrapAndOverwrite and
 * StaticGraph.
 */
class AbstractDataGraph : public Container
{
public:
    /**
     * An object of this type is passed with each callback that is sent when the graph is
     * clicked. The object contains the data index that was pressed and the details of the click
     * event, e.g. PRESSED, RELEASED and screen coordinates.
     */
    class GraphClickEvent
    {
    public:
        /**
         * Initializes a new instance of the GraphClickEvent class.
         *
         * @param  i     The index of the item clicked.
         * @param  event The ClickEvent that caused the callback to be executed.
         *
         * @see setClickAction
         */
        GraphClickEvent(int16_t i, const ClickEvent& event)
            : index(i), clickEvent(event)
        {
        }

        int16_t index;                ///< The index of the item clicked.
        const ClickEvent& clickEvent; ///< The ClickEvent that caused the callback to be executed.
    };

    /**
     * An object of this type is passed with each callback that is sent when the graph is
     * dragged. The object contains the data index that was pressed and the details of the drag
     * event, e.g. old and new screen coordinates.
     */
    class GraphDragEvent
    {
    public:
        /**
         * Initializes a new instance of the GraphDragEvent class.
         *
         * @param  i     The index of the item where the drag has ended.
         * @param  event The DragEvent that caused the callback to be executed.
         *
         * @see setDragAction
         */
        GraphDragEvent(int16_t i, const DragEvent& event)
            : index(i), dragEvent(event)
        {
        }

        int16_t index;              ///< The index of the item where the drag has ended.
        const DragEvent& dragEvent; ///< The DragEvent that caused the callback to be executed.
    };

    /**
     * Initializes a new instance of the AbstractDataGraph class.
     *
     * @param  capacity The capacity, i.e. the number of points in the graph.
     */
    AbstractDataGraph(int16_t capacity);

    /**
     * Sets a scaling factor to be multiplied on each X value added. Since the graph only stores
     * integer values internally, it is possible to set a scale to e.g. 1000 and make the graph
     * work as if there are three digits of precision. The addDataPoint() will multiply the x
     * argument with the scaling factor and store this value. Please note the addDataPointScaled,
     * which allows adding a value that is assumed to have already been multiplied by the
     * scaling factor. Other functions exist in a "scaled" version where the arguments are
     * assumed to already be scaled.
     *
     * By setting the scale to 1 it is possible to simply use integer values for the graph.
     *
     * @param  scale      The scaling factor.
     * @param  updateData (Optional) True to update all existing x value.
     *
     * @see getScaleX, setScaleY, getScaleY
     */
    virtual void setScaleX(int scale, bool updateData = false)
    {
        (void)updateData; // Unused variable
        assert(scale != 0);
        xScale = scale;
    }

    /**
     * Gets the scaling factor previously set using setScaleX().
     *
     * @return The scaling factor.
     *
     * @see setScaleX, setScaleY, getScaleY
     */
    virtual int getScaleX() const
    {
        return xScale;
    }

    /**
     * Sets a scaling factor to be multiplied on each Y value added. Since the graph only stores
     * integer values internally, it is possible to set a scale to e.g. 1000 and make the graph
     * work as if there are three digits of precision. The addDataPoint() will multiply the y
     * argument with the scaling factor and store this value. Please note the addDataPointScaled,
     * which allows adding a value that is assumed to have already been multiplied by the
     * scaling factor. Other functions exist in a "scaled" version where the arguments are
     * assumed to already be scaled.
     *
     * By setting the scale to 1 it is possible to simply use integer values for the graph.
     *
     * @param  scale      The scaling factor.
     * @param  updateData (Optional) True to update all existing y value.
     *
     * @see getScaleY, setScaleX, getScaleX
     */
    virtual void setScaleY(int scale, bool updateData = false)
    {
        (void)updateData; // Unused variable
        assert(scale != 0);
        yScale = scale;
    }

    /**
     * Gets the scaling factor previously set using setScaleY().
     *
     * @return The scaling factor.
     *
     * @see setScaleY, setScaleX, getScaleX
     */
    virtual int getScaleY() const
    {
        return yScale;
    }

    /**
     * @copydoc Image::setAlpha()
     *
     * @note All graph elements have to take this alpha into consideration.
     */
    virtual void setAlpha(uint8_t newAlpha)
    {
        alpha = newAlpha;
    }

    /** @copydoc Image::getAlpha() */
    FORCE_INLINE_FUNCTION uint8_t getAlpha() const
    {
        return alpha;
    }

    virtual void setWidth(int16_t width);

    virtual void setHeight(int16_t height);

    /**
     * Sets graph position inside the widget by reserving a margin around the graph. These areas
     * to the left, the right, above and below are used for drawing optional axis and titles.
     *
     * @param  top    The top margin in pixels.
     * @param  left   The left margin in pixels.
     * @param  right  The right margin in pixels.
     * @param  bottom The bottom margin in pixels.
     *
     * @see GraphLabelsX, GraphLabelsY, GraphTitle, getGraphAreaMarginTop, getGraphAreaMarginLeft,
     *      getGraphAreaMarginRight, getGraphAreaMarginBottom, setGraphAreaPadding
     *
     * @note The graph is automatically invalidated when the graph margins are changed.
     */
    void setGraphAreaMargin(int16_t top, int16_t left, int16_t right, int16_t bottom);

    /**
     * Gets graph margin top.
     *
     * @return The graph margin top.
     *
     * @see setGraphAreaMargin
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaMarginTop() const
    {
        return topArea.getHeight();
    }

    /**
     * Gets graph margin left.
     *
     * @return The graph margin left.
     *
     * @see setGraphAreaMargin
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaMarginLeft() const
    {
        return leftArea.getWidth();
    }

    /**
     * Gets graph margin right.
     *
     * @return The graph margin right.
     *
     * @see setGraphAreaMargin
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaMarginRight() const
    {
        return rightArea.getWidth();
    }

    /**
     * Gets graph margin bottom.
     *
     * @return The graph margin bottom.
     *
     * @see setGraphAreaMargin
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaMarginBottom() const
    {
        return bottomArea.getHeight();
    }

    /**
     * Adds padding around the graph that will not be drawn in (apart from dots, boxes, lines
     * etc. that extend around the actual data point). The set padding will also work to make a
     * gap between the graph and any labels that might have been added to the graph. To reserve
     * an area that the graph will not be drawn in, use setGraphAreaMargin.
     *
     * @param  top    The top padding in pixels.
     * @param  left   The left padding in pixels.
     * @param  right  The right padding in pixels.
     * @param  bottom The bottom padding in pixels.
     *
     * @see setGraphAreaMargin, getGraphAreaPaddingTop, getGraphAreaPaddingLeft,
     *      getGraphAreaPaddingRight, getGraphAreaPaddingBottom
     *
     * @note The graph is automatically invalidated when the margins are set.
     */
    void setGraphAreaPadding(int16_t top, int16_t left, int16_t right, int16_t bottom);

    /**
     * Gets graph area padding top.
     *
     * @return The graph area padding top.
     *
     * @see setGraphAreaPadding
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaPaddingTop() const
    {
        return topPadding;
    }

    /**
     * Gets graph area padding left.
     *
     * @return The graph area padding left.
     *
     * @see setGraphAreaPadding
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaPaddingLeft() const
    {
        return leftPadding;
    }

    /**
     * Gets graph area padding right.
     *
     * @return The graph area padding right.
     *
     * @see setGraphAreaPadding
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaPaddingRight() const
    {
        return rightPadding;
    }

    /**
     * Gets graph area padding bottom.
     *
     * @return The graph area padding bottom.
     *
     * @see setGraphAreaPadding
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaPaddingBottom() const
    {
        return bottomPadding;
    }

    /**
     * Gets graph area width. This is the width of the actual graph area and is the same as the
     * width of the graph widget minus margin and padding.
     *
     * @return The graph area width.
     *
     * @see setGraphAreaMargin, setGraphAreaPadding
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaWidth() const
    {
        return graphArea.getWidth() - (leftPadding + rightPadding);
    }

    /**
     * Gets graph area width including padding (but not margin). This is the width of the actual
     * graph area and is the same as the width of the graph widget minus margin.
     *
     * @return The graph width including graph padding.
     *
     * @see setGraphAreaPadding
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaWidthIncludingPadding() const
    {
        return graphArea.getWidth();
    }

    /**
     * Gets graph area height. This is the height of the actual graph area and is the same as
     * the height of the graph widget minus margin and padding.
     *
     * @return The graph area height.
     *
     * @see setGraphAreaMargin, setGraphAreaPadding
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaHeight() const
    {
        return graphArea.getHeight() - (topPadding + bottomPadding);
    }

    /**
     * Gets graph area height including padding (but not margin). This is the height of the
     * actual graph area and is the same as the height of the graph widget minus margin.
     *
     * @return The graph area height including graph padding.
     *
     * @see setGraphAreaMargin
     */
    FORCE_INLINE_FUNCTION int16_t getGraphAreaHeightIncludingPadding() const
    {
        return graphArea.getHeight();
    }

    /**
     * Sets minimum and maximum x and y coordinate ranges for the graph. This can be used to
     * zoom in or out and only show parts of the graph. The range given is scaled.
     *
     * @param  xMin The minimum x value.
     * @param  xMax The maximum x value.
     * @param  yMin The minimum y value.
     * @param  yMax The maximum y value.
     *
     * @see setScaleX, setScaleY, setGraphRangeX, setGraphRangeY, setGraphRangeXAuto,
     *      setGraphRangeYAuto, setGraphRangeScaled
     */
    FORCE_INLINE_FUNCTION void setGraphRange(int xMin, int xMax, int yMin, int yMax)
    {
        setGraphRangeX(xMin, xMax);
        setGraphRangeY(yMin, yMax);
    }

    /**
     * @copydoc AbstractDataGraph::setGraphRange(int, int, int, int)
     */
    FORCE_INLINE_FUNCTION void setGraphRange(int xMin, int xMax, float yMin, float yMax)
    {
        setGraphRangeX(xMin, xMax);
        setGraphRangeY(yMin, yMax);
    }

    /**
     * @copydoc AbstractDataGraph::setGraphRange(int, int, int, int)
     */
    FORCE_INLINE_FUNCTION void setGraphRange(float xMin, float xMax, int yMin, int yMax)
    {
        setGraphRangeX(xMin, xMax);
        setGraphRangeY(yMin, yMax);
    }

    /**
     * @copydoc AbstractDataGraph::setGraphRange(int, int, int, int)
     */
    FORCE_INLINE_FUNCTION void setGraphRange(float xMin, float xMax, float yMin, float yMax)
    {
        setGraphRangeX(xMin, xMax);
        setGraphRangeY(yMin, yMax);
    }

    /**
     * Same as setGraphRange(int, int, int, int) except the passed arguments are assumed scaled.
     * This means that if the scale is 1000, a value of 1 is interpreted as 0.001 (i.e. 1/1000).
     *
     * @param  xMin The minimum x value.
     * @param  xMax The maximum x value.
     * @param  yMin The minimum y value.
     * @param  yMax The maximum y value.
     *
     * @see setGraphRange
     */
    FORCE_INLINE_FUNCTION void setGraphRangeScaled(int xMin, int xMax, int yMin, int yMax)
    {
        setGraphRangeXScaled(xMin, xMax);
        setGraphRangeYScaled(yMin, yMax);
    }

    /**
     * Sets minimum and maximum x coordinates for the graph. This can be used to zoom in or out
     * and only show parts of the graph.
     *
     * @param  min The minimum x value.
     * @param  max The maximum x value.
     *
     * @note The graph as well as the area above and below are automatically redrawn (invalidated).
     */
    virtual void setGraphRangeX(int min, int max)
    {
        setGraphRangeXScaled(int2scaledX(min), int2scaledX(max));
    }

    /** @copydoc AbstractDataGraph::setGraphRangeX(int, int) */
    virtual void setGraphRangeX(float min, float max)
    {
        setGraphRangeXScaled(float2scaledX(min), float2scaledX(max));
    }

    /**
     * @copydoc AbstractDataGraph::setGraphRangeX(int, int)
     *
     * @see setGraphRangeX
     *
     * @note The arguments are assumed scaled. This means that if the scale is 1000, a value of 1
     *       is interpreted as 0.001 (i.e. 1/1000).
     */
    virtual void setGraphRangeXScaled(int min, int max);

    /**
     * Automatically adjust min and max x coordinate to show entire graph. It is possible to
     * ensure that the y axis (i.e. x=0) is included in the new range. If the graph range is
     * changed as a consequence of calling this function, the graph is automatically redrawn
     * (invalidated).
     *
     * @param  showYaxis (Optional) True to ensure that the y axis is visible (default is true).
     * @param  margin    (Optional) The margin to add above/below the max/min x value (default is
     *                   no margin).
     *
     * @see setGraphRangeXAuto, setGraphRangeXAutoScaled, setGraphRangeYAuto,
     *      setGraphRangeYAutoScaled
     *
     * @note This takes the currently visible y coordinate range into account.
     * @note If data in the graph is changed, added or deleted, the graph range is not
     *       automatically recalculated. Call this function again to achieve this functionality.
     */
    virtual void setGraphRangeXAuto(bool showYaxis = true, int margin = 0)
    {
        setGraphRangeXAutoScaled(showYaxis, int2scaledX(margin));
    }

    /** @copydoc AbstractDataGraph::setGraphRangeXAuto(bool, int) */
    virtual void setGraphRangeXAuto(bool showYaxis, float margin)
    {
        setGraphRangeXAutoScaled(showYaxis, float2scaledX(margin));
    }

    /**
     * @copydoc AbstractDataGraph::setGraphRangeXAuto(bool, int)
     *
     * @note The arguments are assumed scaled. This means that if the scale is 1000, a value of 1
     *       is interpreted as 0.001 (i.e. 1/1000).
     */
    virtual void setGraphRangeXAutoScaled(bool showYaxis, int margin) = 0;

    /**
     * Sets minimum and maximum y coordinates for the graph. This can be used to zoom in or out
     * and only show parts of the graph.
     *
     * @param  min The minimum y value.
     * @param  max The maximum y value.
     *
     * @note The graph as well as the area to the left and to the right of the graph are
     *       automatically redrawn (invalidated)
     */
    virtual void setGraphRangeY(int min, int max)
    {
        setGraphRangeYScaled(int2scaledY(min), int2scaledY(max));
    }

    /**
     * @copydoc AbstractDataGraph::setGraphRangeY(int, int)
     */
    virtual void setGraphRangeY(float min, float max)
    {
        setGraphRangeYScaled(float2scaledY(min), float2scaledY(max));
    }

    /**
     * Same as setGraphRangeY(int, int) except the passed arguments are assumed scaled.
     *
     * @param  min The minimum y value.
     * @param  max The maximum y value.
     *
     * @see setGraphRangeY
     */
    virtual void setGraphRangeYScaled(int min, int max);

    /**
     * Automatic adjust min and max y coordinate to show entire graph. It is possible to ensure
     * that the x axis (i.e. y=0) is included in the new range. If the graph range is changed,
     * the graph is automatically redrawn (invalidated).
     *
     * @param  showXaxis True to ensure that the x axis is visible (default is true).
     * @param  margin    The margin to add above/below the max/min y value (default is no margin).
     *
     * @note This takes the current visible x coordinate range into account.
     */
    virtual void setGraphRangeYAutoScaled(bool showXaxis, int margin) = 0;

    /** @copydoc AbstractDataGraph::setGraphRangeYAutoScaled(bool, int) */
    virtual void setGraphRangeYAuto(bool showXaxis = true, int margin = 0)
    {
        setGraphRangeYAutoScaled(showXaxis, int2scaledY(margin));
    }

    /** @copydoc AbstractDataGraph::setGraphRangeYAutoScaled(bool, int) */
    virtual void setGraphRangeYAuto(bool showXaxis, float margin)
    {
        setGraphRangeYAutoScaled(showXaxis, float2scaledY(margin));
    }

    /**
     * Gets minimum x value for the graph.
     *
     * @return The minimum x value.
     */
    virtual int getGraphRangeXMinAsInt() const
    {
        return scaled2intX(graphRangeMinX);
    }

    /**
     * Gets minimum x value for the graph.
     *
     * @return The minimum x value.
     */
    virtual float getGraphRangeXMinAsFloat() const
    {
        return scaled2floatX(graphRangeMinX);
    }

    /**
     * @copydoc AbstractDataGraph::getGraphRangeXMinAsInt()
     *
     * @return The graph range x coordinate minimum scaled.
     *
     * @see AbstractDataGraph::getGraphRangeXMinAsInt, AbstractDataGraph::getGraphRangeXMinAsFloat
     *
     * @note The returned value is left scaled.
     */
    virtual int getGraphRangeXMinScaled() const
    {
        return graphRangeMinX;
    }

    /**
     * Gets maximum x value for the graph.
     *
     * @return The maximum x value.
     */
    virtual int getGraphRangeXMaxAsInt() const
    {
        return scaled2intX(graphRangeMaxX);
    }

    /**
     * Gets maximum x value for the graph.
     *
     * @return The maximum x value.
     */
    virtual float getGraphRangeXMaxAsFloat() const
    {
        return scaled2floatX(graphRangeMaxX);
    }

    /**
     * @copydoc AbstractDataGraph::getGraphRangeXMaxAsInt()
     *
     * @return The graph range x coordinate maximum scaled.
     *
     * @see AbstractDataGraph::getGraphRangeXMaxAsInt, AbstractDataGraph::getGraphRangeXMaxAsFloat
     *
     * @note The returned value is left scaled.
     */
    virtual int getGraphRangeXMaxScaled() const
    {
        return graphRangeMaxX;
    }

    /**
     * Gets minimum y value for the graph.
     *
     * @return The minimum y value.
     */
    virtual int getGraphRangeYMinAsInt() const
    {
        return scaled2intY(graphRangeMinY);
    }

    /**
     * Gets minimum y value for the graph.
     *
     * @return The minimum y value.
     */
    virtual float getGraphRangeYMinAsFloat() const
    {
        return scaled2floatY(graphRangeMinY);
    }

    /**
     * @copydoc AbstractDataGraph::getGraphRangeYMinAsInt()
     *
     * @see AbstractDataGraph::getGraphRangeYMinAsInt, AbstractDataGraph::getGraphRangeYMinAsFloat
     *
     * @note The returned value is left scaled.
     */
    virtual int getGraphRangeYMinScaled() const
    {
        return graphRangeMinY;
    }

    /**
     * Gets maximum y value for the graph.
     *
     * @return The maximum y value.
     */
    virtual int getGraphRangeYMaxAsInt() const
    {
        return scaled2intY(graphRangeMaxY);
    }

    /**
     * Gets maximum y value for the graph.
     *
     * @return The maximum y value.
     */
    virtual float getGraphRangeYMaxAsFloat() const
    {
        return scaled2floatY(graphRangeMaxY);
    }

    /**
     * @copydoc AbstractDataGraph::getGraphRangeYMaxAsInt()
     *
     * @see AbstractDataGraph::getGraphRangeYMaxAsInt, AbstractDataGraph::getGraphRangeYMaxAsFloat
     *
     * @note The returned value is left scaled.
     */
    virtual int getGraphRangeYMaxScaled() const
    {
        return graphRangeMaxY;
    }

    /**
     * Clears the graph to its blank/initial state. The graph is invalidated so that the empty
     * graph will be drawn after a call to clear.
     */
    virtual void clear();

    /**
     * Makes gap before the specified index. This can be used to split a graph in two, but for
     * some graph types, e.g. histograms, this has no effect. Only one gap can be specified at a
     * time. Specifying a new gap automatically removes the previous gap.
     *
     * @param  index Zero-based index where the gap should be placed.
     */
    FORCE_INLINE_FUNCTION void setGapBeforeIndex(int16_t index)
    {
        gapBeforeIndex = index;
    }

    /**
     * Gets gap before index as set using setGapBeforeIndex().
     *
     * @return The gap before index.
     *
     * @see setGapBeforeIndex
     */
    FORCE_INLINE_FUNCTION int16_t getGapBeforeIndex() const
    {
        return gapBeforeIndex;
    }

    /**
     * Adds a graph element which will display the graph. Several graph elements can be added.
     * Examples of graph elements are lines, dots, histograms as well as horizontal and vertical
     * grid lines.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphElementGridX, GraphElementGridY, GraphElementArea, GraphElementBoxes,
     *      GraphElementDiamonds, GraphElementDots, GraphElementHistogram, GraphElementLine,
     */
    void addGraphElement(AbstractGraphElement& d);

    /**
     * Adds an element to be shown in the area above the graph. Labels and titles can be added
     * here.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphLabelsX, GraphTitle
     */
    void addTopElement(AbstractGraphDecoration& d);

    /**
     * Adds an element to be shown in the area to the left of the graph. Labels and titles can
     * be added here.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphLabelsY, GraphTitle
     */
    void addLeftElement(AbstractGraphDecoration& d);

    /**
     * Adds an element to be shown in the area to the right of the graph. Labels and titles can
     * be added here.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphLabelsY, GraphTitle
     */
    void addRightElement(AbstractGraphDecoration& d);

    /**
     * Adds an element to be shown in the area below the graph.  Labels and titles can be added
     * here.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphLabelsX, GraphTitle
     */
    void addBottomElement(AbstractGraphDecoration& d);

    /**
     * Gets the number of point used by the graph.
     *
     * @return The number of point used by the graph.
     */
    FORCE_INLINE_FUNCTION int16_t getUsedCapacity() const
    {
        return usedCapacity;
    }

    /**
     * Gets the capacity (max number of points) of the graph.
     *
     * @return The capacity.
     */
    FORCE_INLINE_FUNCTION int16_t getMaxCapacity() const
    {
        return maxCapacity;
    }

    /**
     * Gets graph index nearest to the given screen position. The distance to each point on the
     * graph is measured and the index of the point closest to the given position handed back.
     *
     * @param       x     The x value.
     * @param       y     The y value.
     * @param [out] index Zero-based index of the point closest to the given position.
     *
     * @return True if it succeeds, false if it fails.
     *
     * @see getNearestIndexForScreenX
     */
    virtual bool getNearestIndexForScreenXY(int16_t x, int16_t y, int16_t& index);

    /**
     * Gets graph index nearest to the given screen x value. The index of the graph point
     * closest to the given x coordinate is handed back.
     *
     * @param       x     The x value.
     * @param [out] index Zero-based index of the.
     *
     * @return True if it succeeds, false if it fails.
     *
     * @see getNearestIndexForScreenXY
     */
    virtual bool getNearestIndexForScreenX(int16_t x, int16_t& index) const;

    /**
     * Get the screen x coordinate for the given graph point index.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The screen x value.
     */
    FORCE_INLINE_FUNCTION int16_t indexToScreenX(int16_t index) const
    {
        return indexToScreenXQ5(index).round();
    }

    /**
     * Get the screen y coordinate for the given graph point index.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The screen x value.
     */
    FORCE_INLINE_FUNCTION int16_t indexToScreenY(int16_t index) const
    {
        return indexToScreenYQ5(index).round();
    }

    /**
     * Gets data point x coordinate as int.
     *
     * @param  index Zero-based index of the value to fetch.
     *
     * @return The data point x coordinate as int.
     *
     * @see indexToDataPointXAsFloat, indexToDataPointXScaled, indexToDataPointYAsInt, indexToDataPoint
     */
    FORCE_INLINE_FUNCTION int indexToDataPointXAsInt(int16_t index) const
    {
        return scaled2intX(indexToDataPointXScaled(index));
    }

    /**
     * Gets data point x coordinate as float.
     *
     * @param  index Zero-based index of the value to fetch.
     *
     * @return The data point x coordinate as float.
     *
     * @see indexToDataPointXAsInt, indexToDataPointXScaled, indexToDataPointYAsFloat, indexToDataPoint
     */
    FORCE_INLINE_FUNCTION float indexToDataPointXAsFloat(int16_t index) const
    {
        return scaled2floatX(indexToDataPointXScaled(index));
    }

    /**
     * Gets data point x coordinate scaled.
     *
     * @param  index Zero-based index of the value to fetch.
     *
     * @return The data point x coordinate scaled.
     *
     * @see indexToDataPointXAsInt, indexToDataPointXAsFloat, indexToDataPointYScaled, indexToDataPoint,
     *          indexToDataPointScaled
     */
    virtual int indexToDataPointXScaled(int16_t index) const = 0;

    /**
     * Gets data point y coordinate as int.
     *
     * @param  index Zero-based index of the value to fetch.
     *
     * @return The data point y coordinate as int.
     *
     * @see indexToDataPointYAsFloat, indexToDataPointYScaled, indexToDataPointXAsInt, indexToDataPoint
     */
    FORCE_INLINE_FUNCTION int indexToDataPointYAsInt(int16_t index) const
    {
        return scaled2intY(indexToDataPointYScaled(index));
    }

    /**
     * Gets data point y coordinate as float.
     *
     * @param  index Zero-based index of the value to fetch.
     *
     * @return The data point y coordinate as float.
     *
     * @see indexToDataPointYAsInt, indexToDataPointYScaled, indexToDataPointXAsFloat, indexToDataPoint
     */
    FORCE_INLINE_FUNCTION float indexToDataPointYAsFloat(int16_t index) const
    {
        return scaled2floatY(indexToDataPointYScaled(index));
    }

    /**
     * Gets data point y coordinate scaled.
     *
     * @param  index Zero-based index of the value to fetch.
     *
     * @return The data point y coordinate scaled.
     *
     * @see indexToDataPointYAsInt, indexToDataPointYAsFloat, indexToDataPointXScaled, indexToDataPoint,
     *          indexToDataPointScaled
     */
    virtual int indexToDataPointYScaled(int16_t index) const = 0;

    /**
     * Gets data point as integer values.
     *
     * @param       index  Zero-based index of the values to fetch.
     * @param [out] xvalue The x value.
     * @param [out] yvalue The y value.
     *
     * @see indexToDataPointXAsInt, indexToDataPointYAsInt, indexToDataPointScaled
     */
    FORCE_INLINE_FUNCTION void indexToDataPoint(int16_t index, int& xvalue, int& yvalue) const
    {
        xvalue = indexToDataPointXAsInt(index);
        yvalue = indexToDataPointYAsInt(index);
    }

    /**
     * Gets data point as floating point values.
     *
     * @param       index  Zero-based index of the values to fetch.
     * @param [out] xvalue The x value.
     * @param [out] yvalue The y value.
     *
     * @see indexToDataPointXAsFloat, indexToDataPointYAsFloat, indexToDataPointScaled
     */
    FORCE_INLINE_FUNCTION void indexToDataPoint(int16_t index, float& xvalue, float& yvalue) const
    {
        xvalue = indexToDataPointXAsFloat(index);
        yvalue = indexToDataPointYAsFloat(index);
    }

    /**
     * Gets data point as raw (scaled) values.
     *
     * @param       index  Zero-based index of the values to fetch.
     * @param [out] xvalue The x value.
     * @param [out] yvalue The y value.
     */
    FORCE_INLINE_FUNCTION void indexToDataPointScaled(int16_t index, int& xvalue, int& yvalue) const
    {
        xvalue = indexToDataPointXScaled(index);
        yvalue = indexToDataPointYScaled(index);
    }

    /**
     * Sets an action to be executed when the graph is clicked.
     *
     * @param  callback The callback.
     *
     * @see GraphClickEvent
     */
    FORCE_INLINE_FUNCTION void setClickAction(GenericCallback<const AbstractDataGraph&, const GraphClickEvent&>& callback)
    {
        clickAction = &callback;
    }

    /**
     * Sets an action to be executed when the graph is dragged.
     *
     * @param  callback The callback.
     *
     * @see GraphDragEvent
     */
    FORCE_INLINE_FUNCTION void setDragAction(GenericCallback<const AbstractDataGraph&, const GraphDragEvent&>& callback)
    {
        dragAction = &callback;
    }

    virtual void handleClickEvent(const ClickEvent& event);

    virtual void handleDragEvent(const DragEvent& event);

    /**
     * Multiply an integer value with a constant.
     *
     * @param  i     the value to scale.
     * @param  scale The scale.
     *
     * @return The product of the two numbers.
     */
    FORCE_INLINE_FUNCTION static int int2scaled(int i, int scale)
    {
        return i * scale;
    }

    /**
     * Same as int2scaled(int, int) using the graph's x scale.
     *
     * @param  i The integer value to scale.
     *
     * @return The scaled integer.
     *
     * @note For internal use.
     */
    FORCE_INLINE_FUNCTION int int2scaledX(int i) const
    {
        return int2scaled(i, xScale);
    }

    /**
     * Same as int2scaled(int, int) using the graph's y scale.
     *
     * @param  i The integer value to scale.
     *
     * @return The scaled integer.
     *
     * @note For internal use.
     */
    FORCE_INLINE_FUNCTION int int2scaledY(int i) const
    {
        return int2scaled(i, yScale);
    }

    /**
     * Multiply a floating point value with a constant and round the result.
     *
     * @param  f     the value to scale.
     * @param  scale The scale.
     *
     * @return The product of the two numbers, rounded to nearest integer value.
     */
    FORCE_INLINE_FUNCTION static int float2scaled(float f, int scale)
    {
        const float fs = f * (float)scale;
        if (fs >= 0)
        {
            return (int)(fs + 0.5f);
        }
        return ((int)fs - 1) + (int)(1.5f + (fs - (float)(int)fs));
    }

    /**
     * Same as float2scaled(float, int) using the graph's x scale.
     *
     * @param  f The floating point value to scale.
     *
     * @return The scaled value.
     *
     * @note For internal use.
     */
    FORCE_INLINE_FUNCTION int float2scaledX(float f) const
    {
        return float2scaled(f, xScale);
    }

    /**
     * Same as float2scaled(float, int) using the graph's y scale.
     *
     * @param  f The floating point value to scale.
     *
     * @return The scaled value.
     *
     * @note For internal use.
     */
    FORCE_INLINE_FUNCTION int float2scaledY(float f) const
    {
        return float2scaled(f, yScale);
    }

    /**
     * Divide an integer with a constant and round the result.
     *
     * @param  i     The number to divide.
     * @param  scale The divisor (scale).
     *
     * @return The number divided by the scale, rounded to nearest integer.
     */
    FORCE_INLINE_FUNCTION static int scaled2int(int i, int scale)
    {
        if (i >= 0)
        {
            return ((i * 2 + scale) / scale) / 2;
        }
        return -(((-i * 2 + scale - 1) / scale) / 2);
    }

    /**
     * Same as scaled2int(int, int) using the graph's x scale.
     *
     * @param  i The scaled value to convert to an integer.
     *
     * @return The unscaled value.
     *
     * @note For internal use.
     */
    FORCE_INLINE_FUNCTION int scaled2intX(int i) const
    {
        return scaled2int(i, xScale);
    }

    /**
     * Same as scaled2int(int, int) using the graph's y scale.
     *
     * @param  i The scaled value to convert to an integer.
     *
     * @return The unscaled value.
     *
     * @note For internal use.
     */
    FORCE_INLINE_FUNCTION int scaled2intY(int i) const
    {
        return scaled2int(i, yScale);
    }

    /**
     * Divide a floating point number with a constant.
     *
     * @param  i     The number to divide.
     * @param  scale The divisor (scale).
     *
     * @return The number divided by the scale.
     */
    FORCE_INLINE_FUNCTION static float scaled2float(int i, int scale)
    {
        return (float)i / (float)scale;
    }

    /**
     * Same as scaled2float(int, int) using the graph's x scale.
     *
     * @param  i The scaled value to convert to a floating point value.
     *
     * @return The unscaled value.
     *
     * @note For internal use.
     */
    FORCE_INLINE_FUNCTION float scaled2floatX(int i) const
    {
        return scaled2float(i, xScale);
    }

    /**
     * Same as scaled2float(int, int) using the graph's y scale.
     *
     * @param  i The scaled value to convert to a floating point value.
     *
     * @return The unscaled value.
     *
     * @note For internal use.
     */
    FORCE_INLINE_FUNCTION float scaled2floatY(int i) const
    {
        return scaled2float(i, yScale);
    }

    /**
     * Converts a value from one scale to another scale. This would convert eg. 21 in scale 10 to
     * 210 in scale 100 or to 2 in scale 1.
     *
     * @param  value    The value to convert to another scale.
     * @param  oldScale The old (current) scale of the value.
     * @param  newScale The new scale that the value should be converted to.
     *
     * @return The given data converted to a new scale.
     */
    static int convertToNewScale(int value, int oldScale, int newScale);

    /** Invalidate content. */
    virtual void invalidateContent() const
    {
        if (alpha > 0)
        {
            Container::invalidateContent();
        }
    }

protected:
    uint8_t alpha;          ///< The alpha of the entire graph
    int xScale;             ///< The data scale for the x values
    int yScale;             ///< The data scale for the y values
    Container graphArea;    ///< The graph area (the center area)
    Container leftArea;     ///< The area to the left of the graph
    Container rightArea;    ///< The area to the right of the graph
    Container topArea;      ///< The area above the graph
    Container bottomArea;   ///< The area below the graph
    int16_t topPadding;     ///< The graph area top padding
    int16_t leftPadding;    ///< The graph area left padding
    int16_t rightPadding;   ///< The graph area right padding
    int16_t bottomPadding;  ///< The graph area bottom padding
    int16_t maxCapacity;    ///< Maximum number of points in the graph
    int16_t usedCapacity;   ///< The number of used points in the graph
    int16_t gapBeforeIndex; ///< The graph is disconnected (there is a gap) before this element index.

    GenericCallback<const AbstractDataGraph&, const GraphClickEvent&>* clickAction; ///< The callback to be executed when this Graph is clicked
    GenericCallback<const AbstractDataGraph&, const GraphDragEvent&>* dragAction;   ///< The callback to be executed when this Graph is dragged

    int graphRangeMinX; ///< The graph range minimum x coordinate
    int graphRangeMaxX; ///< The graph range maximum x coordinate
    int graphRangeMinY; ///< The graph range minimum y coordinate
    int graphRangeMaxY; ///< The graph range maximum y coordinate

    /**
     * Invalidate point at a given index. This will call the function invalidateGraphPointAt()
     * on every element added to the graphArea which in turn is responsible for invalidating the
     * part of the screen occupied by its element.
     *
     * @param  index Zero-based index of the element to invalidate.
     */
    void invalidateGraphPointAt(int16_t index);

    /**
     * Invalidate entire graph area (the center of the graph). This is often useful when a graph
     * is cleared or the X or Y range is changed.
     */
    FORCE_INLINE_FUNCTION void invalidateGraphArea() const
    {
        graphArea.invalidate();
    }

    /**
     * Invalidate x axis point at the given index. Since the y axis is often static, the x axis
     * can change, and all labels need to be updated without redrawing the entire graph.
     *
     * @param  index The x index to invalidate.
     *
     * @see invalidateAllXAxisPoints
     */
    void invalidateXAxisPointAt(int16_t index);

    /**
     * Invalidate all x axis points. Similar to invalidateXAxisPointAt, this function will
     * iterate all visible x values and invalidate them in turn.
     *
     * @see invalidateXAxisPointAt
     */
    void invalidateAllXAxisPoints();

    /**
     * Updates the position of all elements in all area after a change in size of the graph area
     * and/or label padding.
     *
     * @note The entire graph area is invalidated.
     */
    void updateAreasPosition();

    /**
     * Convert the given valueScaled (index) to x axis value.
     *
     * @param  valueScaled The value scaled.
     * @param  labelScaled The label scaled.
     *
     * @return The x axis value.
     */
    virtual int indexToXAxis(const int valueScaled, const int labelScaled) const = 0;

    /**
     * Gets screen x coordinate for an absolute value.
     *
     * @param  x The x value.
     *
     * @return The screen x coordinate for the given value.
     */
    virtual CWRUtil::Q5 valueToScreenXQ5(int x) const
    {
        return CWRUtil::muldiv_toQ5(x - graphRangeMinX, getGraphAreaWidth() - 1, graphRangeMaxX - graphRangeMinX) + CWRUtil::toQ5(leftPadding);
    }

    /**
     * Gets screen y coordinate for an absolute value.
     *
     * @param  y The y value.
     *
     * @return The screen y coordinate for the given value.
     */
    virtual CWRUtil::Q5 valueToScreenYQ5(int y) const
    {
        const int16_t graphAreaHeight = getGraphAreaHeight();
        return CWRUtil::toQ5(graphAreaHeight + topPadding - 1) - CWRUtil::muldiv_toQ5(y - graphRangeMinY, graphAreaHeight - 1, graphRangeMaxY - graphRangeMinY);
    }

    /**
     * Gets screen x coordinate for a specific data point added to the graph.
     *
     * @param  index The index of the element to get the x coordinate for.
     *
     * @return The screen x coordinate for the specific data point.
     */
    virtual CWRUtil::Q5 indexToScreenXQ5(int16_t index) const = 0;

    /**
     * Gets screen y coordinate for a specific data point added to the graph.
     *
     * @param  index The index of the element to get the y coordinate for.
     *
     * @return The screen x coordinate for the specific data point.
     */
    virtual CWRUtil::Q5 indexToScreenYQ5(int16_t index) const = 0;

    /**
     * Gets index range for screen x coordinate range taking the current graph range into
     * account.
     *
     * @param       xMin     The low screen x coordinate.
     * @param       xMax     The high screen x coordinate.
     * @param [out] indexMin The low element index.
     * @param [out] indexMax The high element index.
     *
     * @return True if the range from low index to high index is legal.
     *
     * @note For internal use.
     */
    virtual bool xScreenRangeToIndexRange(int16_t xMin, int16_t xMax, int16_t& indexMin, int16_t& indexMax) const = 0;

    /** An abstract graph element. */
    friend class AbstractGraphElement;
};

/**
 * Abstract helper class used to implement graphs with the same distance between the x values
 * (i.e. x is ignored).
 */
class DynamicDataGraph : public AbstractDataGraph
{
public:
    /**
     * Initializes a new instance of the AbstractDataGraphWithY class.
     *
     * @param      capacity The capacity.
     * @param [in] values   Address where to store the y values of the graph.
     */
    DynamicDataGraph(int16_t capacity, int* values)
        : AbstractDataGraph(capacity), xAxisFactor(1), xAxisOffset(0), yValues(values), dataCounter(0)
    {
        DynamicDataGraph::setGraphRangeX(0, capacity - 1);
    }

    /**
     * Sets a scaling factor to be multiplied on each added element. Since the graph only stores
     * integer values internally, it is possible to set a scale to e.g. 1000 and make the graph
     * work as if there are three digits of precision. The addDataPoint() will multiply the
     * argument with the scaling factor and store this value.
     *
     * By setting the scale to 1 it is possible to simply use integer values for the graph.
     *
     * @param  scale      The scaling factor.
     * @param  updateData (Optional) True to update all existing y value.
     *
     * @see getScale
     *
     * @note The current xAxisFactor, xAxisOffet and xGraphRange values are updated to reflect the
     *       new scale being set.
     */
    virtual void setScale(int scale, bool updateData = false)
    {
        setScaleX(scale, updateData);
        setScaleY(scale, updateData);
    }

    /**
     * Gets the scaling factor previously set using setScale().
     *
     * @return The scaling factor.
     *
     * @see setScale
     */
    virtual int getScale() const
    {
        return getScaleY();
    }

    /**
     * Adds a new data point to the end of the graph. The part of the graph that is changed, is
     * automatically redrawn (invalidated).
     *
     * @param  y The new data point.
     *
     * @return The index of the just added value.
     */
    FORCE_INLINE_FUNCTION int16_t addDataPoint(int y)
    {
        return addDataPointScaled(int2scaledY(y));
    }

    /** @copydoc addDataPoint(int) */
    FORCE_INLINE_FUNCTION int16_t addDataPoint(float y)
    {
        return addDataPointScaled(float2scaledY(y));
    }

    /**
     * @copydoc addDataPoint(int)
     *
     * @return The index of the added data point.
     *
     * @note The y value must already be scaled.
     */
    int16_t addDataPointScaled(int y);

    /**
     * Gets the minimum x coordinate for the graph.
     *
     * @return The minimum x coordinate .
     */
    virtual int getGraphRangeXMin() const
    {
        return getGraphRangeXMinScaled();
    }

    /**
     * Gets the maximum x coordinate for the graph.
     *
     * @return The maximum x coordinate .
     */
    virtual int getGraphRangeXMax() const
    {
        return getGraphRangeXMaxScaled();
    }

    virtual void setGraphRangeYAutoScaled(bool showXaxis, int margin);

    virtual void setGraphRangeXAutoScaled(bool /*showYaxis*/, int margin)
    {
        setGraphRangeXScaled(-scaled2intX(margin), (usedCapacity - 1) + scaled2intX(margin));
    }

    /// @cond
    TOUCHGFX_DEPRECATED("Please use setXAxisFactor()", virtual void setXAxisScale(int scale))
    {
        setXAxisFactor(scale);
    }

    /// @endcond

    /**
     * Set x coordinate axis factor value. This is the real x value increment between two data
     * points added to the graph.
     *
     * @param  factor The x axis factor.
     */
    virtual void setXAxisFactor(int factor)
    {
        setXAxisFactorScaled(int2scaledX(factor));
    }

    /// @cond
    TOUCHGFX_DEPRECATED("Please use setXAxisFactor()", virtual void setXAxisScale(float scale))
    {
        setXAxisFactor(scale);
    }

    /// @endcond

    /** @copydoc setXAxisFactor(int) */
    virtual void setXAxisFactor(float factor)
    {
        setXAxisFactorScaled(float2scaledX(factor));
    }

    /**
     * Set x coordinate axis factor value using a pre-scaled value. This is the real x value
     * increment between two data points added to the graph.
     *
     * @param  factor The x axis factor.
     *
     * @see setXAxisFactorScaled,setXAxisFactor
     */
    virtual void setXAxisFactorScaled(int factor)
    {
        xAxisFactor = factor;
    }

    /// @cond
    TOUCHGFX_DEPRECATED("Please use getXAxisFactorAsInt()", virtual int getXAxisScaleAsInt() const)
    {
        return getXAxisFactorAsInt();
    }

    /// @endcond

    /**
     * Get x coordinate axis factor value. This is the real x value increment between two data
     * points added to the graph.
     *
     * @return The x axis factor.
     */
    virtual int getXAxisFactorAsInt() const
    {
        return scaled2intX(getXAxisFactorScaled());
    }

    /// @cond
    TOUCHGFX_DEPRECATED("Please use getXAxisFactorAsFloat()", virtual float getXAxisScaleAsFloat() const)
    {
        return getXAxisFactorAsFloat();
    }

    /// @endcond

    /** @copydoc getXAxisFactorAsInt() */
    virtual float getXAxisFactorAsFloat() const
    {
        return scaled2floatX(getXAxisFactorScaled());
    }

    /**
     * Get x axis factor as a scaled value.
     *
     * @return The x axis factor (scaled).
     *
     * @see getXAxisFactorAsInt, getXAxisFactorAsFloat, setXAxisFactor
     */
    virtual int getXAxisFactorScaled() const
    {
        return xAxisFactor;
    }

    /**
     * Set x coordinate axis offset value. This is the real x value of the first data point
     * added to the graph (i.e. the data point at index 0).
     *
     * @param  offset The x axis offset.
     */
    virtual void setXAxisOffset(int offset)
    {
        setXAxisOffsetScaled(int2scaledX(offset));
    }

    /** @copydoc setXAxisOffset(int) */
    virtual void setXAxisOffset(float offset)
    {
        setXAxisOffsetScaled(float2scaledX(offset));
    }

    /**
     * Set x coordinate axis offset value with a pre-scaled offset value. This is the real x
     * value of the first data point added to the graph (i.e. the data point at index 0).
     *
     * @param  offset The x axis offset.
     *
     * @see setXAxisOffset
     *
     * @see setXAxisOffset, getXAxisOffsetScaled
     */
    virtual void setXAxisOffsetScaled(int offset)
    {
        xAxisOffset = offset;
    }

    /**
     * Get x coordinate axis offset value. This is the real x value of the first data point
     * added to the graph (i.e. the data point at index 0).
     *
     * @return The x axis offset.
     */
    virtual int getXAxisOffsetAsInt() const
    {
        return scaled2intX(getXAxisOffsetScaled());
    }

    /** @copydoc getXAxisOffsetAsInt() */
    virtual float getXAxisOffsetAsFloat() const
    {
        return scaled2floatX(getXAxisOffsetScaled());
    }

    /**
     * Get x axis offset as a scaled value.
     *
     * @return The x axis offset (scaled).
     *
     * @see getXAxisOffsetAsInt, getXAxisOffsetAsFloat, setXAxisOffsetScaled, setXAxisOffset
     */
    virtual int getXAxisOffsetScaled() const
    {
        return xAxisOffset;
    }

    virtual int indexToDataPointXScaled(int16_t index) const
    {
        assert(index >= 0 && index < usedCapacity);
        return (indexToGlobalIndex(index) * xAxisFactor) + xAxisOffset;
    }

    virtual int indexToDataPointYScaled(int16_t index) const
    {
        assert(index >= 0 && index < usedCapacity);
        return yValues[dataIndex(index)];
    }

protected:
    int xAxisFactor;      ///< The axis scale (real distance between two indices)
    int xAxisOffset;      ///< The x axis offset (real value of data point at index 0)
    int* yValues;         ///< The values of the graph
    uint32_t dataCounter; ///< The data counter of how many times addDataPoint() has been called

    /**
     * This function is called before a new value (data point) is added. This allows for
     * invalidation to be calculated based on the global data counter before it is increased as
     * a result of adding the new point.
     */
    virtual void beforeAddValue()
    {
    }

    /**
     * Adds a value to the internal data array and keeps track of when graph points, graph axis
     * and the entire graph needs to be redrawn (invalidated).
     *
     * @param  value The value to add to the array.
     *
     * @return The index of the newly added value.
     */
    virtual int16_t addValue(int value) = 0;

    /**
     * Get the data index in the yValues array of the given screen index. Normally this is just
     * the 'i' but e.g. GraphScrollData does not, for performance reasons.
     *
     * @param  screenIndex Zero-based screen index.
     *
     * @return The index in the yValues array.
     */
    virtual int16_t dataIndex(int16_t screenIndex) const
    {
        return screenIndex;
    }

    /**
     * Convert an index to global index. The index is the index of any data point, The global
     * index is a value that keeps growing whenever a new data point is added the the graph.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The global index.
     */
    virtual int32_t indexToGlobalIndex(int16_t index) const
    {
        return (int32_t)index;
    }

    virtual int indexToXAxis(const int valueScaled, const int /*labelScaled*/) const
    {
        // Value is also index for these types of graphs.
        return (indexToGlobalIndex(scaled2intX(valueScaled)) * xAxisFactor) + xAxisOffset;
    }

    virtual bool xScreenRangeToIndexRange(int16_t xMin, int16_t xMax, int16_t& indexMin, int16_t& indexMax) const;

    virtual void setScaleX(int scale, bool updateData = false);

    virtual int getScaleX() const
    {
        return AbstractDataGraph::getScaleX();
    }

    virtual void setScaleY(int scale, bool updateData = false);

    virtual int getScaleY() const
    {
        return AbstractDataGraph::getScaleY();
    }

    /// @cond
    TOUCHGFX_DEPRECATED("Please use setXAxisFactorScaled()", virtual void setXAxisScaleScaled(int scale))
    {
        setXAxisFactorScaled(scale);
    }

    /// @endcond

    /// @cond
    TOUCHGFX_DEPRECATED("Please use getXAxisFactorScaled()", virtual int getXAxisScaleScaled() const)
    {
        return getXAxisFactorScaled();
    }

    /// @endcond

    virtual CWRUtil::Q5 indexToScreenXQ5(int16_t index) const
    {
        return valueToScreenXQ5(int2scaledX(index));
    }

    virtual CWRUtil::Q5 indexToScreenYQ5(int16_t index) const
    {
        return valueToScreenYQ5(yValues[dataIndex(index)]);
    }
};

/**
 * Abstract helper class used to implement graphs with the same distance between the x values
 * (i.e. x is ignored).
 */
class StaticDataGraph : public AbstractDataGraph
{
public:
    /**
     * Initializes a new instance of the AbstractDataGraphWithY class.
     *
     * @param      capacity The capacity.
     * @param [in] xvalues  Address where to store the x values of the graph.
     * @param [in] yvalues  Address where to store the y values of the graph.
     */
    StaticDataGraph(int16_t capacity, int* xvalues, int* yvalues)
        : AbstractDataGraph(capacity), xValues(xvalues), yValues(yvalues)
    {
        StaticDataGraph::setGraphRangeX(0, capacity - 1);
    }

    virtual void setScaleX(int scale, bool updateData = false);

    virtual void setScaleY(int scale, bool updateData = false);

    /**
     * Adds a new data point to the end of the graph. The part of the graph that is changed, is
     * automatically redrawn (invalidated).
     *
     * @param  x The x value.
     * @param  y The y value.
     *
     * @return The index of the just added value.
     */
    FORCE_INLINE_FUNCTION int16_t addDataPoint(int x, int y)
    {
        return addValue(int2scaledX(x), int2scaledY(y));
    }

    /** @copydoc addDataPoint(int, int) */
    FORCE_INLINE_FUNCTION int16_t addDataPoint(float x, float y)
    {
        return addValue(float2scaledX(x), float2scaledY(y));
    }

    /**
     * @copydoc addDataPoint(int, int)
     *
     * @return The index of the added data point.
     *
     * @note Same as addDataPoint(int, int) except the passed argument is assumed scaled.
     */
    FORCE_INLINE_FUNCTION int16_t addDataPointScaled(int x, int y)
    {
        return addValue(x, y);
    }

    /**
     * Gets data point x coordinate scaled.
     *
     * @param  index Zero-based index of the value to fetch.
     *
     * @return The data point x coordinate scaled.
     *
     * @see indexToDataPointXAsInt, indexToDataPointXAsFloat, indexToDataPointYScaled,
     *      indexToDataPoint, indexToDataPointScaled
     */
    virtual int indexToDataPointXScaled(int16_t index) const
    {
        assert(index >= 0 && index < usedCapacity);
        return xValues[index];
    }

    /**
     * Gets data point y coordinate scaled.
     *
     * @param  index Zero-based index of the value to fetch.
     *
     * @return The data point y coordinate scaled.
     *
     * @see indexToDataPointYAsInt, indexToDataPointYAsFloat, indexToDataPointXScaled,
     *      indexToDataPoint, indexToDataPointScaled
     */
    virtual int indexToDataPointYScaled(int16_t index) const
    {
        assert(index >= 0 && index < usedCapacity);
        return yValues[index];
    }

    /**
     * Deletes the data point at the given x value.
     *
     * @param  x The x value.
     *
     * @return The index of the deleted value, -1 if the x coordinate was not found.
     */
    FORCE_INLINE_FUNCTION int deleteDataPoint(int x)
    {
        return deleteDataPointScaled(int2scaledX(x));
    }

    /** @copydoc deleteDataPoint(int)*/
    FORCE_INLINE_FUNCTION int deleteDataPoint(float x)
    {
        return deleteDataPointScaled(float2scaledX(x));
    }

    /**
     * Same as dalateDataPoint(int) except the passed argument is assumed scaled. Deletes a data
     * point scaled.
     *
     * @param  x The x value.
     *
     * @return The index of the deleted value, -1 if the x coordinate was not found.
     */
    FORCE_INLINE_FUNCTION int16_t deleteDataPointScaled(int x)
    {
        return deleteValue(x);
    }

    /**
     * Same as deleteDataPoint(int) except the passed argument is the index of the value to
     * delete. Deletes a data point.
     *
     * @param  index Zero-based index of the values to delete.
     *
     * @return The index of the deleted value, -1 if the x coordinate was not found.
     */
    FORCE_INLINE_FUNCTION int16_t deleteDataPointIndex(int index)
    {
        return deleteIndex(index);
    }

    /**
     * Sets graph range x coordinate automatic scaled.
     *
     * @param  showYaxis True to show, false to hide the y axis.
     * @param  margin    The margin.
     */
    virtual void setGraphRangeXAutoScaled(bool showYaxis, int margin);

    /**
     * Sets graph range y coordinate automatic scaled.
     *
     * @param  showXaxis (Optional) True to show, false to hide the x axis.
     * @param  margin    (Optional) The margin.
     */
    virtual void setGraphRangeYAutoScaled(bool showXaxis = true, int margin = 0);

protected:
    int* xValues; ///< The x values of the graph
    int* yValues; ///< The y values of the graph

    /**
     * Adds a value to the internal data array and keeps track of when graph points, graph axis
     * and the entire graph needs to be redrawn (invalidated).
     *
     * @param  xvalue The x value to add to the array.
     * @param  yvalue The y value to add to the array.
     *
     * @return The index of the newly added value, or -1 if the value could not be added.
     */
    virtual int16_t addValue(int xvalue, int yvalue) = 0;

    /**
     * Removes a value from the internal data array and keeps track of when graph points, graph
     * axis and the entire graph needs to be redrawn (invalidated).
     *
     * @param  xvalue The x value to remove from the array.
     *
     * @return The index of the removed value, or -1 if the value could not be removed.
     */
    virtual int16_t deleteValue(int xvalue);

    /**
     * Removes a value from the internal data array and keeps track of when graph points, graph
     * axis and the entire graph needs to be redrawn (invalidated).
     *
     * @param  index Zero-based index of the data point to delete.
     *
     * @return The index of the removed value, or -1 if the value could not be removed.
     */
    virtual int16_t deleteIndex(int index);

    virtual int indexToXAxis(const int /*valueScaled*/, const int labelScaled) const
    {
        return labelScaled;
    }

    virtual bool xScreenRangeToIndexRange(int16_t xMin, int16_t xMax, int16_t& indexMin, int16_t& indexMax) const;

    virtual CWRUtil::Q5 indexToScreenXQ5(int16_t index) const
    {
        return valueToScreenXQ5(xValues[index]);
    }

    virtual CWRUtil::Q5 indexToScreenYQ5(int16_t index) const
    {
        return valueToScreenYQ5(yValues[index]);
    }
};

} // namespace touchgfx

#endif // TOUCHGFX_ABSTRACTDATAGRAPH_HPP
