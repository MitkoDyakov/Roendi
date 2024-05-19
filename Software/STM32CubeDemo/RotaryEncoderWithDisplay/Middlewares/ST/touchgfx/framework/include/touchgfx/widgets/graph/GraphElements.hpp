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
 * @file touchgfx/widgets/graph/GraphElements.hpp
 *
 * Declares the various graph element classes. Instances of these classes can be added to a graph.
 */
#ifndef TOUCHGFX_GRAPHELEMENTS_HPP
#define TOUCHGFX_GRAPHELEMENTS_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/CWRUtil.hpp>
#include <touchgfx/widgets/canvas/Canvas.hpp>
#include <touchgfx/widgets/canvas/CanvasWidget.hpp>
#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>

namespace touchgfx
{
/**
 * An abstract graph element. Declares a couple of useful functions to help subclasses which use
 * CWR (Canvas Widget Renderer).
 */
class AbstractGraphElement : public CanvasWidget
{
public:
    /** Default constructor. */
    AbstractGraphElement()
        : CanvasWidget()
    {
    }

    /**
     * Invalidate the point at the given index. This allows a graph element to only invalidate
     * the minimum rectangle required for the given index. The Graph will call this function
     * before and after changing a point to ensure that both the old and the new area are
     * redrawn (invalidated).
     *
     * @param  index Zero-based index of the point.
     */
    virtual void invalidateGraphPointAt(int16_t index) = 0;

protected:
    /**
     * Gets a pointer to the the graph containing the GraphElement.
     *
     * @return A pointer to the graph.
     */
    FORCE_INLINE_FUNCTION AbstractDataGraph* getGraph() const
    {
        return parent ? (AbstractDataGraph*)(parent->getParent()) : (AbstractDataGraph*)0;
    }

    /**
     * Gets graph screen x for x value.
     *
     * @param  graph The graph.
     * @param  x     The x value.
     *
     * @return The graph screen x for value.
     */
    FORCE_INLINE_FUNCTION CWRUtil::Q5 valueToScreenXQ5(const AbstractDataGraph* graph, int x) const
    {
        return graph->valueToScreenXQ5(x);
    }

    /**
     * Gets graph screen y for y value.
     *
     * @param  graph The graph.
     * @param  y     The y value.
     *
     * @return The graph screen y for value.
     */
    FORCE_INLINE_FUNCTION CWRUtil::Q5 valueToScreenYQ5(const AbstractDataGraph* graph, int y) const
    {
        return graph->valueToScreenYQ5(y);
    }

    /**
     * Gets screen x coordinate for a specific data point added to the graph.
     *
     * @param  graph The graph.
     * @param  index The index of the element to get the x coordinate for.
     *
     * @return The screen x coordinate for the specific data point.
     */
    FORCE_INLINE_FUNCTION CWRUtil::Q5 indexToScreenXQ5(const AbstractDataGraph* graph, int16_t index) const
    {
        return graph->indexToScreenXQ5(index);
    }

    /**
     * Gets screen y coordinate for a specific data point added to the graph.
     *
     * @param  graph The graph.
     * @param  index The index of the element to get the y coordinate for.
     *
     * @return The screen x coordinate for the specific data point.
     */
    FORCE_INLINE_FUNCTION CWRUtil::Q5 indexToScreenYQ5(const AbstractDataGraph* graph, int16_t index) const
    {
        return graph->indexToScreenYQ5(index);
    }

    /**
     * Gets graph element range for screen x coordinate range.
     *
     * @param       graph    The graph.
     * @param       xMin     The low X.
     * @param       xMax     The high X.
     * @param [out] indexMin The low index.
     * @param [out] indexMax The high index.
     *
     * @return True if it succeeds, false if it fails.
     */
    FORCE_INLINE_FUNCTION bool xScreenRangeToIndexRange(const AbstractDataGraph* graph, int16_t xMin, int16_t xMax, int16_t& indexMin, int16_t& indexMax) const
    {
        return graph->xScreenRangeToIndexRange(xMin, xMax, indexMin, indexMax);
    }

