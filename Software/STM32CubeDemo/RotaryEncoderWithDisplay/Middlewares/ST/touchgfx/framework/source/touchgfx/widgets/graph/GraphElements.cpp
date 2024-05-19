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

#include <touchgfx/Drawable.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/widgets/graph/GraphElements.hpp>

namespace touchgfx
{

Rect AbstractGraphElement::rectFromQ5Coordinates(CWRUtil::Q5 screenXminQ5, CWRUtil::Q5 screenYminQ5, CWRUtil::Q5 screenXmaxQ5, CWRUtil::Q5 screenYmaxQ5) const
{
    if (screenXminQ5 > screenXmaxQ5)
    {
        const CWRUtil::Q5 tmp = screenXminQ5;
        screenXminQ5 = screenXmaxQ5;
        screenXmaxQ5 = tmp;
    }
    if (screenYminQ5 > screenYmaxQ5)
    {
        const CWRUtil::Q5 tmp = screenYminQ5;
        screenYminQ5 = screenYmaxQ5;
        screenYmaxQ5 = tmp;
    }
    const int16_t xMin = screenXminQ5.to<int>();
    const int16_t yMin = screenYminQ5.to<int>();
    const int16_t xMax = screenXmaxQ5.ceil();
    const int16_t yMax = screenYmaxQ5.ceil();
    return Rect(xMin, yMin, xMax - xMin, yMax - yMin);
}

Rect AbstractGraphElement::rectAround(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5, CWRUtil::Q5 diameterQ5) const
{
    const CWRUtil::Q5 d2Q5 = CWRUtil::Q5((int32_t)diameterQ5 + 1) / 2;
    return rectFromQ5Coordinates(xQ5 - d2Q5, yQ5 - d2Q5, xQ5 + d2Q5, yQ5 + d2Q5);
}

bool AbstractGraphElement::isCenterInvisible(const AbstractDataGraph* graph, int16_t index) const
{
    const int16_t screenXCenter = indexToScreenXQ5(graph, index).round();
    const int16_t screenYCenter = indexToScreenYQ5(graph, index).round();
    return screenXCenter < graph->getGraphAreaPaddingLeft() ||
           screenXCenter >= graph->getGraphAreaPaddingLeft() + graph->getGraphAreaWidth() ||
           screenYCenter < graph->getGraphAreaPaddingTop() ||
           screenYCenter >= graph->getGraphAreaPaddingTop() + graph->getGraphAreaHeight();
}

void AbstractGraphElementNoCWR::normalizeRect(Rect& r) const
{
    if (r.width < 0)
    {
        r.x += r.width;
        r.width = -r.width;
    }
    if (r.height < 0)
    {
        r.y += r.height;
        r.height = -r.height;
    }
}

void GraphElementGridBase::draw(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    const uint8_t a = LCD::div255(getAlpha() * graph->getAlpha());
    if (lineWidth == 0 || a == 0)
    {
        return;
    }

    const Rect invalidRect = enclosingArea(graph) & invalidatedArea;

    const int minorInterval = getCorrectlyScaledGridInterval(graph);
    const int majorInterval = getCorrectlyScaledMajorInterval(graph);

    const CWRUtil::Q5 lw2Q5minor = CWRUtil::toQ5(lineWidth) / 2;
    const int16_t start = getGraphAreaStart(graph);
    const int16_t length = getGraphAreaLength(graph);
    if (minorInterval == 0 && majorInterval == 0)
    {
        const int16_t pos = (valueToScreenQ5(graph, 0) - lw2Q5minor).round();
        const int16_t graphPos = pos - getGraphAreaStartPos(graph);
        if (graphPos + lineWidth >= 0 && graphPos < getGraphAreaEndPos(graph))
        {
            drawLine(invalidRect, start, length, pos, lineWidth, a);
        }
    }
    else if (minorInterval > 0)
    {
        int rangeMin = getGraphRangeMinScaled(graph);
        int rangeMax = getGraphRangeMaxScaled(graph);
        if (abs(rangeMax - rangeMin) / minorInterval > 500)
        {
            return; // Too many grid lines
        }

        if (valueToScreenQ5(graph, rangeMin) > valueToScreenQ5(graph, rangeMax))
        {
            const int tmp = rangeMin;
            rangeMin = rangeMax;
            rangeMax = tmp;
        }
        // Now rangeMax has a higher screen coordinate than rangeMin

        const int margin = rangeMin < rangeMax ? +1 : -1;
        const int minorLo = (rangeMin / minorInterval) - margin;
        const int minorHi = (rangeMax / minorInterval) + margin;
        const int minorStep = minorLo < minorHi ? +1 : -1;
        if (majorInterval == 0)
        {
            // No major lines, simply draw all lines
            for (int minorIndex = minorLo; minorIndex != minorHi + minorStep; minorIndex += minorStep)
            {
                const int16_t pos = (valueToScreenQ5(graph, minorInterval * minorIndex) - lw2Q5minor).round();
                const int16_t graphPos = pos - getGraphAreaStartPos(graph);
                if (graphPos + lineWidth >= 0 && graphPos < getGraphAreaEndPos(graph))
                {
                    drawLine(invalidRect, start, length, pos, lineWidth, a);
                }
            }
        }
        else
        {
            const int majorLo = (rangeMin / majorInterval) - margin;
            const int majorHi = (rangeMax / majorInterval) + margin;
            const int majorStep = majorLo < majorHi ? +1 : -1;
            int majorIndex = majorLo;
            int16_t majorCoord = valueToScreenQ5(graph, majorInterval * majorIndex).round();
            int minorIndex = minorLo;
            int16_t minorCoord = valueToScreenQ5(graph, minorInterval * minorIndex).round();
            for (;;)
            {
                // Draw minor lines up to the major line
                while (minorCoord < majorCoord)
                {
                    const int16_t pos = (valueToScreenQ5(graph, minorInterval * minorIndex) - lw2Q5minor).round();
                    const int16_t graphPos = pos - getGraphAreaStartPos(graph);
                    if (graphPos + lineWidth >= 0 && graphPos < getGraphAreaEndPos(graph))
                    {
                        drawLine(invalidRect, start, length, pos, lineWidth, a);
                    }
                    minorIndex += minorStep;
                    minorCoord = valueToScreenQ5(graph, minorInterval * minorIndex).round();
                }
                // Advance minor past the major line we are about to draw
                while (minorCoord <= majorCoord)
                {
                    minorIndex += minorStep;
                    minorCoord = valueToScreenQ5(graph, minorInterval * minorIndex).round();
                }
                if (majorCoord <= minorCoord)
                {
                    majorIndex += majorStep;
                    if (majorIndex == majorHi + majorStep)
                    {
                        break;
                    }
                    majorCoord = valueToScreenQ5(graph, majorInterval * majorIndex).round();
                }
            }
        }
    }
}

void GraphElementGridBase::drawLine(const Rect& invalidatedArea, int16_t start, int16_t length, int16_t pos, int16_t width, uint8_t a) const
{
    Rect r = lineRect(start, length, pos, width);
    normalizeRect(r);
    r &= invalidatedArea;
    if (!r.isEmpty())
    {
        translateRectToAbsolute(r);
        HAL::lcd().fillRect(r, color, a);
    }
}

bool GraphElementArea::drawCanvasWidget(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    if (graph->getUsedCapacity() <= 1)
    {
        return true; // Nothing to draw, everything is fine!
    }

    int16_t indexMin;
    int16_t indexMax;
    if (!xScreenRangeToIndexRange(graph, invalidatedArea.x, invalidatedArea.right(), indexMin, indexMax))
    {
        return true; // Nothing to draw, everything is fine!
    }

    const int16_t gapIndex = graph->getGapBeforeIndex();
    const int baseline = convertToGraphScaleY(graph, yBaseline, dataScale);
    const CWRUtil::Q5 screenYbaseQ5 = roundQ5(valueToScreenYQ5(graph, baseline));
    CWRUtil::Q5 screenXQ5;
    if (indexMin + 1 == gapIndex)
    {
        if (indexMin > 0)
        {
            indexMin--; // Draw the last line segment before the gap
        }
        else
        {
            indexMin++; // Do not draw a 1 segment line (a "dot")
        }
    }

    const Rect invalidRect = Rect(graph->getGraphAreaPaddingLeft(), graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidth(), graph->getGraphAreaHeight()) & invalidatedArea;

    Canvas canvas(getPainter(), getAbsoluteRect(), invalidRect, getAlpha());

    canvas.moveTo(roundQ5(indexToScreenXQ5(graph, indexMin)), screenYbaseQ5);
    for (int16_t index = indexMin; index <= indexMax; index++)
    {
        if (index == gapIndex)
        {
            canvas.lineTo(screenXQ5, screenYbaseQ5);
            screenXQ5 = roundQ5(indexToScreenXQ5(graph, index));
            canvas.lineTo(screenXQ5, screenYbaseQ5);
        }
        else
        {
            screenXQ5 = roundQ5(indexToScreenXQ5(graph, index));
        }
        canvas.lineTo(screenXQ5, roundQ5(indexToScreenYQ5(graph, index)));
    }
    canvas.lineTo(screenXQ5, screenYbaseQ5);
    return canvas.render(graph->getAlpha());
}

void GraphElementArea::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    const int baseline = convertToGraphScaleY(graph, yBaseline, dataScale);
    const CWRUtil::Q5 screenYbaseQ5 = roundQ5(valueToScreenYQ5(graph, baseline));
    CWRUtil::Q5 screenXminQ5 = indexToScreenXQ5(graph, index);
    CWRUtil::Q5 screenXmaxQ5 = screenXminQ5;
    CWRUtil::Q5 screenYminQ5 = indexToScreenYQ5(graph, index);
    CWRUtil::Q5 screenYmaxQ5 = screenYminQ5;

