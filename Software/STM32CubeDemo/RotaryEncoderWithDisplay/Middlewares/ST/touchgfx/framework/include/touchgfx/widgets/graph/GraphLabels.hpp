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
 * @file touchgfx/widgets/graph/GraphLabels.hpp
 *
 * Declares classes for adding labels and title to the graph.
 */
#ifndef TOUCHGFX_GRAPHLABELS_HPP
#define TOUCHGFX_GRAPHLABELS_HPP

#include <touchgfx/Font.hpp>
#include <touchgfx/TypedText.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>
#include <touchgfx/widgets/graph/GraphElements.hpp>

namespace touchgfx
{
/** Helper class used for adding labels around the graph. Currently empty. */
class AbstractGraphDecoration : public AbstractGraphElementNoCWR
{
};

/**
 * Helper class for adding labels on the side of a graph.
 *
 * @see GraphLabelsX, GraphLabelsY
 */
class GraphLabelsBase : public AbstractGraphDecoration
{
public:
    GraphLabelsBase()
        : AbstractGraphDecoration(),
          dataScale(1), labelInterval(0), labelTypedText(TYPED_TEXT_INVALID), labelRotation(TEXT_ROTATE_0), labelDecimals(0), labelDecimalPoint('.'), majorLabel(0)
    {
    }

    virtual void draw(const Rect& invalidatedArea) const;