    /**
     * Find the screen rectangle containing the Q5 screen rectangle by rounding the coordinates
     * up/down.
     *
     * @param  screenXminQ5 The minimum screen x coordinate (in Q5).
     * @param  screenYminQ5 The maximum screen y coordinate (in Q5).
     * @param  screenXmaxQ5 The minimum screen x coordinate (in Q5).
     * @param  screenYmaxQ5 The maximum screen y coordinate (in Q5).
     *
     * @return A Rect containing the Q5 rectangle.
     */
    Rect rectFromQ5Coordinates(CWRUtil::Q5 screenXminQ5, CWRUtil::Q5 screenYminQ5, CWRUtil::Q5 screenXmaxQ5, CWRUtil::Q5 screenYmaxQ5) const;

    /**
     * Find the screen rectangle around a given point with the specified diameter.
     *
     * @param  xQ5        The screen x coordinate (in Q5).
     * @param  yQ5        The screen y coordinate (in Q5).
     * @param  diameterQ5 The diameter (in Q5).
     *
     * @return A Rect containing the point (and diameter).
     */
    Rect rectAround(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5, CWRUtil::Q5 diameterQ5) const;

    /**
     * Round the given CWRUtil::Q5 to the nearest integer and return it as a CWRUtil::Q5 instead
     * of an integer.
     *
     * @param  q5 The CWRUtil::Q5 value to round.
     *
     * @return The nearest integer as a CWRUtil::Q5 value.
     */
    FORCE_INLINE_FUNCTION CWRUtil::Q5 roundQ5(CWRUtil::Q5 q5) const
    {
        return CWRUtil::toQ5(q5.round());
    }

    /**
     * Converts the value to the proper X scale of the graph.
     *
     * @param  graph The graph.
     * @param  value The value.
     * @param  scale The scale.
     *
     * @return The given data converted to a graph scale x coordinate.
     */
    FORCE_INLINE_FUNCTION int convertToGraphScaleX(const AbstractDataGraph* graph, int value, int scale) const
    {
        return AbstractDataGraph::convertToNewScale(value, scale, graph->getScaleX());
    }

    /**
     * Converts the value to the proper Y scale of the graph.
     *
     * @param  graph The graph.
     * @param  value The value.
     * @param  scale The scale.
     *
     * @return The given data converted to a graph scale y coordinate.
     */
    FORCE_INLINE_FUNCTION int convertToGraphScaleY(const AbstractDataGraph* graph, int value, int scale) const
    {
        return AbstractDataGraph::convertToNewScale(value, scale, graph->getScaleY());
    }

    /**
     * @copydoc AbstractDataGraph::indexToXAxis
     *
     * @param  graph       The graph.
     *
     * @return The x axis value.
     */
    FORCE_INLINE_FUNCTION int getIndexToXAxis(const AbstractDataGraph* graph, const int valueScaled, const int labelScaled) const
    {
        return graph->indexToXAxis(valueScaled, labelScaled);
    }

    /**
     * Query if the center of a given data point index is visible inside the graph area.
     *
     * @param  graph The graph.
     * @param  index The data point index.
     *
     * @return True if center invisible, false if not.
     */
    bool isCenterInvisible(const AbstractDataGraph* graph, int16_t index) const;
};

/**
 * An abstract graph element. Declares a couple of useful functions to help subclasses which do
 * not use CWR (Canvas Widget Renderer).
 */
class AbstractGraphElementNoCWR : public AbstractGraphElement
{
public:
    AbstractGraphElementNoCWR()
        : color(0)
    {
    }

    /**
     * Sets the color of the graph element.
     *
     * @param  newColor The new color.
     *
     * @see getColor
     */
    virtual void setColor(colortype newColor)
    {
        color = newColor;
    }

    /**
     * Gets the color of the graph element.
     *
     * @return The color.
     *
     * @see setColor
     */
    virtual colortype getColor() const
    {
        return color;
    }

    virtual bool drawCanvasWidget(const Rect&) const
    {
        return true;
    }

protected:
    colortype color; ///< The currently assigned color