    if (index > 0)
    {
        screenXminQ5 = indexToScreenXQ5(graph, index - 1);
        const CWRUtil::Q5 screenYnewQ5 = indexToScreenYQ5(graph, index - 1);
        screenYminQ5 = MIN(screenYminQ5, screenYnewQ5);
        screenYmaxQ5 = MAX(screenYmaxQ5, screenYnewQ5);
    }
    if (index < graph->getUsedCapacity() - 1)
    {
        screenXmaxQ5 = indexToScreenXQ5(graph, index + 1);
        const CWRUtil::Q5 screenYnewQ5 = indexToScreenYQ5(graph, index + 1);
        screenYminQ5 = MIN(screenYminQ5, screenYnewQ5);
        screenYmaxQ5 = MAX(screenYmaxQ5, screenYnewQ5);
    }
    screenYminQ5 = MIN(screenYminQ5, screenYbaseQ5);
    screenYmaxQ5 = MAX(screenYmaxQ5, screenYbaseQ5);
    Rect dirty(rectFromQ5Coordinates(screenXminQ5, screenYminQ5, screenXmaxQ5, screenYmaxQ5));
    dirty &= Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight());
    invalidateRect(dirty);
}

bool GraphElementLine::drawCanvasWidget(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    if (graph->getUsedCapacity() <= 1)
    {
        return true; // Nothing to draw, everything is fine!
    }

    const CWRUtil::Q5 lineWidthQ5 = CWRUtil::toQ5(lineWidth);
    const uint16_t lineWidthHalf = CWRUtil::Q5(((int)lineWidthQ5 + 1) / 2).ceil();
    int16_t indexMin;
    int16_t indexMax;
    if (!xScreenRangeToIndexRange(graph, invalidatedArea.x - lineWidthHalf, invalidatedArea.right() + lineWidthHalf, indexMin, indexMax))
    {
        return true; // Nothing to draw, everything is fine!
    }

    const Rect invalidRect = Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight()) & invalidatedArea;

    Canvas canvas(getPainter(), getAbsoluteRect(), invalidRect, getAlpha());

    const int16_t gapIndex = graph->getGapBeforeIndex();
    if (gapIndex <= 0 || gapIndex <= indexMin || gapIndex > indexMax)
    {
        drawIndexRange(canvas, graph, indexMin, indexMax);
    }
    else
    {
        drawIndexRange(canvas, graph, indexMin, gapIndex - 1);
        drawIndexRange(canvas, graph, gapIndex, indexMax);
    }
    return canvas.render(graph->getAlpha());
}