    /**
     * Sets a scaling factor to be multiplied on the labelInterval. Any already set
     * labelInterval will be updated to the new correctly scaled value. To avoid this, use
     * setIntervalScaled after setting scale.
     *
     * @param  scale The scaling factor.
     *
     * @see getScale
     */
    FORCE_INLINE_FUNCTION void setScale(int scale)
    {
        assert(scale != 0);
        labelInterval = AbstractDataGraph::convertToNewScale(labelInterval, dataScale, scale);
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
     * Sets the interval between each label.
     *
     * @param  interval The interval between each label.
     *
     * @see getIntervalAsInt, getIntervalAsFloat, setMajorLabel
     *
     * @note If interval is 0 only the axis is shown.
     */
    FORCE_INLINE_FUNCTION void setInterval(int interval)
    {
        setIntervalScaled(AbstractDataGraph::int2scaled(interval, dataScale));
    }

    /** @copydoc setInterval(int) */
    FORCE_INLINE_FUNCTION void setInterval(float interval)
    {
        setIntervalScaled(AbstractDataGraph::float2scaled(interval, dataScale));
    }

    /**
     * @copydoc setInterval(int)
     *
     * @note The interval set here must already be scaled.
     */
    FORCE_INLINE_FUNCTION void setIntervalScaled(int interval)
    {
        labelInterval = abs(interval);
    }

    /**
     * Gets the interval between each label.
     *
     * @return The interval between each label.
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
        return labelInterval;
    }

    /**
     * Sets "major" label that will be responsible for drawing major labels. If a label would be
     * drawn at the same position as the major label, the label will not be drawn.
     *
     * @param  major Reference to a major label object.
     */
    FORCE_INLINE_FUNCTION void setMajorLabel(const GraphLabelsBase& major)
    {
        majorLabel = &major;
    }

    /**
     * Sets TypedText to use for the label. The TypedText should contain exactly one wildcard.
     *
     * @param  typedText The typed text.
     *
     * @see getLabelTypedText
     */
    FORCE_INLINE_FUNCTION void setLabelTypedText(const TypedText& typedText)
    {
        labelTypedText = typedText;
    }

    /**
     * Gets TypedText label.
     *
     * @return The label typed text.
     *
     * @see setLabelTypedText
     */
    FORCE_INLINE_FUNCTION const TypedText& getLabelTypedText() const
    {
        return labelTypedText;
    }

    /**
     * Sets label rotation.
     *
     * @param  rotation The rotation or the text.
     *
     * @see getLabelRotation
     */
    FORCE_INLINE_FUNCTION void setLabelRotation(TextRotation rotation)
    {
        labelRotation = rotation;
    }

    /**
     * Gets label rotation.
     *
     * @return The label rotation.
     *
     * @see setLabelRotation
     */
    FORCE_INLINE_FUNCTION TextRotation getLabelRotation() const
    {
        return labelRotation;
    }

    /**
     * Sets number of decimals for labels, default is no decimals and no decimal point.
     *
     * @param  decimals The number of label decimals.
     *
     * @see setLabelDecimalPoint
     */
    FORCE_INLINE_FUNCTION void setLabelDecimals(uint16_t decimals)
    {
        labelDecimals = decimals;
    }

    /**
     * Gets number of decimals for labels.
     *
     * @return The number of label decimals.
     */
    FORCE_INLINE_FUNCTION uint16_t getLabelDecimals() const
    {
        return labelDecimals;
    }

    /**
     * Sets label decimal point. Default is to use '.' but this can be changed using this
     * function.
     *
     * @param  decimalPoint The character to use for decimal point.
     *
     * @see setLabelDecimals
     *
     * @note The decimal point is only set if the label decimals > 0.
     */
    FORCE_INLINE_FUNCTION void setLabelDecimalPoint(Unicode::UnicodeChar decimalPoint)
    {
        labelDecimalPoint = decimalPoint;
    }

    /**
     * Gets label decimal point previously set.
     *
     * @return The label decimal point.
     *
     * @see setLabelDecimalPoint
     */
    FORCE_INLINE_FUNCTION Unicode::UnicodeChar getLabelDecimalPoint() const
    {
        return labelDecimalPoint;
    }

    virtual void invalidateGraphPointAt(int16_t)
    {
    }

    /**
     * Gets correctly scaled minor interval, as the minor label may have a scale that differs
     * the scale of the graph and this label.
     *
     * @param  graph The graph.
     *
     * @return The correctly scaled minor interval.
     */
    virtual int getCorrectlyScaledLabelInterval(const AbstractDataGraph* graph) const = 0;

    /**
     * Gets correctly scaled major interval, as the major label may have a scale that differs
     * the scale of the graph and this label.
     *
     * @param  graph The graph.
     *
     * @return The correctly scaled major interval.
     */
    virtual int getCorrectlyScaledMajorInterval(const AbstractDataGraph* graph) const
    {
        return majorLabel == 0 ? 0 : majorLabel->getCorrectlyScaledLabelInterval(graph);
    }

protected:
    int dataScale;                          ///< The scaling factor
    int labelInterval;                      ///< The interval between each label.
    TypedText labelTypedText;               ///< The TypedText to use for the label.
    TextRotation labelRotation;             ///< The TextRotation to use for the label.
    uint16_t labelDecimals;                 ///< The number of decimals on the label.
    Unicode::UnicodeChar labelDecimalPoint; ///< The label decimal point character.
    const GraphLabelsBase* majorLabel;      ///< A pointer to a major label, if any

    /**
     * Draw labels for all indexes in the given range. This is used where there is a gap in the
     * graph and the labels have to be drawn using different x scales.
     *
     * @param [in] invalidatedArea The canvas.
     * @param      fontToDraw      The font to draw.
     * @param      graph           The graph.
     * @param      rangeMin        The minimum index.
     * @param      rangeMax        The maximum index.
     * @param      minorInterval   The minor interval.
     * @param      majorInterval   The major interval.
     * @param      a               The alpha of the strings.
     */
    virtual void drawIndexRange(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, const int rangeMin, const int rangeMax, const int minorInterval, const int majorInterval, const uint8_t a) const;

    /**
     * Draw string.
     *
     * @param  invalidatedArea The invalidated area.
     * @param  fontToDraw      The font to draw.
     * @param  graph           The graph.
     * @param  valueScaled     The value scaled.
     * @param  labelScaled     The label scaled.
     * @param  a               The alpha of the string.
     */
    virtual void drawString(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, const int valueScaled, const int labelScaled, const uint8_t a) const = 0;

    /**
     * Gets graph range minimum scaled.
     *
     * @param  graph The graph.
     *
     * @return The graph range minimum scaled.
     */
    virtual int getGraphRangeMinScaled(const AbstractDataGraph* graph) const = 0;

    /**
     * Gets graph range maximum scaled.
     *
     * @param  graph The graph.
     *
     * @return The graph range maximum scaled.
     */
    virtual int getGraphRangeMaxScaled(const AbstractDataGraph* graph) const = 0;

    /**
     * Format label according to the set number of decimals and the decimal point.
     *
     * @param [in,out] buffer       The buffer to fill with the formatted number.
     * @param          bufferSize   Size of the buffer.
     * @param          label        The label value.
     * @param          decimals     The number of decimals.
     * @param          decimalPoint The decimal point.
     * @param          scale        The scale of the label value.
     */
    void formatLabel(Unicode::UnicodeChar* buffer, int16_t bufferSize, int label, int decimals, Unicode::UnicodeChar decimalPoint, int scale) const;
};

/**
 * GraphLabelsX will draw labels along the X axis at given intervals. By combining two
 * GraphLabelsX it is possible to have different appearance for major and minor y offsets.
 */
class GraphLabelsX : public GraphLabelsBase
{
public:
    virtual void invalidateGraphPointAt(int16_t index);