    /**
     * Normalize rectangle by changing a rectangle with negative width or height to a rectangle
     * with positive width or height at the correct position.
     *
     * @param [in,out] r The rectangle.
     */
    void normalizeRect(Rect& r) const;

private:
    virtual void setPainter(AbstractPainter& /*painter*/)
    {
    }
};

/**
 * GraphElementGridBase is a helper class used to implement classed to draw grid lines in the
 * graph.
 */
class GraphElementGridBase : public AbstractGraphElementNoCWR
{
public:
    /** Default constructor. */
    GraphElementGridBase()
        : dataScale(1), gridInterval(10), lineWidth(1), majorGrid(0)
    {
    }

    virtual void draw(const Rect& invalidatedArea) const;

    /**
     * Sets a scaling factor to be multiplied on the grid interval. Any already set grid interval
     * will be updated to the new correctly scaled value. To avoid this, use setIntervalScaled()
     * after setting scale.
     *
     * @param  scale The scaling factor.
     *
     * @see getScale
     */
    FORCE_INLINE_FUNCTION void setScale(int scale)
    {
        assert(scale != 0);
        gridInterval = AbstractDataGraph::convertToNewScale(gridInterval, dataScale, scale);
        dataScale = scale;
    }

    /**
     * Gets the scaling factor set using setScale.
     *
     * @return The scaling factor.
     *
     * @see setScale
     */
    FORCE_INLINE_FUNCTION int getScale() const
    {
        return dataScale;
    }

    /**
     * Sets the interval between each grid line.
     *
     * @param  interval The interval between each grid line.
     *
     * @see getIntervalAsInt, getIntervalAsFloat, setMajorGrid
     *
     * @note If interval is 0 only the axis is shown.
     */
    FORCE_INLINE_FUNCTION void setInterval(int interval)
    {
        setIntervalScaled(abs(interval) * dataScale);
    }

    /** @copydoc setInterval(int) */
    FORCE_INLINE_FUNCTION void setInterval(float interval)
    {
        setIntervalScaled(AbstractDataGraph::float2scaled(abs(interval), dataScale));
    }

    /**
     * @copydoc setInterval(int)
     *
     * @note The interval set here must already be scaled.
     */
    FORCE_INLINE_FUNCTION void setIntervalScaled(int interval)
    {
        gridInterval = abs(interval);
    }

    /**
     * Gets the interval between each grid line.
     *
     * @return The interval between each grid line.
     *
     * @see setInterval
     */
    FORCE_INLINE_FUNCTION int getIntervalAsInt() const
    {
        return AbstractDataGraph::scaled2int(getIntervalScaled(), dataScale);
    }

    /** @copydoc getIntervalAsInt() */
    FORCE_INLINE_FUNCTION float getIntervalAsFloat() const
    {
        return AbstractDataGraph::scaled2float(getIntervalScaled(), dataScale);
    }

    /**
     * @copydoc getIntervalAsInt()
     *
     * @note The interval returned here is left unscaled.
     */
    FORCE_INLINE_FUNCTION int getIntervalScaled() const
    {
        return gridInterval;
    }

    /**
     * Sets "major" grid that will be responsible for drawing major grid lines. If a grid line
     * would be drawn at the same position as the major grid line, the grid line will not be
     * drawn.
     *
     * @param  major Reference to a major grid line object.
     */
    FORCE_INLINE_FUNCTION void setMajorGrid(const GraphElementGridBase& major)
    {
        majorGrid = &major;
    }

    /**
     * Sets line width of the grid lines.
     *
     * @param  width The width of the grid lines.
     *
     * @see getLineWidth
     */
    FORCE_INLINE_FUNCTION void setLineWidth(uint8_t width)
    {
        lineWidth = width;
    }

    /**
     * Gets line width.
     *
     * @return The line width.
     *
     * @see setLineWidth
     */
    FORCE_INLINE_FUNCTION uint8_t getLineWidth() const
    {
        return lineWidth;
    }

    virtual void invalidateGraphPointAt(int16_t)
    {
    }

    /**
     * Gets correctly scaled minor interval, as the minor grid may have a scale that differs the
     * scale of the graph and this grid line.
     *
     * @param  graph The graph.
     *
     * @return The correctly scaled minor interval.
     */
    virtual int getCorrectlyScaledGridInterval(const AbstractDataGraph* graph) const = 0;