void GraphElementLine::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    CWRUtil::Q5 const lineWidthQ5 = CWRUtil::toQ5(lineWidth);
    Rect dirty(rectAround(indexToScreenXQ5(graph, index), indexToScreenYQ5(graph, index), lineWidthQ5));
    if (index > 0)
    {
        const Rect other(rectAround(indexToScreenXQ5(graph, index - 1), indexToScreenYQ5(graph, index - 1), lineWidthQ5));
        dirty.expandToFit(other);
    }
    if (index < graph->getUsedCapacity() - 1)
    {
        const Rect other(rectAround(indexToScreenXQ5(graph, index + 1), indexToScreenYQ5(graph, index + 1), lineWidthQ5));
        dirty.expandToFit(other);
    }
    dirty &= Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight());
    invalidateRect(dirty);
}

void GraphElementLine::drawIndexRange(Canvas& canvas, const AbstractDataGraph* graph, int16_t indexMin, int16_t indexMax) const
{
    if (indexMin == indexMax)
    {
        return;
    }

    const CWRUtil::Q5 lineWidthQ5 = CWRUtil::toQ5(lineWidth);

    CWRUtil::Q5 screenXstartQ5 = roundQ5(indexToScreenXQ5(graph, indexMin));
    CWRUtil::Q5 screenYstartQ5 = roundQ5(indexToScreenYQ5(graph, indexMin));
    canvas.moveTo(screenXstartQ5, screenYstartQ5);
    int16_t index = indexMin;
    int16_t advance = 1;
    do
    {
        if (index == indexMax)
        {
            advance = -1;
        }
        index += advance;
        const CWRUtil::Q5 screenXendQ5 = roundQ5(indexToScreenXQ5(graph, index));
        const CWRUtil::Q5 screenYendQ5 = roundQ5(indexToScreenYQ5(graph, index));
        CWRUtil::Q5 dxQ5 = screenXendQ5 - screenXstartQ5;
        CWRUtil::Q5 dyQ5 = screenYendQ5 - screenYstartQ5;
        const CWRUtil::Q5 dQ5 = CWRUtil::length(dxQ5, dyQ5);
        if (dQ5)
        {
            dyQ5 = CWRUtil::muldivQ5(lineWidthQ5, dyQ5, dQ5) / 2;
            dxQ5 = CWRUtil::muldivQ5(lineWidthQ5, dxQ5, dQ5) / 2;
            canvas.lineTo(screenXstartQ5 - dyQ5, screenYstartQ5 + dxQ5);
            canvas.lineTo(screenXendQ5 - dyQ5, screenYendQ5 + dxQ5);
            screenXstartQ5 = screenXendQ5;
            screenYstartQ5 = screenYendQ5;
        }
    } while (index > indexMin);
}

