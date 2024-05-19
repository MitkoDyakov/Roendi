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
#include <touchgfx/widgets/canvas/Canvas.hpp>
#include <touchgfx/widgets/canvas/Line.hpp>

namespace touchgfx
{
Line::Line()
    : CanvasWidget(),
      startXQ5(0), startYQ5(0), endXQ5(0), endYQ5(0),
      lineWidthQ5(CWRUtil::toQ5<int>(1)),
      lineEnding(BUTT_CAP_ENDING),
      minimalRect(),
      lineCapArcIncrement(18)
{
    Drawable::setWidthHeight(0, 0);
}

void Line::setStart(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5)
{
    if (startXQ5 == xQ5 && startYQ5 == yQ5)
    {
        return;
    }

    startXQ5 = xQ5;
    startYQ5 = yQ5;

    updateCachedShape();
}

void Line::updateStart(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5)
{
    if (startXQ5 == xQ5 && startYQ5 == yQ5)
    {
        return;
    }

    invalidateContent();

    startXQ5 = xQ5;
    startYQ5 = yQ5;

    updateCachedShape();

    invalidateContent();
}

void Line::setEnd(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5)
{
    if (endXQ5 == xQ5 && endYQ5 == yQ5)
    {
        return;
    }

    endXQ5 = xQ5;
    endYQ5 = yQ5;

    updateCachedShape();
}

void Line::updateEnd(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5)
{
    if (endXQ5 == xQ5 && endYQ5 == yQ5)
    {
        return;
    }

    invalidateContent();

    endXQ5 = xQ5;
    endYQ5 = yQ5;

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
    Canvas canvas(getPainter(), getAbsoluteRect(), invalidatedArea, getAlpha());

    CWRUtil::Q5 radiusQ5;
    if (lineEnding == ROUND_CAP_ENDING)
    {
        const int angleInDegrees = CWRUtil::angle(cornerXQ5[0] - startXQ5, cornerYQ5[0] - startYQ5, radiusQ5);
        canvas.moveTo(cornerXQ5[0], cornerYQ5[0]);
        canvas.lineTo(cornerXQ5[1], cornerYQ5[1]);
        for (int i = lineCapArcIncrement; i < 180; i += lineCapArcIncrement)
        {
            canvas.lineTo(endXQ5 + radiusQ5 * CWRUtil::sine(angleInDegrees - i), endYQ5 - radiusQ5 * CWRUtil::cosine(angleInDegrees - i));
        }
        canvas.lineTo(cornerXQ5[2], cornerYQ5[2]);
        canvas.lineTo(cornerXQ5[3], cornerYQ5[3]);
        for (int i = 180 - lineCapArcIncrement; i > 0; i -= lineCapArcIncrement)
        {
            canvas.lineTo(startXQ5 + radiusQ5 * CWRUtil::sine(angleInDegrees + i), startYQ5 - radiusQ5 * CWRUtil::cosine(angleInDegrees + i));
        }
    }
    else
    {
        canvas.moveTo(cornerXQ5[0], cornerYQ5[0]);
        canvas.lineTo(cornerXQ5[1], cornerYQ5[1]);
        canvas.lineTo(cornerXQ5[2], cornerYQ5[2]);
        canvas.lineTo(cornerXQ5[3], cornerYQ5[3]);
    }
    return canvas.render();
}

void Line::updateCachedShape()
{
    CWRUtil::Q5 dxQ5 = endXQ5 - startXQ5;
    CWRUtil::Q5 dyQ5 = endYQ5 - startYQ5;
    CWRUtil::Q5 dQ5 = CWRUtil::toQ5<int>(0);
    // Look for horizontal, vertical or no-line
    if (dxQ5 == 0)
    {
        if (dyQ5 == 0)
        {
            cornerXQ5[0] = cornerXQ5[1] = cornerXQ5[2] = cornerXQ5[3] = startXQ5;
            cornerYQ5[0] = cornerYQ5[1] = cornerYQ5[2] = cornerYQ5[3] = startYQ5;
            return;
        }
        dQ5 = abs(dyQ5);
    }
    else if (dyQ5 == 0)
    {
        dQ5 = abs(dxQ5);
    }
    else
    {
        // We want to hit as close to the limit inside 32bits.
        // sqrt(0x7FFFFFFF / 2) = 46340, which is roughly toQ5(1448)
        static const int32_t MAXVAL = 46340;
        const int32_t common_divisor = gcd(abs((int32_t)dxQ5), abs((int32_t)dyQ5));
        // First try to scale down
        if (common_divisor != 1)
        {
            dxQ5 = CWRUtil::Q5((int32_t)dxQ5 / common_divisor);
            dyQ5 = CWRUtil::Q5((int32_t)dyQ5 / common_divisor);
        }
        // Neither dx or dy is zero, find the largest multiplier / smallest divisor to stay within limit
        if (abs((int32_t)dxQ5) <= MAXVAL || abs((int32_t)dyQ5) <= MAXVAL)
        {
            // Look for largest multiplier
            const int32_t mulx = MAXVAL / abs((int32_t)dxQ5);
            const int32_t muly = MAXVAL / abs((int32_t)dyQ5);
            const int32_t mult = MIN(mulx, muly);
            dxQ5 = CWRUtil::Q5(mult * (int32_t)dxQ5);
            dyQ5 = CWRUtil::Q5(mult * (int32_t)dyQ5);
        }
        else
        {
            // Look for smallest divisor
            const int32_t divx = abs((int32_t)dxQ5) / MAXVAL;
            const int32_t divy = abs((int32_t)dyQ5) / MAXVAL;
            const int32_t divi = MAX(divx, divy) + 1;
            dxQ5 = CWRUtil::Q5((int32_t)dxQ5 / divi);
            dyQ5 = CWRUtil::Q5((int32_t)dyQ5 / divi);
        }
        dQ5 = CWRUtil::length(dxQ5, dyQ5);
    }

    dyQ5 = CWRUtil::muldivQ5(lineWidthQ5, dyQ5, dQ5) / 2;
    dxQ5 = CWRUtil::muldivQ5(lineWidthQ5, dxQ5, dQ5) / 2;

    switch (lineEnding)
    {
    case BUTT_CAP_ENDING:
        cornerXQ5[0] = startXQ5 - dyQ5;
        cornerYQ5[0] = startYQ5 + dxQ5;
        cornerXQ5[1] = endXQ5 - dyQ5;
        cornerYQ5[1] = endYQ5 + dxQ5;
        cornerXQ5[2] = endXQ5 + dyQ5;
        cornerYQ5[2] = endYQ5 - dxQ5;
        cornerXQ5[3] = startXQ5 + dyQ5;
        cornerYQ5[3] = startYQ5 - dxQ5;
        break;
    case ROUND_CAP_ENDING:
        // Nothing cached, calculated on each draw, but extremes are same as SQUARE_CAP_ENDING, so
        // Fall Through (for calculations)
    case SQUARE_CAP_ENDING:
        cornerXQ5[0] = (startXQ5 - dyQ5) - dxQ5;
        cornerYQ5[0] = (startYQ5 + dxQ5) - dyQ5;
        cornerXQ5[1] = (endXQ5 - dyQ5) + dxQ5;
        cornerYQ5[1] = (endYQ5 + dxQ5) + dyQ5;
        cornerXQ5[2] = (endXQ5 + dyQ5) + dxQ5;
        cornerYQ5[2] = (endYQ5 - dxQ5) + dyQ5;
        cornerXQ5[3] = (startXQ5 + dyQ5) - dxQ5;
        cornerYQ5[3] = (startYQ5 - dxQ5) - dyQ5;
        break;
    }

    CWRUtil::Q5 xMinQ5 = cornerXQ5[0];
    CWRUtil::Q5 xMaxQ5 = cornerXQ5[0];
    CWRUtil::Q5 yMinQ5 = cornerYQ5[0];
    CWRUtil::Q5 yMaxQ5 = cornerYQ5[0];
    for (int i = 1; i < 4; i++)
    {
        if (cornerXQ5[i] < xMinQ5)
        {
            xMinQ5 = cornerXQ5[i];
        }
        if (cornerXQ5[i] > xMaxQ5)
        {
            xMaxQ5 = cornerXQ5[i];
        }
        if (cornerYQ5[i] < yMinQ5)
        {
            yMinQ5 = cornerYQ5[i];
        }
        if (cornerYQ5[i] > yMaxQ5)
        {
            yMaxQ5 = cornerYQ5[i];
        }
    }
    const int16_t xMin = xMinQ5.floor();
    const int16_t yMin = yMinQ5.floor();
    const int16_t xMax = xMaxQ5.ceil();
    const int16_t yMax = yMaxQ5.ceil();
    minimalRect = Rect(xMin, yMin, xMax - xMin, yMax - yMin);

    if (lineEnding == ROUND_CAP_ENDING)
    {
        cornerXQ5[0] = startXQ5 - dyQ5;
        cornerYQ5[0] = startYQ5 + dxQ5;
        cornerXQ5[1] = endXQ5 - dyQ5;
        cornerYQ5[1] = endYQ5 + dxQ5;
        cornerXQ5[2] = endXQ5 + dyQ5;
        cornerYQ5[2] = endYQ5 - dxQ5;
        cornerXQ5[3] = startXQ5 + dyQ5;
        cornerYQ5[3] = startYQ5 - dxQ5;
    }
}

touchgfx::Rect Line::rectContainingPoints(const Rect& fullRect, CWRUtil::Q5 x0, CWRUtil::Q5 y0, CWRUtil::Q5 x1, CWRUtil::Q5 y1, CWRUtil::Q5 x2, CWRUtil::Q5 y2) const
{
    const int16_t xMin = MIN(MIN(x0, x1), x2).floor();
    const int16_t yMin = MIN(MIN(y0, y1), y2).floor();
    const int16_t xMax = MAX(MAX(x0, x1), x2).ceil();
    const int16_t yMax = MAX(MAX(y0, y1), y2).ceil();
    Rect r(xMin, yMin, xMax - xMin, yMax - yMin);
    r &= fullRect;
    return r;
}

Rect Line::getMinimalRect() const
{
    return minimalRect;
}

void Line::updateLengthAndAngle(CWRUtil::Q5 length, CWRUtil::Q5 angle)
{
    updateEnd(startXQ5 + length * CWRUtil::sine(angle), startYQ5 - length * CWRUtil::cosine(angle));
}

void Line::invalidateContent() const
{
    if (alpha == 0)
    {
        return;
    }
    Rect smallRect = getMinimalRect();
    if (abs(startXQ5.to<int>() - endXQ5.to<int>()) < lineWidthQ5.to<int>() * 2 ||
        abs(startYQ5.to<int>() - endYQ5.to<int>()) < lineWidthQ5.to<int>() * 2)
    {
        invalidateRect(smallRect);
        return;
    }

    const int16_t center_x = ((startXQ5 + endXQ5) / 2).to<int16_t>();
    const int16_t center_y = ((startYQ5 + endYQ5) / 2).to<int16_t>();
    // The following should be "lineWidth/sqrt(2)" but to speed up we take the slightly larger "lineWidth/1.3333"="(lineWidth*3)/4"
    const int16_t extra_width = ((CWRUtil::Q5)(((int)(lineWidthQ5)*3 + 3) >> 2)).ceil();
#define same_sign(x, y) (((x) < 0 && (y) < 0) || ((x) > 0 && (y) > 0))
    if (smallRect.width < smallRect.height)
    {
        const int16_t left_x = center_x - extra_width;
        const int16_t right_x = center_x + extra_width;
        // "vertical" line
        if (same_sign(endXQ5 - startXQ5, endYQ5 - startYQ5))
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
        if (same_sign(endXQ5 - startXQ5, endYQ5 - startYQ5))
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