    /**
     * Gets correctly scaled major interval, as the major grid may have a scale that differs the
     * scale of the graph and this grid line.
     *
     * @param  graph The graph.
     *
     * @return The correctly scaled major interval.
     */
    virtual int getCorrectlyScaledMajorInterval(const AbstractDataGraph* graph) const
    {
        return majorGrid == 0 ? 0 : majorGrid->getCorrectlyScaledGridInterval(graph);
    }

protected:
    int dataScale;                         ///< The scaling factor
    int gridInterval;                      ///< The grid line interval.
    uint8_t lineWidth;                     ///< Width of the line.
    const GraphElementGridBase* majorGrid; ///< A pointer to a major grid, if any

    /**
     * Draw vertical line using LCD::fillRect and handles negative dimensions properly.
     *
     * @param  invalidatedArea The invalidated area to intersect the line with.
     * @param  start           The start coordinate (offset from edge).
     * @param  length          The length of the line.
     * @param  pos             The start coordinate (distance in span).
     * @param  width           The width of the line.
     * @param  a               The alpha of the line.
     */
    virtual void drawLine(const Rect& invalidatedArea, int16_t start, int16_t length, int16_t pos, int16_t width, uint8_t a) const;

    /**
     * Return the enclosing area for the grid lines. Vertical lines are allowed to go a bit to
     * the left and right, but not above or below. Vice versa for horizontal gridlines.
     *
     * @param  graph The graph.
     *
     * @return A Rect which the grid lines should be inside.
     */
    virtual Rect enclosingArea(const AbstractDataGraph* graph) const = 0;

    /**
     * Return the specific rectangle for line.
     *
     * @param  start  The start (distance from the border).
     * @param  length The length (length of the line).
     * @param  pos    The position (horizonal offset for vertical lines, vertical offset for
     *                horizontal lines).
     * @param  width  The (line) width.
     *
     * @return A Rect.
     */
    virtual Rect lineRect(int16_t start, int16_t length, int16_t pos, int16_t width) const = 0;

    /**
     * Gets graph range minimum x/y scaled.
     *
     * @param  graph The graph.
     *
     * @return The graph range minimum scaled.
     */
    virtual int getGraphRangeMinScaled(const AbstractDataGraph* graph) const = 0;

    /**
     * Gets graph range maximum x/y scaled.
     *
     * @param  graph The graph.
     *
     * @return The graph range maximum scaled.
     */
    virtual int getGraphRangeMaxScaled(const AbstractDataGraph* graph) const = 0;

    /**
     * Gets graph area start (distance from the border).
     *
     * @param  graph The graph.
     *
     * @return The graph area start (distance from the border).
     */
    virtual int getGraphAreaStart(const AbstractDataGraph* graph) const = 0;

    /**
     * Gets graph area length (distance from border to border).
     *
     * @param  graph The graph.
     *
     * @return The graph area length (distance from border to border.
     */
    virtual int getGraphAreaLength(const AbstractDataGraph* graph) const = 0;

    /**
     * Gets graph area start position. Horizontal offset of first vertical line or vertical
     * offset of first horizontal line.
     *
     * @param  graph The graph.
     *
     * @return The graph area start position. Horizontal offset of first vertical line or
     *         vertical offset og first horizontal line.
     */
    virtual int getGraphAreaStartPos(const AbstractDataGraph* graph) const = 0;

    /**
     * Gets graph area end position. Horizontal offset of last vertical line or vertical offset
     * of last horizontal line.
     *
     * @param  graph The graph.
     *
     * @return The graph area end position. Horizontal offset of last vertical line or vertical
     *         offset of last horizontal line.
     */
    virtual int getGraphAreaEndPos(const AbstractDataGraph* graph) const = 0;