void GraphElementVerticalGapLine::draw(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    const int16_t gapIndex = graph->getGapBeforeIndex();
    const uint8_t a = LCD::div255(getAlpha() * graph->getAlpha());
    if (gapIndex == 0 || a == 0)
    {
        return;
    }

    const int16_t screenXmin = indexToScreenXQ5(graph, gapIndex - 1).round();
    int16_t screenXmax = screenXmin + lineWidth;
    if (lineWidth == 0)
    {
        screenXmax = indexToScreenXQ5(graph, gapIndex).round();
    }
    Rect r(screenXmin, graph->getGraphAreaPaddingTop(), screenXmax - screenXmin, graph->getGraphAreaHeight());
    normalizeRect(r);
    r &= invalidatedArea;
    if (!r.isEmpty())
    {
        translateRectToAbsolute(r);
        HAL::lcd().fillRect(r, color, a);
    }
}

void GraphElementVerticalGapLine::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    invalidateIndex(graph, graph->getGapBeforeIndex());
    invalidateIndex(graph, index);
}

void GraphElementVerticalGapLine::invalidateIndex(const AbstractDataGraph* graph, int16_t index) const
{
    if (index > 0)
    {
        const int16_t screenXmin = indexToScreenXQ5(graph, index - 1).round();
        int16_t screenXmax = screenXmin + lineWidth;
        if (lineWidth == 0)
        {
            screenXmax = indexToScreenXQ5(graph, index).round();
        }
        Rect dirty(screenXmin, graph->getGraphAreaPaddingTop(), screenXmax - screenXmin, graph->getGraphAreaHeight());
        normalizeRect(dirty);
        invalidateRect(dirty);
    }
}

