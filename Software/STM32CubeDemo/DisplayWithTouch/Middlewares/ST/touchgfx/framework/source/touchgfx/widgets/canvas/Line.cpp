/******************************************************************************
* Copyright (c) 2018(-2022) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.19.1 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

#include <touchgfx/Drawable.hpp>
#include <touchgfx/widgets/canvas/Canvas.hpp>
#include <touchgfx/widgets/canvas/Line.hpp>

namespace touchgfx
{
Line::Line()
    : CanvasWidget(),
      startX(0), startY(0), endX(0), endY(0),
      lineWidth(CWRUtil::toQ5<int>(1)),
      lineEnding(BUTT_CAP_ENDING),
      minimalRect(),
      lineCapArcIncrement(18)
{
    Drawable::setWidthHeight(0, 0);
}

void Line::setStart(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5)
{
    if (startX == xQ5 && startY == yQ5)
    {
        return;
    }

    startX = xQ5;
    startY = yQ5;

    updateCachedShape();
}

void Line::updateStart(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5)
{
    if (startX == xQ5 && startY == yQ5)
    {
        return;
    }

    invalidateContent();

    startX = xQ5;
    startY = yQ5;

    updateCachedShape();

    invalidateContent();
}

void Line::setEnd(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5)
{
    if (endX == xQ5 && endY == yQ5)
    {
        return;
    }

    endX = xQ5;
    endY = yQ5;

    updateCachedShape();
}

void Line::updateEnd(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5)
{
    if (endX == xQ5 && endY == yQ5)
    {
        return;
    }

    invalidateContent();

    endX = xQ5;
    endY = yQ5;

    updateCachedShape();

    invalidateContent();
}

void Line::setLineEndingStyle(Line::LINE_ENDING_STYLE lineEndingStyle)
{
    lineEnding = lineEndingStyle;
    updateCachedShape();
}

Line::LINE_ENDING_STYLE Line::getLineEndingStyle() const
{
    return lineEnding;
}

void Line::setCapPrecision(int precision)
{
    if (precision < 1)
    {
        precision = 1;
    }
    if (precision > 180)
    {
        precision = 180;
    }
    lineCapArcIncrement = precision;
}

bool Line::drawCanvasWidget(const Rect& invalidatedArea) const
{
    Canvas canvas(this, invalidatedArea);

    CWRUtil::Q5 radius;
    if (lineEnding == ROUND_CAP_ENDING)
    {
        const int angleInDegrees = CWRUtil::angle(xCorner[0] - startX, yCorner[0] - startY, radius);
        canvas.moveTo(xCorner[0], yCorner[0]);
        canvas.lineTo(xCorner[1], yCorner[1]);
        for (int i = lineCapArcIncrement; i < 180; i += lineCapArcIncrement)
        {
            canvas.lineTo(endX + radius * CWRUtil::sine(angleInDegrees - i), endY - radius * CWRUtil::cosine(angleInDegrees - i));
        }
        canvas.lineTo(xCorner[2], yCorner[2]);
        canvas.lineTo(xCorner[3], yCorner[3]);
        for (int i = 180 - lineCapArcIncrement; i > 0; i -= lineCapArcIncrement)
        {
            canvas.lineTo(startX + radius * CWRUtil::sine(angleInDegrees + i), startY - radius * CWRUtil::cosine(angleInDegrees + i));
        }
    }
    else
    {
        canvas.moveTo(xCorner[0], yCorner[0]);
        canvas.lineTo(xCorner[1], yCorner[1]);
        canvas.lineTo(xCorner[2], yCorner[2]);
        canvas.lineTo(xCorner[3], yCorner[3]);
    }
    return canvas.render();
}

void Line::updateCachedShape()
{
    CWRUtil::Q5 dx = endX - startX;
    CWRUtil::Q5 dy = endY - startY;
    CWRUtil::Q5 d = CWRUtil::toQ5<int>(0);
    // Look for horizontal, vertical or no-line
    if ((int32_t)dx == 0)
    {
        if ((int32_t)dy == 0)
        {
            xCorner[0] = xCorner[1] = xCorner[2] = xCorner[3] = startX;
            yCorner[0] = yCorner[1] = yCorner[2] = yCorner[3] = startY;
            return;
        }
        d = abs(dy);
    }
    else if ((int32_t)dy == 0)
    {
        d = abs(dx);
    }
    else
    {
        // We want to hit as close to the limit inside 32bits.
        // sqrt(0x7FFFFFFF / 2) = 46340, which is roughly toQ5(1448)
        static const int32_t MAXVAL = 46340;
        int32_t common_divisor = gcd(abs((int32_t)dx), abs((int32_t)dy));
        // First try to scale down
        if (common_divisor != 1)
        {
            dx = CWRUtil::Q5((int32_t)dx / common_divisor);
            dy = CWRUtil::Q5((int32_t)dy / common_divisor);
        }
        // Neither dx or dy is zero, find the largest multiplier / smallest divisor to stay within limit
        if (abs((int32_t)dx) <= MAXVAL || abs((int32_t)dy) <= MAXVAL)
        {
            // Look for largest multiplier
            int32_t mulx = MAXVAL / abs((int32_t)dx);
            int32_t muly = MAXVAL / abs((int32_t)dy);
            int32_t mult = MIN(mulx, muly);
            dx = CWRUtil::Q5(mult * (int32_t)dx);
            dy = CWRUtil::Q5(mult * (int32_t)dy);
        }
        else
        {
            // Look for smallest divisor
            int32_t divx = abs((int32_t)dx) / MAXVAL;
            int32_t divy = abs((int32_t)dy) / MAXVAL;
            int32_t divi = MAX(divx, divy) + 1;
            dx = CWRUtil::Q5((int32_t)dx / divi);
            dy = CWRUtil::Q5((int32_t)dy / divi);
        }
        d = CWRUtil::length(dx, dy);
    }

    dy = CWRUtil::muldivQ5(lineWidth, dy, d) / 2;
    dx = CWRUtil::muldivQ5(lineWidth, dx, d) / 2;

    switch (lineEnding)
    {
    case BUTT_CAP_ENDING:
        xCorner[0] = startX - dy;
        yCorner[0] = startY + dx;
        xCorner[1] = endX - dy;
        yCorner[1] = endY + dx;
        xCorner[2] = endX + dy;
        yCorner[2] = endY - dx;
        xCorner[3] = startX + dy;
        yCorner[3] = startY - dx;
        break;
    case ROUND_CAP_ENDING:
        // Nothing cached, calculated on each draw, but extremes are same as SQUARE_CAP_ENDING, so
        // Fall Through (for calculations)
    default:
    case SQUARE_CAP_ENDING:
        xCorner[0] = (startX - dy) - dx;
        yCorner[0] = (startY + dx) - dy;
        xCorner[1] = (endX - dy) + dx;
        yCorner[1] = (endY + dx) + dy;
        xCorner[2] = (endX + dy) + dx;
        yCorner[2] = (endY - dx) + dy;
        xCorner[3] = (startX + dy) - dx;
        yCorner[3] = (startY - dx) - dy;
        break;
    }

    CWRUtil::Q5 xMin = xCorner[0];
    CWRUtil::Q5 xMax = xCorner[0];
    CWRUtil::Q5 yMin = yCorner[0];
    CWRUtil::Q5 yMax = yCorner[0];
    for (int i = 1; i < 4; i++)
    {
        if (xCorner[i] < xMin)
        {
            xMin = xCorner[i];
        }
        if (xCorner[i] > xMax)
        {
            xMax = xCorner[i];
        }
        if (yCorner[i] < yMin)
        {
            yMin = yCorner[i];
        }
        if (yCorner[i] > yMax)
        {
            yMax = yCorner[i];
        }
    }
    int16_t minX = xMin.floor();
    int16_t minY = yMin.floor();
    int16_t maxX = xMax.ceil();
    int16_t maxY = yMax.ceil();
    minimalRect = Rect(minX, minY, maxX - minX, maxY - minY);

    if (lineEnding == ROUND_CAP_ENDING)
    {
        xCorner[0] = startX - dy;
        yCorner[0] = startY + dx;
        xCorner[1] = endX - dy;
        yCorner[1] = endY + dx;
        xCorner[2] = endX + dy;
        yCorner[2] = endY - dx;
        xCorner[3] = startX + dy;
        yCorner[3] = startY - dx;
    }
}

touchgfx::Rect Line::rectContainingPoints(const Rect& fullRect, CWRUtil::Q5 x0, CWRUtil::Q5 y0, CWRUtil::Q5 x1, CWRUtil::Q5 y1, CWRUtil::Q5 x2, CWRUtil::Q5 y2) const
{
    const int16_t minX = MIN(MIN(x0, x1), x2).floor();
    const int16_t minY = MIN(MIN(y0, y1), y2).floor();
    const int16_t maxX = MAX(MAX(x0, x1), x2).ceil();
    const int16_t maxY = MAX(MAX(y0, y1), y2).ceil();
    Rect r(minX, minY, maxX - minX, maxY - minY);
    r &= fullRect;
    return r;
}

Rect Line::getMinimalRect() const
{
    return minimalRect;
}

void Line::updateLengthAndAngle(CWRUtil::Q5 length, CWRUtil::Q5 angle)
{
    updateEnd(startX + length * CWRUtil::sine(angle), startY - length * CWRUtil::cosine(angle));
}

void Line::invalidateContent() const
{
    if (alpha == 0)
    {
        return;
    }
    Rect smallRect = getMinimalRect();
    if (abs(startX.to<int>() - endX.to<int>()) < lineWidth.to<int>() * 2 ||
        abs(startY.to<int>() - endY.to<int>()) < lineWidth.to<int>() * 2)
    {
        invalidateRect(smallRect);
        return;
    }

    int16_t center_x = ((startX + endX) / 2).to<int16_t>();
    int16_t center_y = ((startY + endY) / 2).to<int16_t>();
    // The following should be "lineWidth/sqrt(2)" but to speed up we take the slightly larger "lineWidth/1.3333"="(lineWidth*3)/4"
    int16_t extra_width = ((CWRUtil::Q5)(((int)(lineWidth)*3 + 3) >> 2)).ceil();
#define same_sign(x, y) (((x) < 0 && (y) < 0) || ((x) > 0 && (y) > 0))
    if (smallRect.width < smallRect.height)
    {
        const int16_t left_x = center_x - extra_width;
        const int16_t right_x = center_x + extra_width;
        // "vertical" line
        if (same_sign(endX - startX, endY - startY))
        {
            // From top-left to bottom-right
            Rect topLeftRect(smallRect.x, smallRect.y, right_x - smallRect.x, center_y - smallRect.y);
            Rect bottomRightRect(left_x, center_y, smallRect.right() - left_x, smallRect.bottom() - center_y);
            invalidateRect(topLeftRect);
            invalidateRect(bottomRightRect);
        }
        else
        {
            // From top-right to bottom-left
            Rect topRightRect(left_x, smallRect.y, smallRect.right() - left_x, center_y - smallRect.y);
            Rect bottomLeftRect(smallRect.x, center_y, right_x - smallRect.x, smallRect.bottom() - center_y);
            invalidateRect(topRightRect);
            invalidateRect(bottomLeftRect);
        }
    }
    else
    {
        const int16_t top_y = center_y - extra_width;
        const int16_t bottom_y = center_y + extra_width;
        // "horizontal" line
        if (same_sign(endX - startX, endY - startY))
        {
            // From top-left to bottom-right
            Rect topLeftRect(smallRect.x, smallRect.y, center_x - smallRect.x, bottom_y - smallRect.y);
            Rect bottomRightRect(center_x, top_y, smallRect.right() - center_x, smallRect.bottom() - top_y);
            invalidateRect(topLeftRect);
            invalidateRect(bottomRightRect);
        }
        else
        {
            // From top-right to bottom-left
            Rect bottomLeftRect(smallRect.x, top_y, center_x - smallRect.x, smallRect.bottom() - top_y);
            Rect topRightRect(center_x, smallRect.y, smallRect.right() - center_x, bottom_y - smallRect.y);
            invalidateRect(bottomLeftRect);
            invalidateRect(topRightRect);
        }
    }
#undef same_sign
}

} // namespace touchgfx