    /**
     * The graph value to the correct screen coordinate in Q5.
     *
     * @param  graph The graph.
     * @param  value The value.
     *
     * @return The graph value to the correct screen coordinate in Q5.
     */
    virtual CWRUtil::Q5 valueToScreenQ5(const AbstractDataGraph* graph, int value) const = 0;
};

/**
 * GraphElementGridX draws vertical lines at selected intervals along the x axis. By combining
 * two GraphElementGridX instances, it is possible to have minor and major grid lines.
 *
 * @note The grid lines are drawn using LCD::fillRect for higher performance.
 */
class GraphElementGridX : public GraphElementGridBase
{
public:
    virtual int getCorrectlyScaledGridInterval(const AbstractDataGraph* graph) const
    {
        return convertToGraphScaleX(graph, gridInterval, dataScale);
    }

protected:
    virtual Rect enclosingArea(const AbstractDataGraph* graph) const
    {
        return Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight());
    }

    virtual Rect lineRect(int16_t start, int16_t length, int16_t pos, int16_t width) const
    {
        return Rect(pos, start, width, length);
    }

    virtual int getGraphRangeMinScaled(const AbstractDataGraph* graph) const
    {
        return graph->getGraphRangeXMinScaled();
    }

    virtual int getGraphRangeMaxScaled(const AbstractDataGraph* graph) const
    {
        return graph->getGraphRangeXMaxScaled();
    }

    virtual int getGraphAreaStart(const AbstractDataGraph* graph) const
    {
        return graph->getGraphAreaPaddingTop();
    }

    virtual int getGraphAreaLength(const AbstractDataGraph* graph) const
    {
        return graph->getGraphAreaHeight();
    }

    virtual int getGraphAreaStartPos(const AbstractDataGraph* graph) const
    {
        return graph->getGraphAreaPaddingLeft();
    }

    virtual int getGraphAreaEndPos(const AbstractDataGraph* graph) const
    {
        return graph->getGraphAreaWidth();
    }

    virtual CWRUtil::Q5 valueToScreenQ5(const AbstractDataGraph* graph, int value) const
    {
        return valueToScreenXQ5(graph, value);
    }
};

/**
 * GraphElementGridY draws horizontal lines at selected intervals along the y axis. By combining
 * two GraphElementGridY instances, it is possible to have minor and major grid lines.
 *
 * @note The grid lines are drawn using LCD::fillRect for higher performance.
 */
class GraphElementGridY : public GraphElementGridBase
{
public:
    virtual int getCorrectlyScaledGridInterval(const AbstractDataGraph* graph) const
    {
        return convertToGraphScaleY(graph, gridInterval, dataScale);
    }

protected:
    virtual Rect enclosingArea(const AbstractDataGraph* graph) const
    {
        return Rect(graph->getGraphAreaPaddingLeft(), 0, graph->getGraphAreaWidth(), graph->getGraphAreaHeightIncludingPadding());
    }

    virtual Rect lineRect(int16_t start, int16_t length, int16_t pos, int16_t width) const
    {
        return Rect(start, pos, length, width);
    }

    virtual int getGraphRangeMinScaled(const AbstractDataGraph* graph) const
    {
        return graph->getGraphRangeYMinScaled();
    }

    virtual int getGraphRangeMaxScaled(const AbstractDataGraph* graph) const
    {
        return graph->getGraphRangeYMaxScaled();
    }

    virtual int getGraphAreaStart(const AbstractDataGraph* graph) const
    {
        return graph->getGraphAreaPaddingLeft();
    }

    virtual int getGraphAreaLength(const AbstractDataGraph* graph) const
    {
        return graph->getGraphAreaWidth();
    }

    virtual int getGraphAreaStartPos(const AbstractDataGraph* graph) const
    {
        return graph->getGraphAreaPaddingTop();
    }

    virtual int getGraphAreaEndPos(const AbstractDataGraph* graph) const
    {
        return graph->getGraphAreaHeight();
    }

    virtual CWRUtil::Q5 valueToScreenQ5(const AbstractDataGraph* graph, int value) const
    {
        return valueToScreenYQ5(graph, value);
    }
};

/**
 * GraphElementArea will fill the area below the line connecting the data points in the graph.
 *
 * @note The Area is drawn using Canvas Widget Renderer which is slower but produces much nicer
 *       graphics.
 */
class GraphElementArea : public AbstractGraphElement
{
public:
    GraphElementArea()
        : dataScale(1), yBaseline(0)
    {
    }