void GraphElementHistogram::draw(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    const uint8_t a = LCD::div255(getAlpha() * graph->getAlpha());
    if (graph->getUsedCapacity() == 0 || barWidth == 0 || a == 0)
    {
        return; // Nothing to draw, everything is fine!
    }

    const int baseline = convertToGraphScaleY(graph, yBaseline, dataScale);
    const CWRUtil::Q5 barOffsetQ5 = CWRUtil::toQ5(barOffset);
    const CWRUtil::Q5 barWidthQ5 = CWRUtil::toQ5(barWidth);
    const CWRUtil::Q5 barWidthHalfQ5 = CWRUtil::Q5(((int)barWidthQ5 + 1) / 2);
    const int16_t barWidthHalf = barWidthHalfQ5.ceil();
    const int16_t screenYzero = valueToScreenYQ5(graph, baseline).round();
    int16_t indexMin;
    int16_t indexMax;
    if (!xScreenRangeToIndexRange(graph, invalidatedArea.x + barOffset - barWidthHalf, invalidatedArea.right() + barOffset + barWidthHalf, indexMin, indexMax))
    {
        return; // Nothing to draw, everything is fine!
    }

    const Rect invalidRect = Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight()) & invalidatedArea;
    for (int16_t index = indexMin; index <= indexMax; index++)
    {
        const int16_t screenX = (indexToScreenXQ5(graph, index) + barOffsetQ5 - barWidthHalfQ5).round();
        const int16_t screenY = indexToScreenYQ5(graph, index).round();
        Rect r(screenX, screenY, barWidth, screenYzero - screenY);
        normalizeRect(r);
        r &= invalidRect;
        if (!r.isEmpty())
        {
            translateRectToAbsolute(r);
            HAL::lcd().fillRect(r, color, a);
        }
    }
}

void GraphElementHistogram::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    const CWRUtil::Q5 screenXQ5 = indexToScreenXQ5(graph, index);
    const CWRUtil::Q5 barWidthHalfQ5 = CWRUtil::Q5((int)CWRUtil::toQ5(barWidth) + 1) / 2;
    const CWRUtil::Q5 barOffsetQ5 = CWRUtil::toQ5(barOffset);
    const int baseline = convertToGraphScaleY(graph, yBaseline, dataScale);
    Rect dirty(rectFromQ5Coordinates(screenXQ5 + barOffsetQ5 - barWidthHalfQ5, indexToScreenYQ5(graph, index), screenXQ5 + barOffsetQ5 + barWidthHalfQ5, valueToScreenYQ5(graph, baseline)));
    dirty &= Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight());
    invalidateRect(dirty);
}

void GraphElementBoxes::draw(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    const uint8_t a = LCD::div255(getAlpha() * graph->getAlpha());
    if (graph->getUsedCapacity() == 0 || boxWidth == 0 || a == 0)
    {
        return; // Nothing to draw, everything is fine!
    }

    const CWRUtil::Q5 boxWidthQ5 = CWRUtil::toQ5(boxWidth);
    const CWRUtil::Q5 boxWidthHalfQ5 = boxWidthQ5 / 2;
    const uint16_t boxWidthHalf = CWRUtil::Q5(((int)boxWidthQ5 + 1) / 2).ceil();
    int16_t indexMin;
    int16_t indexMax;
    if (!xScreenRangeToIndexRange(graph, invalidatedArea.x - boxWidthHalf, invalidatedArea.right() + boxWidthHalf, indexMin, indexMax))
    {
        return; // Nothing to draw, everything is fine!
    }

    for (int16_t index = indexMin; index <= indexMax; index++)
    {
        if (isCenterInvisible(graph, index))
        {
            continue;
        }
        const CWRUtil::Q5 screenXQ5 = indexToScreenXQ5(graph, index);
        const CWRUtil::Q5 screenYQ5 = indexToScreenYQ5(graph, index);
        Rect r((screenXQ5 - boxWidthHalfQ5).round(), (screenYQ5 - boxWidthHalfQ5).round(), boxWidth, boxWidth);
        r &= invalidatedArea;
        if (!r.isEmpty())
        {
            translateRectToAbsolute(r);
            HAL::lcd().fillRect(r, color, a);
        }
    }
}

