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

#include <math.h>
#include <touchgfx/widgets/canvas/CWRVectorRenderer.hpp>

namespace touchgfx
{
void CWRVectorRenderer::setup(const Rect& canvasAreaAbs, const Rect& invalidatedAreaRel)
{
    canvasAreaAbsolute = canvasAreaAbs;
    drawArea = invalidatedAreaRel;
    canvasPainter = 0;

    // Clear transformation matrix
    matrix.reset();

    // Alpha reset
    alpha = 255;
    colorAlpha = 255;

    // Stroke reset
    strokeWidth = 1.0f;
    strokeLineJoin = VG_STROKE_LINEJOIN_MITER;
    strokeLineCap = VG_STROKE_LINECAP_BUTT;
}

void CWRVectorRenderer::tearDown()
{
    // Clear drawing area to avoid drawing any paths until next setup
    drawArea = Rect();
    // Wait for the painter to finish
    if (canvasPainter)
    {
        canvasPainter->tearDown();
    }
}

void CWRVectorRenderer::drawPath(const uint8_t* cmds, uint32_t nCmds, const float* points, uint32_t nPoints, const float* /* bbox */)
{
    // Draw the path. Try reduced areas until success
    Rect area = drawArea;
    if (area.isEmpty())
    {
        return;
    }

    const int16_t bottom = area.bottom();
    while (area.y < bottom)
    {
        while (!drawPathArea(cmds, nCmds, points, nPoints, area))
        {
            if (area.height == 1)
            {
                // Failed on a single line
                break;
            }
            area.height = (area.height + 1) >> 1; // Cannot become 0 as (2+1)>>1=1
        }
        area.y += area.height;
        if (area.bottom() > bottom)
        {
            area.height = bottom - area.y;
        }
    }
}

bool CWRVectorRenderer::drawPathArea(const uint8_t* cmds, uint32_t nCmds, const float* points, uint32_t nPoints, const Rect& area) const
{
    assert(cmds);
    assert(points);

    if (drawMode != STROKE)
    {
        return drawFill(cmds, nCmds, points, nPoints, area);
    }
    return drawStroke(cmds, nCmds, points, nPoints, area);
}

bool CWRVectorRenderer::drawFill(const uint8_t* cmds, uint32_t nCmds, const float* points, uint32_t nPoints, const Rect& area) const
{
    (void)nPoints;
    uint32_t cmdInx = 0;
    uint32_t pointInx = 0;

    Canvas canvas(canvasPainter, canvasAreaAbsolute, area, 255U);
    canvas.setFillingRule((drawMode == FILL_EVEN_ODD) ? Rasterizer::FILL_EVEN_ODD : Rasterizer::FILL_NON_ZERO);

    float positionX = 0.0f;
    float positionY = 0.0f;
    Matrix3x3::Point transformedPosition = matrix.affineTransform(positionX, positionY);
    while (cmdInx < nCmds)
    {
        switch (static_cast<VectorPrimitives>(cmds[cmdInx]))
        {
        case VECTOR_PRIM_CLOSE:
            assert(pointInx <= nPoints);
            canvas.close();
            break;
        case VECTOR_PRIM_MOVE:
            assert(pointInx + 2 <= nPoints);
            positionX = points[pointInx];
            positionY = points[pointInx + 1];
            transformedPosition = matrix.affineTransform(positionX, positionY);
            canvas.moveTo(transformedPosition.x, transformedPosition.y);
            pointInx += 2;
            break;
        case VECTOR_PRIM_LINE:
            assert(pointInx + 1 <= nPoints);
            positionX = points[pointInx];
            positionY = points[pointInx + 1];
            transformedPosition = matrix.affineTransform(positionX, positionY);
            canvas.lineTo(transformedPosition.x, transformedPosition.y);
            pointInx += 2;
            break;
        case VECTOR_PRIM_HLINE:
            assert(pointInx + 1 <= nPoints);
            positionX = points[pointInx];
            transformedPosition = matrix.affineTransform(positionX, positionY);
            canvas.lineTo(transformedPosition.x, transformedPosition.y);
            pointInx += 1;
            break;
        case VECTOR_PRIM_VLINE:
            assert(pointInx + 1 <= nPoints);
            positionY = points[pointInx];
            transformedPosition = matrix.affineTransform(positionX, positionY);
            canvas.lineTo(transformedPosition.x, transformedPosition.y);
            pointInx += 1;
            break;
        case VECTOR_PRIM_BEZIER_QUAD:
            assert(pointInx + 4 <= nPoints);
            positionX = points[pointInx + 2];
            positionY = points[pointInx + 3];
            {
                const Matrix3x3::Point p1 = matrix.affineTransform(points[pointInx], points[pointInx + 1]);
                const Matrix3x3::Point p2 = matrix.affineTransform(positionX, positionY);
                canvas.quadraticBezierTo(transformedPosition.x, transformedPosition.y, p1.x, p1.y, p2.x, p2.y);
                transformedPosition = p2;
            }
            pointInx += 4;
            break;
        case VECTOR_PRIM_BEZIER_CUBIC:
            assert(pointInx + 6 <= nPoints);
            positionX = points[pointInx + 4];
            positionY = points[pointInx + 5];
            {
                const Matrix3x3::Point p1 = matrix.affineTransform(points[pointInx], points[pointInx + 1]);
                const Matrix3x3::Point p2 = matrix.affineTransform(points[pointInx + 2], points[pointInx + 3]);
                const Matrix3x3::Point p3 = matrix.affineTransform(positionX, positionY);
                canvas.cubicBezierTo(transformedPosition.x, transformedPosition.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                transformedPosition = p3;
            }
            pointInx += 6;
            break;
        }
        cmdInx++;
    }

    return canvas.render(LCD::div255(colorAlpha * alpha));
}

bool CWRVectorRenderer::drawStroke(const uint8_t* cmds, uint32_t nCmds, const float* points, uint32_t nPoints, const Rect& area) const
{
    (void)nPoints;
    uint32_t cmdInx = 0;
    uint32_t pointInx = 0;

    if (strokeWidth <= 0.0f)
    {
        return true;
    }

    StrokeCanvas canvas(canvasPainter, canvasAreaAbsolute, area, 255U, matrix);
    canvas.setStroke(strokeWidth, strokeMiterLimit, strokeLineJoin, strokeLineCap, LCD::div255(colorAlpha * alpha));

    float positionX = 0.0f;
    float positionY = 0.0f;
    float initialShapeX = 0.0f;
    float initialShapeY = 0.0f;
    uint32_t cmdInxPathStart = 0;
    while (cmdInx < nCmds)
    {
        assert(static_cast<VectorPrimitives>(cmds[cmdInx]) == VECTOR_PRIM_CLOSE || pointInx < nPoints);
        switch (static_cast<VectorPrimitives>(cmds[cmdInx]))
        {
        case VECTOR_PRIM_CLOSE:
            positionX = initialShapeX;
            positionY = initialShapeY;
            if (cmdInxPathStart < cmdInx) // A single "CLOSE" on it's own does nothing
            {
                // In case we are not back at the beginning, draw a line to that point
                canvas.strokeLineTo(positionX, positionY);
                // Check if it a zero-length closed line and draw a double linecap
                if (!canvas.strokeDrawDoubleLineCap())
                {
                    // The line has some extend - close the outside
                    canvas.strokeClose(positionX, positionY);
                    // Move back to the start
                    canvas.strokeMoveTo(positionX, positionY);
                    // Draw inside backwards without linecaps(false)
                    drawStrokeBackwards(cmdInxPathStart, cmdInx + 1, pointInx, cmds, points, false, canvas);
                    // Close the inside of the stroke
                    canvas.strokeClose(positionX, positionY);
                }
            }
            // Next path starts after the CLOSE command
            cmdInxPathStart = cmdInx + 1;
            break;
        case VECTOR_PRIM_MOVE:
            assert(pointInx + 2 <= nPoints);
            if (cmdInxPathStart < cmdInx) // More than a singe "MOVE"
            {
                // Check if it a zero-length line and draw a double linecap
                if (!canvas.strokeDrawDoubleLineCap())
                {
                    // The line has some extend, follow the commands backwards and draw linecaps(true)
                    drawStrokeBackwards(cmdInxPathStart, cmdInx, pointInx, cmds, points, true, canvas);
                }
            }
            cmdInxPathStart = cmdInx;
            initialShapeX = positionX = points[pointInx];
            initialShapeY = positionY = points[pointInx + 1];
            canvas.strokeMoveTo(positionX, positionY);
            pointInx += 2;
            break;
        case VECTOR_PRIM_LINE:
            assert(pointInx + 2 <= nPoints);
            positionX = points[pointInx];
            positionY = points[pointInx + 1];
            canvas.strokeLineTo(positionX, positionY);
            pointInx += 2;
            break;
        case VECTOR_PRIM_HLINE:
            assert(pointInx + 1 <= nPoints);
            positionX = points[pointInx];
            canvas.strokeLineTo(positionX, positionY);
            pointInx += 1;
            break;
        case VECTOR_PRIM_VLINE:
            assert(pointInx + 1 <= nPoints);
            positionY = points[pointInx];
            canvas.strokeLineTo(positionX, positionY);
            pointInx += 1;
            break;
        case VECTOR_PRIM_BEZIER_QUAD:
            assert(pointInx + 4 <= nPoints);
            canvas.strokeBezierQuad(positionX, positionY, points[pointInx], points[pointInx + 1], points[pointInx + 2], points[pointInx + 3]);
            positionX = points[pointInx + 2];
            positionY = points[pointInx + 3];
            pointInx += 4;
            break;
        case VECTOR_PRIM_BEZIER_CUBIC:
            assert(pointInx + 6 <= nPoints);
            canvas.strokeBezierCubic(positionX, positionY, points[pointInx], points[pointInx + 1], points[pointInx + 2], points[pointInx + 3], points[pointInx + 4], points[pointInx + 5]);
            positionX = points[pointInx + 4];
            positionY = points[pointInx + 5];
            pointInx += 6;
            break;
        }
        cmdInx++;
    }
    if (cmdInxPathStart < cmdInx) // More than a singe "MOVE"
    {
        // Check if it a zero-length line and draw a double linecap
        if (!canvas.strokeDrawDoubleLineCap())
        {
            // The line has some extend, follow the commands backwards and draw linecaps(true)
            drawStrokeBackwards(cmdInxPathStart, cmdInx, pointInx, cmds, points, true, canvas);
        }
    }
    return canvas.strokeRender();
}

void CWRVectorRenderer::drawStrokeBackwards(uint32_t cmdInxPathStart, uint32_t cmdInx, uint32_t pointInx, const uint8_t* cmds, const float* points, bool drawLineCaps, StrokeCanvas& canvas) const
{
    float positionX = 0.0f;
    float positionY = 0.0f;
    canvas.lineCapInsteadOfLineJoin(drawLineCaps);
    while (cmdInx > cmdInxPathStart)
    {
        assert(cmdInx > 0);
        cmdInx--;
        switch (static_cast<VectorPrimitives>(cmds[cmdInx]))
        {
        case VECTOR_PRIM_CLOSE:
            getPreviousDestination(positionX, positionY, cmdInx, pointInx, cmds, points);
            canvas.strokeLineTo(positionX, positionY);
            break;
        case VECTOR_PRIM_MOVE:
            assert(cmdInx == cmdInxPathStart);
            break;
        case VECTOR_PRIM_LINE:
            assert(pointInx >= 2);
            pointInx -= 2;
            getPreviousDestination(positionX, positionY, cmdInx, pointInx, cmds, points);
            canvas.strokeLineTo(positionX, positionY);
            break;
        case VECTOR_PRIM_HLINE:
        case VECTOR_PRIM_VLINE:
            assert(pointInx >= 1);
            pointInx -= 1;
            getPreviousDestination(positionX, positionY, cmdInx, pointInx, cmds, points);
            canvas.strokeLineTo(positionX, positionY);
            break;
        case VECTOR_PRIM_BEZIER_QUAD:
            assert(pointInx >= 4);
            pointInx -= 4;
            getPreviousDestination(positionX, positionY, cmdInx, pointInx, cmds, points);
            canvas.strokeBezierQuad(points[pointInx + 2], points[pointInx + 3], //lint !e662 !e661
                                    points[pointInx], points[pointInx + 1],     //lint !e662 !e661
                                    positionX, positionY);
            break;
        case VECTOR_PRIM_BEZIER_CUBIC:
            assert(pointInx >= 6);
            pointInx -= 6;
            getPreviousDestination(positionX, positionY, cmdInx, pointInx, cmds, points);
            canvas.strokeBezierCubic(points[pointInx + 4], points[pointInx + 5], //lint !e662 !e661
                                     points[pointInx + 2], points[pointInx + 3], //lint !e662 !e661
                                     points[pointInx], points[pointInx + 1],     //lint !e662 !e661
                                     positionX, positionY);
            break;
        }
    }
    if (drawLineCaps)
    {
        canvas.drawFinalLineCap();
    }
}

void CWRVectorRenderer::getPreviousDestination(float& positionX, float& positionY, uint32_t cmdInx, uint32_t pointInx, const uint8_t* cmds, const float* points) const
{
    bool foundX = false;
    bool foundY = false;
    while (cmdInx > 0)
    {
        cmdInx--;
        switch (static_cast<VectorPrimitives>(cmds[cmdInx]))
        {
        case VECTOR_PRIM_CLOSE:
            break;
        case VECTOR_PRIM_MOVE:
        case VECTOR_PRIM_LINE:
        case VECTOR_PRIM_BEZIER_QUAD:
        case VECTOR_PRIM_BEZIER_CUBIC:
            assert(pointInx >= 2);
            pointInx -= 2;
            if (!foundX)
            {
                positionX = points[pointInx]; //lint !e662 !e661
            }
            if (!foundY)
            {
                positionY = points[pointInx + 1]; //lint !e662 !e661
            }
            return;
        case VECTOR_PRIM_HLINE:
            assert(pointInx >= 1);
            pointInx -= 1;
            if (!foundX)
            {
                positionX = points[pointInx]; //lint !e662 !e661
            }
            if (foundY)
            {
                return;
            }
            foundX = true;
            break;
        case VECTOR_PRIM_VLINE:
            assert(pointInx >= 1);
            pointInx -= 1;
            if (!foundY)
            {
                positionY = points[pointInx]; //lint !e662 !e661
            }
            if (foundX)
            {
                return;
            }
            foundY = true;
            break;
        }
    }
    if (!foundX)
    {
        positionX = 0.0f;
    }
    if (!foundY)
    {
        positionY = 0.0f;
    }
}

void CWRVectorRenderer::setStrokeWidth(float w)
{
    strokeWidth = w;
}

void CWRVectorRenderer::setColor(colortype c)
{
    // Save color alpha for rendering
    colorAlpha = c >> 24;

    getColorPainterColor().setColor(c);
    canvasPainter = &getColorPainter();
}

void CWRVectorRenderer::setAlpha(uint8_t a)
{
    alpha = a;
}

void CWRVectorRenderer::setLinearGradient(float x0, float y0, float x1, float y1,
                                          uint32_t stops,
                                          const float* /* stopPositions */,
                                          const uint32_t* stopColors,
                                          float width,
                                          float height,
                                          bool isSolid,
                                          const uint32_t* palette)
{
    // If 'x1' = 'x2' and 'y1' = 'y2', then the area to be painted shall be painted
    // as a single color using the color and opacity of the last gradient stop.
    if ((x0 - x1) == 0.0f && (y0 - y1) == 0.0f)
    {
        setColor(stopColors[stops - 1]);
        return;
    }

    // Make colorAlpha neutral
    colorAlpha = 255;

    AbstractPainterLinearGradient& linearPainter = getLinearPainter();
    linearPainter.setGradientEndPoints(x0, y0, x1, y1, width, height, matrix);
    assert(palette && "A gradient palette is required by CWRVectorRenderer");
    linearPainter.setGradientTexture(palette, isSolid);
    canvasPainter = &linearPainter;
}

void CWRVectorRenderer::setTransformationMatrix(const Matrix3x3& m)
{
    matrix = m;
}

void CWRVectorRenderer::StrokeCanvas::strokeMoveTo(float x, float y)
{
    lastX = x;
    lastY = y;
    firstLineTo = true;
    noLineHasBeenDrawn = true;
}

void CWRVectorRenderer::StrokeCanvas::strokeLineTo(float x, float y)
{
    const float dx = x - lastX;
    const float dy = y - lastY;
    float len = sqrtf(dx * dx + dy * dy);
    if (len > 0.0f)
    {
        noLineHasBeenDrawn = false;
        len /= strokeWidthHalf;
        const float thisStrokeStartX = lastX + dy / len;
        const float thisStrokeStartY = lastY - dx / len;
        const float thisStrokeEndX = x + dy / len;
        const float thisStrokeEndY = y - dx / len;
        if (firstLineTo)
        {
            initialStrokeStartX = thisStrokeStartX;
            initialStrokeStartY = thisStrokeStartY;
            initialStrokeEndX = thisStrokeEndX;
            initialStrokeEndY = thisStrokeEndY;
            moveToWithTransform(thisStrokeStartX, thisStrokeStartY);
            firstLineTo = false;
        }
        else if (drawLineCapInsteadOfLineJoin)
        {
            strokeDrawLineCap(lastStrokeEndX, lastStrokeEndY, thisStrokeStartX, thisStrokeStartY);
            drawLineCapInsteadOfLineJoin = false;
        }
        else
        {
            strokeDrawLineJoin(lastX, lastY, lastStrokeStartX, lastStrokeStartY,
                               lastStrokeEndX, lastStrokeEndY, thisStrokeStartX, thisStrokeStartY, thisStrokeEndX, thisStrokeEndY);
        }
        lastStrokeStartX = thisStrokeStartX;
        lastStrokeStartY = thisStrokeStartY;
        lastStrokeEndX = thisStrokeEndX;
        lastStrokeEndY = thisStrokeEndY;
        lastX = x;
        lastY = y;
    }
}

bool CWRVectorRenderer::StrokeCanvas::strokeClose(float x, float y)
{
    if (strokeDrawDoubleLineCap())
    {
        return false; // Created two line-caps, stop drawing, we are done
    }
    strokeDrawLineJoin(x, y, lastStrokeStartX, lastStrokeStartY,
                       lastStrokeEndX, lastStrokeEndY, initialStrokeStartX, initialStrokeStartY, initialStrokeEndX, initialStrokeEndY);
    return true; // Closed as with a line-join, continue drawing on inside
}

bool CWRVectorRenderer::StrokeCanvas::strokeDrawDoubleLineCap()
{
    if (noLineHasBeenDrawn)
    {
        const float y1 = lastY - strokeWidthHalf;
        const float y2 = lastY + strokeWidthHalf;
        moveToWithTransform(lastX, y1);
        strokeDrawLineCap(lastX, y1, lastX, y2);
        strokeDrawLineCap(lastX, y2, lastX, y1);
        return true;
    }
    return false;
}

void CWRVectorRenderer::StrokeCanvas::lineCapInsteadOfLineJoin(bool drawLineCaps)
{
    drawLineCapInsteadOfLineJoin = drawLineCaps;
}

void CWRVectorRenderer::StrokeCanvas::drawFinalLineCap()
{
    strokeDrawLineCap(lastStrokeEndX, lastStrokeEndY, initialStrokeStartX, initialStrokeStartY);
}

void CWRVectorRenderer::StrokeCanvas::strokeDrawLineJoin(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    if (strokeLineJoin == VG_STROKE_LINEJOIN_ROUND)
    {
        lineToWithTransform(x2, y2);
        // Same formula as used below for MITER
        if ((x1 - x2) * (y3 - y4) >= (y1 - y2) * (x3 - x4))
        {
            strokeDrawLineJoinRound(x, y, x2, y2, x3, y3, 1);
        }
        lineToWithTransform(x3, y3);
        return;
    }

    if (strokeLineJoin == VG_STROKE_LINEJOIN_MITER)
    {
        // https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Given_two_points_on_each_line
        const float divisor = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        // If divisor>0, we are on the "outside" of the angle
        // If divisor<0, we are on the "inside" of the angle
        if (divisor != 0.0f)
        {
            const float miterX = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / divisor;
            const float miterY = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / divisor;
            const float distSquared = (x - miterX) * (x - miterX) + (y - miterY) * (y - miterY);
            if (distSquared <= miterLimitSquared)
            {
                lineToWithTransform(miterX, miterY);
                return;
            }
        }
    }

    // VG_STROKE_LINEJOIN_BEVEL (and fallback for VG_STROKE_LINEJOIN_MITER)
    lineToWithTransform(x2, y2);
    lineToWithTransform(x3, y3);
}

void CWRVectorRenderer::StrokeCanvas::strokeDrawLineJoinRound(float x, float y, float x2, float y2, float x3, float y3, int level)
{
    assert(level >= 1 && level <= 3);
    float midX = (x2 + x3) / 2.0f;
    float midY = (y2 + y3) / 2.0f;
    const float midLen = sqrtf((x - midX) * (x - midX) + (y - midY) * (y - midY));
    if (midLen == 0.0f)
    {
        const float dx = (x3 - x2) / 2.0f;
        const float dy = (y3 - y2) / 2.0f;
        midX = x2 + dx + dy;
        midY = y2 + dy - dx;
    }
    else
    {
        midX = x + (midX - x) * strokeWidthHalf / midLen;
        midY = y + (midY - y) * strokeWidthHalf / midLen;
    }
    if (level < 3)
    {
        strokeDrawLineJoinRound(x, y, x2, y2, midX, midY, level + 1);
        lineToWithTransform(midX, midY);
        strokeDrawLineJoinRound(x, y, midX, midY, x3, y3, level + 1);
    }
    else
    {
        lineToWithTransform(midX, midY);
    }
}

void CWRVectorRenderer::StrokeCanvas::strokeDrawLineCap(float x2, float y2, float x3, float y3)
{
    if (strokeLineCap == VG_STROKE_LINECAP_BUTT)
    {
        lineToWithTransform(x2, y2);
        lineToWithTransform(x3, y3);
        return;
    }
    const float dx = (x3 - x2) / 2.0f;
    const float dy = (y3 - y2) / 2.0f;
    // dx,dy is from (x2,y2) to the midpoint between (x2,y2) and (x3,y3)
    // +dy,-dx is orthogonal and points in the direction of where the cap should be drawn
    if (strokeLineCap == VG_STROKE_LINECAP_SQUARE)
    {
        lineToWithTransform(x2 + dy, y2 - dx);
        lineToWithTransform(x3 + dy, y3 - dx);
        return;
    }
    // VG_STROKE_LINECAP_ROUND:
    // Using 3^2 + 4^2 = 5^2 as well as 7^2 + 24^2 = 25^2
    lineToWithTransform(x2, y2);
    lineToWithTransform(x2 + dx * 0.04f + dy * 0.28f, y2 + dy * 0.04f - dx * 0.28f);
    lineToWithTransform(x2 + dx * 0.2f + dy * 0.6f, y2 + dy * 0.2f - dx * 0.6f);
    lineToWithTransform(x2 + dx * 0.4f + dy * 0.8f, y2 + dy * 0.4f - dx * 0.8f);
    lineToWithTransform(x2 + dx * 0.72f + dy * 0.96f, y2 + dy * 0.72f - dx * 0.96f);
    lineToWithTransform(x2 + dx + dy, y2 + dy - dx);
    lineToWithTransform(x2 + dx * 1.28f + dy * 0.96f, y2 + dy * 1.28f - dx * 0.96f);
    lineToWithTransform(x2 + dx * 1.6f + dy * 0.8f, y2 + dy * 1.6f - dx * 0.8f);
    lineToWithTransform(x2 + dx * 1.8f + dy * 0.6f, y2 + dy * 1.8f - dx * 0.6f);
    lineToWithTransform(x2 + dx * 1.96f + dy * 0.28f, y2 + dy * 1.96f - dx * 0.28f);
    lineToWithTransform(x3, y3);
}

} // namespace touchgfx