    /**
     * Sets a scaling factor to be multiplied on the baseline. Any already set baseline will be
     * updated to the new correctly scaled value. To avoid this, use setBaselineScaled()
     * after setting scale.
     *
     * @param  scale The scaling factor.
     *
     * @see getScale
     */
    FORCE_INLINE_FUNCTION void setScale(int scale)
    {
        assert(scale != 0);
        yBaseline = AbstractDataGraph::convertToNewScale(yBaseline, dataScale, scale);
        dataScale = scale;
    }

    /**
     * Gets the scaling factor set using setScale.
     *
     * @return The scaling factor.
     *
     * @see setScale
     */
    FORCE_INLINE_FUNCTION int getScale() const
    {
        return dataScale;
    }

    /**
     * Sets the base of the area drawn. Normally, the base is 0 which means that the area is
     * drawn below positive y values and above negative y values. Setting the base to a very
     * high number will cause the area above the graph to be drawn. Setting the base to a very
     * low number will cause the area below the graph to be drawn (even for negative numbers,
     * which are higher than the base value).
     *
     * @param  baseline The baseline value.
     *
     * @see getBaselineAsInt, getBaselineAsFloat
     */
    FORCE_INLINE_FUNCTION void setBaseline(int baseline)
    {
        setBaselineScaled(baseline * dataScale);
    }

    /** @copydoc setBaseline(int) */
    FORCE_INLINE_FUNCTION void setBaseline(float baseline)
    {
        setBaselineScaled(AbstractDataGraph::float2scaled(baseline, dataScale));
    }

    /**
     * @copydoc setBaseline(int)
     *
     * @note The baseline set here must already be scaled.
     */
    FORCE_INLINE_FUNCTION void setBaselineScaled(int baseline)
    {
        yBaseline = baseline;
    }

    /**
     * Gets the base previously set using setBase.
     *
     * @return The base value.
     *
     * @see setBaseline
     */
    FORCE_INLINE_FUNCTION int getBaselineAsInt() const
    {
        return AbstractDataGraph::scaled2int(getBaselineScaled(), dataScale);
    }

    /** @copydoc getBaselineAsInt() */
    FORCE_INLINE_FUNCTION float getBaselineAsFloat() const
    {
        return AbstractDataGraph::scaled2float(getBaselineScaled(), dataScale);
    }

    /**
     * @copydoc getBaselineAsInt()
     *
     * @note The baseline returned here is left unscaled.
     */
    FORCE_INLINE_FUNCTION int getBaselineScaled() const
    {
        return yBaseline;
    }

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    int dataScale; ///< The scaling factor
    int yBaseline; ///< The base value.
};

/**
 * GraphElementLine will draw a line with a given thickness through the data points in the graph.
 *
 * @note The Line is drawn using Canvas Widget Renderer which is slower but produces much nicer
 *       graphics.
 */
class GraphElementLine : public AbstractGraphElement
{
public:
    GraphElementLine()
        : lineWidth(2)
    {
    }

    /**
     * Sets line width.
     *
     * @param  width The width.
     *
     * @see getLineWidth
     */
    FORCE_INLINE_FUNCTION void setLineWidth(uint8_t width)
    {
        lineWidth = width;
    }

    /**
     * Gets line width.
     *
     * @return The line width.
     *
     * @see setLineWidth
     */
    FORCE_INLINE_FUNCTION uint8_t getLineWidth() const
    {
        return lineWidth;
    }

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint8_t lineWidth; ///< Width of the line

    /**
     * Draw a line between all indexes in the given range. This is used where there is a gap in
     * the graph and the line has to be drawn as two separate lines.
     *
     * @param [in] canvas   The canvas.
     * @param      graph    The graph.
     * @param      indexMin The minimum index.
     * @param      indexMax The maximum index.
     */
    void drawIndexRange(Canvas& canvas, const AbstractDataGraph* graph, int16_t indexMin, int16_t indexMax) const;
};

/**
 * The GraphElementVerticalGapLine is used to draw a vertical line where the gap in the graph
 * is. This only makes sense to add to a GraphWrapAndOverwrite (or GraphWrapAndOverwriteData).
 *
 * @note The vertical line is drawn using LCD::fillRect for higher performance.
 */