void GraphElementBoxes::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    if (isCenterInvisible(graph, index))
    {
        return;
    }
    const CWRUtil::Q5 boxWidthQ5 = CWRUtil::toQ5(boxWidth);
    const CWRUtil::Q5 boxWidthHalfQ5 = boxWidthQ5 / 2;
    const CWRUtil::Q5 screenXQ5 = indexToScreenXQ5(graph, index);
    const CWRUtil::Q5 screenYQ5 = indexToScreenYQ5(graph, index);
    Rect dirty((screenXQ5 - boxWidthHalfQ5).round(), (screenYQ5 - boxWidthHalfQ5).round(), boxWidth, boxWidth);
    invalidateRect(dirty);
}

bool GraphElementDots::drawCanvasWidget(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    if (graph->getUsedCapacity() == 0)
    {
        return true; // Nothing to draw, everything is fine!
    }

    const CWRUtil::Q5 dotWidthQ5 = CWRUtil::toQ5(dotWidth);
    const CWRUtil::Q5 dotWidth3Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(3), CWRUtil::toQ5(10));
    const CWRUtil::Q5 dotWidth4Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(4), CWRUtil::toQ5(10));
    const CWRUtil::Q5 dotWidth7Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(7), CWRUtil::toQ5(50));
    const CWRUtil::Q5 dotWidth24Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(24), CWRUtil::toQ5(50));
    const CWRUtil::Q5 dotWidth2Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(1), CWRUtil::toQ5(2));
    const uint16_t dotWidthHalf = CWRUtil::Q5(((int)dotWidthQ5 + 1) / 2).ceil(); // Round up
    int16_t indexMin;
    int16_t indexMax;
    if (!xScreenRangeToIndexRange(graph, invalidatedArea.x - dotWidthHalf, invalidatedArea.right() + dotWidthHalf, indexMin, indexMax))
    {
        return true;
    }

    const bool bigDots = (dotWidth > 6);

    Canvas canvas(getPainter(), getAbsoluteRect(), invalidatedArea, getAlpha());

    for (int16_t index = indexMin; index <= indexMax; index++)
    {
        if (isCenterInvisible(graph, index))
        {
            continue;
        }
        const CWRUtil::Q5 screenXcenterQ5 = roundQ5(indexToScreenXQ5(graph, index));
        const CWRUtil::Q5 screenYcenterQ5 = roundQ5(indexToScreenYQ5(graph, index));
        const Rect dirty(rectAround(screenXcenterQ5, screenYcenterQ5, dotWidthQ5) & invalidatedArea);
        if (!dirty.isEmpty())
        {
            if (bigDots)
            {
                canvas.moveTo(screenXcenterQ5 - dotWidth2Q5, screenYcenterQ5);
                canvas.lineTo(screenXcenterQ5 - dotWidth24Q5, screenYcenterQ5 - dotWidth7Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth4Q5, screenYcenterQ5 - dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth3Q5, screenYcenterQ5 - dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth7Q5, screenYcenterQ5 - dotWidth24Q5);
                canvas.lineTo(screenXcenterQ5, screenYcenterQ5 - dotWidth2Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth7Q5, screenYcenterQ5 - dotWidth24Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth3Q5, screenYcenterQ5 - dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth4Q5, screenYcenterQ5 - dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth24Q5, screenYcenterQ5 - dotWidth7Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth2Q5, screenYcenterQ5);
                canvas.lineTo(screenXcenterQ5 + dotWidth24Q5, screenYcenterQ5 + dotWidth7Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth4Q5, screenYcenterQ5 + dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth3Q5, screenYcenterQ5 + dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth7Q5, screenYcenterQ5 + dotWidth24Q5);
                canvas.lineTo(screenXcenterQ5, screenYcenterQ5 + dotWidth2Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth7Q5, screenYcenterQ5 + dotWidth24Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth3Q5, screenYcenterQ5 + dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth4Q5, screenYcenterQ5 + dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth24Q5, screenYcenterQ5 + dotWidth7Q5);
            }
            else
            {
                canvas.moveTo(screenXcenterQ5 - dotWidth2Q5, screenYcenterQ5);
                canvas.lineTo(screenXcenterQ5 - dotWidth4Q5, screenYcenterQ5 - dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth3Q5, screenYcenterQ5 - dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5, screenYcenterQ5 - dotWidth2Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth3Q5, screenYcenterQ5 - dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth4Q5, screenYcenterQ5 - dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth2Q5, screenYcenterQ5);
                canvas.lineTo(screenXcenterQ5 + dotWidth4Q5, screenYcenterQ5 + dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth3Q5, screenYcenterQ5 + dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5, screenYcenterQ5 + dotWidth2Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth3Q5, screenYcenterQ5 + dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth4Q5, screenYcenterQ5 + dotWidth3Q5);
            }
        }
    }
    return canvas.render(graph->getAlpha());
}