    virtual int getCorrectlyScaledLabelInterval(const AbstractDataGraph* graph) const
    {
        return convertToGraphScaleX(graph, labelInterval, dataScale);
    }

protected:
    void drawIndexRange(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, const int rangeMin, const int rangeMax, const int minorInterval, const int majorInterval, const uint8_t a) const;

    virtual void drawString(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, const int valueScaled, const int labelScaled, const uint8_t a) const;

    virtual int getGraphRangeMinScaled(const AbstractDataGraph* graph) const
    {
        return graph->getGraphRangeXMinScaled();
    }

    virtual int getGraphRangeMaxScaled(const AbstractDataGraph* graph) const
    {
        return graph->getGraphRangeXMaxScaled();
    }
};

/**
 * GraphLabelsY will draw labels along the Y axis at given intervals. By combining two
 * GraphLabelsY it is possible to have different appearance for major and minor y offsets.
 */
class GraphLabelsY : public GraphLabelsBase
{
public:
    virtual int getCorrectlyScaledLabelInterval(const AbstractDataGraph* graph) const
    {
        return convertToGraphScaleY(graph, labelInterval, dataScale);
    }

protected:
    virtual void drawString(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, const int valueScaled, const int labelScaled, const uint8_t a) const;

    virtual int getGraphRangeMinScaled(const AbstractDataGraph* graph) const
    {
        return graph->getGraphRangeYMinScaled();
    }

    virtual int getGraphRangeMaxScaled(const AbstractDataGraph* graph) const
    {
        return graph->getGraphRangeYMaxScaled();
    }
};

/**
 * The GraphTitle is just a simple text, but it is automatically moved with the graph. Also, the
 * alpha value is combined with the alpha of the graph and so it will be faded if the graph is
 * faded.
 */
class GraphTitle : public AbstractGraphDecoration
{
public:
    GraphTitle()
        : titleTypedText(TYPED_TEXT_INVALID), titleRotation(TEXT_ROTATE_0)
    {
    }

    /**
     * Sets TypedText to use as a title. It can be any static text which is just added as a
     * title.
     *
     * @param  typedText The typed text.
     *
     * @see getTitleTypedText
     */
    FORCE_INLINE_FUNCTION void setTitleTypedText(const TypedText& typedText)
    {
        titleTypedText = typedText;
    }

    /**
     * Gets title typed text.
     *
     * @return The title typed text.
     *
     * @see setTitleTypedText
     */
    FORCE_INLINE_FUNCTION const TypedText& getTitleTypedText() const
    {
        return titleTypedText;
    }

    /**
     * Sets TextRotation of the title.
     *
     * @param  rotation The rotation.
     *
     * @see setTitleTypedText, getTitleRotation
     */
    FORCE_INLINE_FUNCTION void setTitleRotation(TextRotation rotation)
    {
        titleRotation = rotation;
    }

    /**
     * Gets title rotation.
     *
     * @return The title rotation.
     *
     * @see setTitleRotation
     */
    FORCE_INLINE_FUNCTION TextRotation getTitleRotation() const
    {
        return titleRotation;
    }

    virtual void draw(const Rect& invalidatedArea) const;

    virtual bool drawCanvasWidget(const Rect&) const
    {
        return true;
    }

    virtual void invalidateGraphPointAt(int16_t)
    {
    }

private:
    TypedText titleTypedText;   ///< The title typed text
    TextRotation titleRotation; ///< The title rotation
};

} // namespace touchgfx

#endif // TOUCHGFX_GRAPHLABELS_HPP