class GraphElementVerticalGapLine : public AbstractGraphElementNoCWR
{
public:
    /**
     * Sets the width of the gap line in pixels. If the gap line is set to 0 the gap line will
     * extend to the next point in the graph.
     *
     * @param  width The width.
     *
     * @see getGapLineWidth
     */
    FORCE_INLINE_FUNCTION void setGapLineWidth(uint16_t width)
    {
        lineWidth = width;
    }

    /**
     * Gets the width of the gap line as set using setGapLineWidth().
     *
     * @return The gap line width.
     */
    FORCE_INLINE_FUNCTION uint16_t getGapLineWidth() const
    {
        return lineWidth;
    }

    virtual void draw(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint16_t lineWidth; ///< Width of the line

private:
    void invalidateIndex(const AbstractDataGraph* graph, int16_t index) const;
};

/**
 * The GraphElementHistogram is used to draw blocks from the x axis to the data point in the
 * graph. If more graphs are placed on top of each other, the histogram can be moved slightly to
 * the left/right.
 *
 * @note Historgram boxes are drawn using LCD::fillRect for higher performance.
 */
class GraphElementHistogram : public AbstractGraphElementNoCWR
{
public:
    GraphElementHistogram()
        : dataScale(1), yBaseline(0), barWidth(2), barOffset(0)
    {
    }

    /**
     * Sets a scaling factor to be multiplied on the baseline. Any already set baseline will be
     * updated to the new correctly scaled value. To avoid this, use setBaselineScaled()
     * after setting scale.
     *
     * @param  scale The scaling factor.
     *
     * @see getScale
     */
    FORCE_INLINE_FUNCTION void setScale(int scale)
    {
        assert(scale != 0);
        yBaseline = AbstractDataGraph::convertToNewScale(yBaseline, dataScale, scale);
        dataScale = scale;
    }

    /**
     * Gets the scaling factor set using setScale.
     *
     * @return The scaling factor.
     *
     * @see setScale
     */
    FORCE_INLINE_FUNCTION int getScale() const
    {
        return dataScale;
    }

    /**
     * Sets the base of the area drawn. Normally, the base is 0 which means that the area is
     * drawn below positive y values and above negative y values. Setting the base to a very
     * high number will cause the area above the graph to be drawn. Setting the base to a very
     * low number will cause the area below the graph to be drawn (even for negative numbers,
     * which are higher than the base value).
     *
     * @param  baseline The base value.
     *
     * @see getBaselineAsInt, getBaselineAsFloat
     */
    FORCE_INLINE_FUNCTION void setBaseline(int baseline)
    {
        setBaselineScaled(baseline * dataScale);
    }

    /** @copydoc setBaseline(int) */
    FORCE_INLINE_FUNCTION void setBaseline(float baseline)
    {
        setBaselineScaled(AbstractDataGraph::float2scaled(baseline, dataScale));
    }

    /**
     * @copydoc setBaseline(int)
     *
     * @note The baseline set here must already be scaled.
     */
    FORCE_INLINE_FUNCTION void setBaselineScaled(int baseline)
    {
        yBaseline = baseline;
    }

    /**
     * Gets the base previously set using setBaseline.
     *
     * @return The base value.
     *
     * @see setBaseline
     */
    FORCE_INLINE_FUNCTION int getBaselineAsInt() const
    {
        return AbstractDataGraph::scaled2int(getBaselineScaled(), dataScale);
    }

    /** @copydoc getBaselineAsInt() */
    FORCE_INLINE_FUNCTION float getBaselineAsFloat() const
    {
        return AbstractDataGraph::scaled2float(getBaselineScaled(), dataScale);
    }

    /**
     * @copydoc getBaselineAsInt()
     *
     * @note The baseline returned here is left unscaled.
     */
    FORCE_INLINE_FUNCTION int getBaselineScaled() const
    {
        return yBaseline;
    }

    /**
     * Sets bar width of each histogram column in pixels.
     *
     * @param  width The width.
     *
     * @see getBarWidth
     */
    FORCE_INLINE_FUNCTION void setBarWidth(uint16_t width)
    {
        barWidth = width;
    }

