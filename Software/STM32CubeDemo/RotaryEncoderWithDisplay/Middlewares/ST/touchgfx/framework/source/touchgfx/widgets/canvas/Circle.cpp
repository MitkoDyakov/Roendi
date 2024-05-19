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
#include <touchgfx/widgets/canvas/Circle.hpp>

namespace touchgfx
{
Circle::Circle()
    : CanvasWidget(),
      circleCenterX(0), circleCenterY(0), circleRadius(0),
      circleArcAngleStart(CWRUtil::toQ5<int>(0)), circleArcAngleEnd(CWRUtil::toQ5<int>(360)),
      circleLineWidth(0), circleArcIncrement(5),
      circleCapArcIncrement(180)
{
    Drawable::setWidthHeight(0, 0);
}

void Circle::setPrecision(int precision)
{
    if (precision < 1)
    {
        precision = 1;
    }
    if (precision > 120)
    {
        precision = 120;
    }
    circleArcIncrement = precision;
}

int Circle::getPrecision() const
{
    return circleArcIncrement;
}

void Circle::setCapPrecision(int precision)
{
    if (precision < 1)
    {
        precision = 1;
    }
    if (precision > 180)
    {
        precision = 180;
    }
    circleCapArcIncrement = precision;
}

int Circle::getCapPrecision() const
{
    return circleCapArcIncrement;
}

bool Circle::drawCanvasWidget(const Rect& invalidatedArea) const
{
    CWRUtil::Q5 arcStart = circleArcAngleStart;
    CWRUtil::Q5 arcEnd = circleArcAngleEnd;

    CWRUtil::Q5 const _360 = CWRUtil::toQ5<int>(360);

    // Put start before end by swapping
    if (arcStart > arcEnd)
    {
        CWRUtil::Q5 const tmp = arcStart;
        arcStart = arcEnd;
        arcEnd = tmp;
    }

    if ((arcEnd - arcStart) >= _360)
    {
        // The entire circle has to be drawn
        arcStart = CWRUtil::toQ5<int>(0);
        arcEnd = _360;
    }

    const int16_t circleCenterX16 = circleCenterX.to<int16_t>();
    const int16_t circleCenterY16 = circleCenterY.to<int16_t>();
    int32_t routside = 0;
    if (circleLineWidth != 0)
    {
        // Check if invalidated area is completely inside the circle
        const int32_t dx1 = abs((int)circleCenterX16 - invalidatedArea.x); // Find distances between each corner and circle center
        const int32_t dx2 = abs((int)circleCenterX16 - invalidatedArea.right());
        const int32_t dy1 = abs((int)circleCenterY16 - invalidatedArea.y);
        const int32_t dy2 = abs((int)circleCenterY16 - invalidatedArea.bottom());
        const int32_t dx = MAX(dx1, dx2) + 1; // Largest horz/vert distance (round up)
        const int32_t dy = MAX(dy1, dy2) + 1;
        // From https://www.mathopenref.com/polygonincircle.html
        const int32_t rmin = ((circleRadius - (circleLineWidth / 2)) * CWRUtil::cosine((circleArcIncrement + 1) / 2)).to<int>();
        // Check if invalidatedArea is completely inside circle
        if ((dx * dx) + (dy * dy) < rmin * rmin)
        {
            return true;
        }
        routside = (circleRadius + (circleLineWidth / 2)).to<int>() + 1;
    }
    else
    {
        routside = circleRadius.to<int>() + 1;
    }
    // From https://yal.cc/rectangle-circle-intersection-test/
    const int32_t invalidatedRight = invalidatedArea.right();
    const int32_t minimumX = MIN(circleCenterX16, invalidatedRight);
    const int32_t nearestX = circleCenterX16 - MAX(invalidatedArea.x, minimumX);
    const int32_t invalidatedBottom = invalidatedArea.bottom();
    const int32_t minimumY = MIN(circleCenterY16, invalidatedBottom);
    const int32_t nearestY = circleCenterY16 - MAX(invalidatedArea.y, minimumY);
    // Check if invalidatedArea is completely outside circle
    if ((nearestX * nearestX) + (nearestY * nearestY) > routside * routside)
    {
        return true;
    }

    Canvas canvas(getPainter(), getAbsoluteRect(), invalidatedArea, getAlpha());

    CWRUtil::Q5 radius = circleRadius;
    CWRUtil::Q5 lineWidth = circleLineWidth;
    if (circleLineWidth > circleRadius * 2)
    {
        lineWidth = (circleRadius + circleLineWidth / 2);
        radius = lineWidth / 2;
    }

    CWRUtil::Q5 arc = arcStart;
    CWRUtil::Q5 const circleArcIncrementQ5 = CWRUtil::toQ5<int>(circleArcIncrement);
    moveToAngleRadius2(canvas, arc, (radius * 2) + lineWidth);
    CWRUtil::Q5 nextArc = CWRUtil::Q5(ROUNDUP((int)(arc + CWRUtil::toQ5<int>(1)), (int)circleArcIncrementQ5));
    while (nextArc <= arcEnd)
    {
        arc = nextArc;
        lineToAngleRadius2(canvas, arc, (radius * 2) + lineWidth);
        nextArc += circleArcIncrementQ5;
    }
    if (arc < arcEnd)
    {
        // "arc" is not updated. It is the last arc in steps of "circleArcIncrement"
        lineToAngleRadius2(canvas, arcEnd, (radius * 2) + lineWidth);
    }

    if (lineWidth == CWRUtil::toQ5<int>(0))
    {
        // Draw a filled circle / pie / pacman
        if (arcEnd - arcStart < _360)
        {
            // Not a complete circle, line to center
            canvas.lineTo(circleCenterX, circleCenterY);
        }
    }
    else
    {
        if (canvas.wasOutlineTooComplex())
        {
            return false;
        }

        CWRUtil::Q5 const circleCapArcIncrementQ5 = CWRUtil::toQ5<int>(circleCapArcIncrement);
        CWRUtil::Q5 const _180 = CWRUtil::toQ5<int>(180);
        if (arcEnd - arcStart < _360)
        {
            // Draw the circle cap
            CWRUtil::Q5 const capX = circleCenterX + (radius * CWRUtil::sine(arcEnd));
            CWRUtil::Q5 const capY = circleCenterY - (radius * CWRUtil::cosine(arcEnd));
            for (CWRUtil::Q5 capAngle = arcEnd + circleCapArcIncrementQ5; capAngle < arcEnd + _180; capAngle += circleCapArcIncrementQ5)
            {
                lineToXYAngleRadius2(canvas, capX, capY, capAngle, lineWidth);
            }
        }

        // Not a filled circle, draw the path on the inside of the circle
        if (arc < arcEnd)
        {
            lineToAngleRadius2(canvas, arcEnd, (radius * 2) - lineWidth);
        }

        nextArc = arc;
        while (nextArc >= arcStart)
        {
            arc = nextArc;
            lineToAngleRadius2(canvas, arc, (radius * 2) - lineWidth);
            nextArc -= circleArcIncrementQ5;
        }

        if (arc > arcStart)
        {
            lineToAngleRadius2(canvas, arcStart, (radius * 2) - lineWidth);
        }

        if (arcEnd - arcStart < _360)
        {
            // Draw the circle cap
            CWRUtil::Q5 const capX = circleCenterX + (radius * CWRUtil::sine(arcStart));
            CWRUtil::Q5 const capY = circleCenterY - (radius * CWRUtil::cosine(arcStart));
            for (CWRUtil::Q5 capAngle = (arcStart - _180) + circleCapArcIncrementQ5; capAngle < arcStart; capAngle += circleCapArcIncrementQ5)
            {
                lineToXYAngleRadius2(canvas, capX, capY, capAngle, lineWidth);
            }
        }
    }

    return canvas.render();
}

Rect Circle::getMinimalRect(CWRUtil::Q5 arcStart, CWRUtil::Q5 arcEnd) const
{
    CWRUtil::Q5 xMin = CWRUtil::toQ5<int>(getWidth());
    CWRUtil::Q5 xMax = CWRUtil::toQ5<int>(0);
    CWRUtil::Q5 yMin = CWRUtil::toQ5<int>(getHeight());
    CWRUtil::Q5 yMax = CWRUtil::toQ5<int>(0);
    calculateMinimalRect(arcStart, arcEnd, xMin, xMax, yMin, yMax);
    return Rect(xMin.to<int>() - 1, yMin.to<int>() - 1,
                (xMax.to<int>() - xMin.to<int>()) + 2, (yMax.to<int>() - yMin.to<int>()) + 2);
}

void Circle::updateArc(CWRUtil::Q5 setStartAngleQ5, CWRUtil::Q5 setEndAngleQ5)
{
    CWRUtil::Q5 startAngleQ5 = setStartAngleQ5;
    CWRUtil::Q5 endAngleQ5 = setEndAngleQ5;
    if (circleArcAngleStart == startAngleQ5 && circleArcAngleEnd == endAngleQ5)
    {
        return;
    }

    // Make sure old start < end
    if (circleArcAngleStart > circleArcAngleEnd)
    {
        CWRUtil::Q5 const tmp = circleArcAngleStart;
        circleArcAngleStart = circleArcAngleEnd;
        circleArcAngleEnd = tmp;
    }
    // Make sure new start < end
    if (startAngleQ5 > endAngleQ5)
    {
        CWRUtil::Q5 const tmp = startAngleQ5;
        startAngleQ5 = endAngleQ5;
        endAngleQ5 = tmp;
    }

    // Nice constant
    const CWRUtil::Q5 _360 = CWRUtil::toQ5<int>(360);

    // Get old circle range start in [0..360[
    if (circleArcAngleStart >= _360)
    {
        const int x = (circleArcAngleStart / _360).to<int>();
        circleArcAngleStart -= _360 * x;
        circleArcAngleEnd -= _360 * x;
    }
    else if (circleArcAngleStart < 0)
    {
        const int x = 1 + ((-circleArcAngleStart) / _360).to<int>();
        circleArcAngleStart += _360 * x;
        circleArcAngleEnd += _360 * x;
    }
    // Detect full circle
    if ((circleArcAngleEnd - circleArcAngleStart) > _360)
    {
        circleArcAngleEnd = circleArcAngleStart + _360;
    }

    // Get new circle range start in [0..360[
    if (startAngleQ5 >= _360)
    {
        const int x = (startAngleQ5 / _360).to<int>();
        startAngleQ5 -= _360 * x;
        endAngleQ5 -= _360 * x;
    }
    else if (startAngleQ5 < 0)
    {
        const int x = 1 + (-startAngleQ5 / _360).to<int>();
        startAngleQ5 += _360 * x;
        endAngleQ5 += _360 * x;
    }
    // Detect full circle
    if ((endAngleQ5 - startAngleQ5) >= _360)
    {
        // Align full new circle with old start.
        // So old[90..270] -> new[0..360] becomes new[90..450] for smaller invalidated area
        startAngleQ5 = circleArcAngleStart;
        endAngleQ5 = startAngleQ5 + _360;
    }
    else if ((circleArcAngleEnd - circleArcAngleStart) >= _360)
    {
        // New circle is not full, but old is. Align old circle with new.
        // So old[0..360] -> new[90..270] becomes old[90..450] for smaller invalidated area
        circleArcAngleStart = startAngleQ5;
        circleArcAngleEnd = circleArcAngleStart + _360;
    }

    // New start is after old end. Could be overlap
    // if old[10..30]->new[350..380] becomes new[-10..20]
    if (startAngleQ5 > circleArcAngleEnd && endAngleQ5 - _360 >= circleArcAngleStart)
    {
        startAngleQ5 -= _360;
        endAngleQ5 -= _360;
    }
    // Same as above but for old instead of new
    if (circleArcAngleStart > endAngleQ5 && circleArcAngleEnd - _360 >= startAngleQ5)
    {
        circleArcAngleStart -= _360;
        circleArcAngleEnd -= _360;
    }

    Rect r;
    if (startAngleQ5 > circleArcAngleEnd || endAngleQ5 < circleArcAngleStart)
    {
        // Arcs do not overlap. Invalidate both arcs.
        r = getMinimalRect(circleArcAngleStart, circleArcAngleEnd);
        invalidateRect(r);

        r = getMinimalRect(startAngleQ5, endAngleQ5);
        invalidateRect(r);
    }
    else
    {
        // Arcs overlap. Invalidate both ends.
        if (circleArcAngleStart != startAngleQ5)
        {
            r = getMinimalRectForUpdatedStartAngle(startAngleQ5);
            invalidateRect(r);
        }
        if (circleArcAngleEnd != endAngleQ5)
        {
            r = getMinimalRectForUpdatedEndAngle(endAngleQ5);
            invalidateRect(r);
        }
    }

    circleArcAngleStart = setStartAngleQ5;
    circleArcAngleEnd = setEndAngleQ5;
}

void Circle::calculateMinimalRect(CWRUtil::Q5 arcStart, CWRUtil::Q5 arcEnd, CWRUtil::Q5& xMin, CWRUtil::Q5& xMax, CWRUtil::Q5& yMin, CWRUtil::Q5& yMax) const
{
    // Put start before end by swapping
    if (arcStart > arcEnd)
    {
        CWRUtil::Q5 const tmp = arcStart;
        arcStart = arcEnd;
        arcEnd = tmp;
    }

    CWRUtil::Q5 const _90 = CWRUtil::toQ5<int>(90);
    CWRUtil::Q5 const _360 = CWRUtil::toQ5<int>(360);

    if ((arcEnd - arcStart) >= _360)
    {
        // The entire circle has to be drawn
        arcStart = CWRUtil::toQ5<int>(0);
        arcEnd = _360;
    }

    if (circleLineWidth == 0)
    {
        // Check start angle
        updateMinMaxAngleSolid(arcStart, xMin, xMax, yMin, yMax);
        // Here we have a up to 4 approximation steps on angles divisible by 90
        CWRUtil::Q5 i;
        for (i = CWRUtil::Q5(ROUNDUP((int)(arcStart + (CWRUtil::Q5)1), (int)_90)); i <= arcEnd; i += _90)
        {
            updateMinMaxAngleSolid(i, xMin, xMax, yMin, yMax);
        }
        // Check end angle
        if ((i - _90) < arcEnd)
        {
            updateMinMaxAngleSolid(arcEnd, xMin, xMax, yMin, yMax);
        }
        updateMinMaxX(circleCenterX, xMin, xMax);
        updateMinMaxY(circleCenterY, yMin, yMax);
    }
    else
    {
        const CWRUtil::Q5 halfLineWidth = (circleLineWidth + (CWRUtil::Q5)1) / 2;
        // Check start angle
        updateMinMaxAngleLine(arcStart, halfLineWidth, xMin, xMax, yMin, yMax);
        // Here we have a up to 4 approximation steps on angles divisible by 90
        CWRUtil::Q5 i;
        for (i = CWRUtil::Q5(ROUNDUP((int)(arcStart + (CWRUtil::Q5)1), (int)_90)); i <= arcEnd; i += _90)
        {
            updateMinMaxAngleLine(i, halfLineWidth, xMin, xMax, yMin, yMax);
        }
        // Check end angle
        if ((i - _90) < arcEnd)
        {
            updateMinMaxAngleLine(arcEnd, halfLineWidth, xMin, xMax, yMin, yMax);
        }
    }
}

Rect Circle::getMinimalRectForUpdatedStartAngle(const CWRUtil::Q5& startAngleQ5) const
{
    CWRUtil::Q5 minAngle = CWRUtil::Q5(0); // Unused default value
    CWRUtil::Q5 maxAngle = CWRUtil::Q5(0); // Unused default value
    const int circleArcIncrementQ5int = (int)CWRUtil::toQ5<int>(circleArcIncrement);
    if (circleArcAngleStart < circleArcAngleEnd)
    {
        // start is smaller than end
        if (startAngleQ5 < circleArcAngleStart)
        {
            // start moved even lower
            minAngle = startAngleQ5;
            maxAngle = CWRUtil::Q5(ROUNDUP((int)circleArcAngleStart, circleArcIncrementQ5int));
            maxAngle = MIN(maxAngle, circleArcAngleEnd); // No need to go higher than end
        }
        else if (startAngleQ5 < circleArcAngleEnd)
        {
            // start moved higher, but not higher than end
            minAngle = circleArcAngleStart;
            maxAngle = CWRUtil::Q5(ROUNDUP((int)startAngleQ5, circleArcIncrementQ5int));
            maxAngle = MIN(maxAngle, circleArcAngleEnd); // No need to go higher than end
        }
        else
        {
            // start moved past end
            minAngle = circleArcAngleStart;
            maxAngle = startAngleQ5;
        }
    }
    else
    {
        // start is higher than end
        if (startAngleQ5 > circleArcAngleStart)
        {
            // start moved even higher
            minAngle = CWRUtil::Q5(ROUNDDOWN((int)circleArcAngleStart, circleArcIncrementQ5int));
            minAngle = MAX(minAngle, circleArcAngleEnd); // No need to go lower then end
            maxAngle = startAngleQ5;
        }
        else if (startAngleQ5 > circleArcAngleEnd)
        {
            // start moved lower, but not lower than end
            minAngle = CWRUtil::Q5(ROUNDDOWN((int)startAngleQ5, circleArcIncrementQ5int));
            minAngle = MAX(minAngle, circleArcAngleEnd); // No need to go lower than end
            maxAngle = circleArcAngleStart;
        }
        else
        {
            // start moved lower past end
            minAngle = startAngleQ5;
            maxAngle = circleArcAngleStart;
        }
    }
    return getMinimalRect(minAngle, maxAngle);
}

Rect Circle::getMinimalRectForUpdatedEndAngle(const CWRUtil::Q5& endAngleQ5) const
{
    CWRUtil::Q5 minAngle = CWRUtil::Q5(0); // Unused default value
    CWRUtil::Q5 maxAngle = CWRUtil::Q5(0); // Unused default value
    const int circleArcIncrementQ5int = (int)CWRUtil::toQ5<int>(circleArcIncrement);
    if (circleArcAngleStart < circleArcAngleEnd)
    {
        // start is smaller than end
        if (endAngleQ5 > circleArcAngleEnd)
        {
            // end moved even higher
            minAngle = CWRUtil::Q5(ROUNDDOWN((int)circleArcAngleEnd, circleArcIncrementQ5int));
            minAngle = MAX(minAngle, circleArcAngleStart);
            maxAngle = endAngleQ5;
        }
        else if (endAngleQ5 > circleArcAngleStart)
        {
            // end moved lower, but not past start
            minAngle = CWRUtil::Q5(ROUNDDOWN((int)endAngleQ5, circleArcIncrementQ5int));
            minAngle = MAX(minAngle, circleArcAngleStart); // No need to go lower than start
            maxAngle = circleArcAngleEnd;
        }
        else
        {
            // end move past start
            minAngle = endAngleQ5;
            maxAngle = circleArcAngleEnd;
        }
    }
    else
    {
        // start is higher than end
        if (endAngleQ5 < circleArcAngleEnd)
        {
            // end moved even lower
            minAngle = endAngleQ5;
            maxAngle = CWRUtil::Q5(ROUNDUP((int)circleArcAngleEnd, circleArcIncrementQ5int));
            maxAngle = MIN(maxAngle, circleArcAngleStart); // No need to go higher than start
        }
        else if (endAngleQ5 < circleArcAngleStart)
        {
            // end moved higher, but not higher than start
            minAngle = circleArcAngleEnd;
            maxAngle = CWRUtil::Q5(ROUNDUP((int)endAngleQ5, circleArcIncrementQ5int));
            maxAngle = MIN(maxAngle, circleArcAngleStart);
        }
        else
        {
            // end moved past start
            minAngle = circleArcAngleEnd;
            maxAngle = endAngleQ5;
        }
    }
    return getMinimalRect(minAngle, maxAngle);
}
} // namespace touchgfx