void GraphElementDots::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    if (isCenterInvisible(graph, index))
    {
        return;
    }
    Rect dirty(rectAround(indexToScreenXQ5(graph, index), indexToScreenYQ5(graph, index), CWRUtil::toQ5(dotWidth)));
    dirty &= Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight());
    invalidateRect(dirty);
}

bool GraphElementDiamonds::drawCanvasWidget(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    if (graph->getUsedCapacity() == 0)
    {
        return true; // Nothing to draw, everything is fine!
    }

    const CWRUtil::Q5 diamondWidthQ5 = CWRUtil::toQ5(diamondWidth);
    const CWRUtil::Q5 dotWidthHalfQ5 = diamondWidthQ5 / 2;
    const uint16_t dotWidthHalf = CWRUtil::Q5(((int)diamondWidthQ5 + 1) / 2).ceil(); // Round up
    int16_t indexMin;
    int16_t indexMax;
    if (!xScreenRangeToIndexRange(graph, invalidatedArea.x - dotWidthHalf, invalidatedArea.right() + dotWidthHalf, indexMin, indexMax))
    {
        return true;
    }

    Canvas canvas(getPainter(), getAbsoluteRect(), invalidatedArea, getAlpha());

    for (int16_t index = indexMin; index <= indexMax; index++)
    {
        if (isCenterInvisible(graph, index))
        {
            continue;
        }
        const CWRUtil::Q5 screenXcenterQ5 = roundQ5(indexToScreenXQ5(graph, index));
        const CWRUtil::Q5 screenYcenterQ5 = roundQ5(indexToScreenYQ5(graph, index));
        const Rect dirty(rectAround(screenXcenterQ5, screenYcenterQ5, diamondWidthQ5) & invalidatedArea);
        if (!dirty.isEmpty())
        {
            canvas.moveTo(screenXcenterQ5 - dotWidthHalfQ5, screenYcenterQ5);
            canvas.lineTo(screenXcenterQ5, screenYcenterQ5 - dotWidthHalfQ5);
            canvas.lineTo(screenXcenterQ5 + dotWidthHalfQ5, screenYcenterQ5);
            canvas.lineTo(screenXcenterQ5, screenYcenterQ5 + dotWidthHalfQ5);
        }
    }
    return canvas.render(graph->getAlpha());
}

void GraphElementDiamonds::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    if (isCenterInvisible(graph, index))
    {
        return;
    }
    Rect dirty(rectAround(indexToScreenXQ5(graph, index), indexToScreenYQ5(graph, index), CWRUtil::toQ5(diamondWidth)));
    invalidateRect(dirty);
}

} // namespace touchgfx