    /**
     * Gets bar width of the histogram columns.
     *
     * @return The bar width.
     *
     * @see setBarWidth
     */
    FORCE_INLINE_FUNCTION uint16_t getBarWidth() const
    {
        return barWidth;
    }

    /**
     * Sets bar offset (horizontally) in pixels. This can be used when there are two different histogram
     * graphs on top of each other to prevent one histogram from covering the other.
     *
     * @param  offset The offset.
     *
     * @see getBarOffset
     */
    FORCE_INLINE_FUNCTION void setBarOffset(int16_t offset)
    {
        barOffset = offset;
    }

    /**
     * Gets bar offset (horizontally). Bar offset can be used when there are two different
     * histogram graphs on top of each other to prevent one histogram from covering the other.
     *
     * @return The bar offset.
     *
     * @see setBarOffset
     */
    FORCE_INLINE_FUNCTION int16_t getBarOffset() const
    {
        return barOffset;
    }

    virtual void draw(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    int dataScale;     ///< The scaling factor
    int yBaseline;     ///< The baseline
    uint16_t barWidth; ///< Width of each bar
    int16_t barOffset; ///< The horizontal bar offset
};

/**
 * GraphElementBoxes will draw square box for every data point in graph.
 *
 * @note The boxes are drawn using LCD::fillRect for higher performance. This also means that
 *       boxes with an odd width will not align properly if combined with a GraphElementLine or
 *       any other GraphElement that uses Canvas Widget Renderer. Use an even number for box width
 *       in these cases.
 */
class GraphElementBoxes : public AbstractGraphElementNoCWR
{
public:
    GraphElementBoxes()
        : boxWidth(2)
    {
    }

    /**
     * Sets box width.
     *
     * @param  width The width.
     *
     * @see getBoxWidth
     */
    FORCE_INLINE_FUNCTION void setBoxWidth(uint16_t width)
    {
        boxWidth = width;
    }

    /**
     * Gets box width.
     *
     * @return The box width.
     *
     * @see setBoxWidth
     */
    FORCE_INLINE_FUNCTION uint16_t getBoxWidth() const
    {
        return boxWidth;
    }

    virtual void draw(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint16_t boxWidth; ///< Width of the box
};

/**
 * GraphElementDots will draw a circular dot for every data point in graph.
 *
 * @note The Dots are drawn using Canvas Widget Renderer which is slower but produces much nicer
 *       graphics.
 */
class GraphElementDots : public AbstractGraphElement
{
public:
    GraphElementDots()
        : dotWidth(2)
    {
    }

    /**
     * Sets dot width.
     *
     * @param  width The width.
     *
     * @see getDotWidth
     */
    FORCE_INLINE_FUNCTION void setDotWidth(uint8_t width)
    {
        dotWidth = width;
    }

    /**
     * Gets dot width.
     *
     * @return The dot width.
     *
     * @see setDotWidth
     */
    FORCE_INLINE_FUNCTION uint8_t getDotWidth() const
    {
        return dotWidth;
    }

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint8_t dotWidth; ///< Width of the dot
};

/**
 * GraphElementDiamonds will draw a diamond (a square with the corners up/down/left/right) for
 * every data point in graph.
 *
 * @note The Diamonds are drawn using Canvas Widget Renderer which is slower but produces much
 *       nicer graphics.
 */
class GraphElementDiamonds : public AbstractGraphElement
{
public:
    GraphElementDiamonds()
        : diamondWidth(2)
    {
    }

    /**
     * Sets diamond width.
     *
     * @param  width The width.
     *
     * @see getDiamondWidth
     */
    FORCE_INLINE_FUNCTION void setDiamondWidth(uint8_t width)
    {
        diamondWidth = width;
    }

    /**
     * Gets diamond width.
     *
     * @return The diamond width.
     *
     * @see setDiamondWidth
     */
    FORCE_INLINE_FUNCTION uint8_t getDiamondWidth() const
    {
        return diamondWidth;
    }

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint8_t diamondWidth; ///< Width of the diamond
};

} // namespace touchgfx

#endif // TOUCHGFX_GRAPHELEMENTS_HPP
