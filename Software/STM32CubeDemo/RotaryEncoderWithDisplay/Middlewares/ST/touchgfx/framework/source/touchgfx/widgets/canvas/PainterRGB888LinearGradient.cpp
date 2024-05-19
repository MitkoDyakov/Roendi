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

#include <touchgfx/hal/Paint.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888LinearGradient.hpp>

namespace touchgfx
{
void PainterRGB888LinearGradient::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    uint8_t* p = destination + offset * 3;

    if (isVertical)
    {
        // The whole line is the same color
        uint32_t color;
        if (widgetY <= coord0)
        {
            color = texture[deltaColor > 0 ? 0 : 1023];
        }
        else if (widgetY >= coord1)
        {
            color = texture[deltaColor > 0 ? 1023 : 0];
        }
        else
        {
            const int32_t colorOffset = (int)((widgetY - coord0) * deltaColor);
            color = texture[deltaColor > 0 ? colorOffset : 1023 + colorOffset];
        }

        const uint8_t b = color & 0xFF;
        const uint8_t g = (color >> 8) & 0xFF;
        const uint8_t r = (color >> 16) & 0xFF;
        uint8_t* const p_end = p + count * 3;

        if (isSolid && (alpha == 255))
        {
            while (p < p_end)
            {
                *p++ = b;
                *p++ = g;
                *p++ = r;
            }
        }
        else
        {
            const uint8_t a = (color >> 24) & 0xFF;
            const uint8_t alphaTot = LCD::div255(a * alpha);
            const uint8_t ialpha = 0xFF - alphaTot;
            while (p < p_end)
            {
                uint8_t pByte = *p;
                *p++ = LCD::div255(b * alphaTot + pByte * ialpha);
                pByte = *p;
                *p++ = LCD::div255(g * alphaTot + pByte * ialpha);
                pByte = *p;
                *p++ = LCD::div255(r * alphaTot + pByte * ialpha);
            };
        }
        return;
    }

    //vx0 is where first color line (y=clSlope*x+clOffset) intersects the horizontal line y=widgetY
    float vx0;
    if (isHorizontal)
    {
        vx0 = coord0;
    }
    else
    {
        vx0 = (widgetY - clOffset) / clSlope;
        if (deltaColor < 0.0f)
        {
            vx0 -= horizontalDistance;
        }
    }

    int x = widgetX;

    // Left of gradient
    // All pixels up to first gradient line, paint using fixed color
    if (x <= vx0)
    {
        // Starting before gradient
        const int pixels = (int)(MIN(vx0 + 1 - x, count));
        // Start in correct end of palette depending on direction
        const uint32_t colorStart = texture[deltaColor > 0 ? 0 : 1023];
        uint8_t* const p_end = p + pixels * 3;
        const uint8_t b = colorStart & 0xFF;
        const uint8_t g = (colorStart >> 8) & 0xFF;
        const uint8_t r = (colorStart >> 16) & 0xFF;
        if (isSolid && (alpha == 255))
        {
            while (p < p_end)
            {
                *p++ = b;
                *p++ = g;
                *p++ = r;
            }
        }
        else
        {
            const uint8_t a = (colorStart >> 24) & 0xFF;
            const uint8_t alphaTot = LCD::div255(a * alpha);
            const uint8_t ialpha = 0xFF - alphaTot;
            while (p < p_end)
            {
                uint8_t pByte = *p;
                *p++ = LCD::div255(b * alphaTot + pByte * ialpha);
                pByte = *p;
                *p++ = LCD::div255(g * alphaTot + pByte * ialpha);
                pByte = *p;
                *p++ = LCD::div255(r * alphaTot + pByte * ialpha);
            };
        }
        x += pixels;
    }

    const int16_t right = widgetX + count;
    if (x < right) // More to draw
    {
        //vx1 is where y=widgetY intersects the last color line (fixed distance from vx0)
        const float vx1 = vx0 + horizontalDistance;

        if (x < vx1) // Draw gradient part until last color line
        {
            const int endx = (int)(MIN(right, vx1));
            const uint8_t* const p_end = p + (endx - x) * 3;

            // Start with first or last color depending on direction
            float colorF = deltaColor > 0 ? 0.0000f : 1023.9999f;
            colorF += (x - vx0) * deltaColor;

            if (isSolid && (alpha == 255))
            {
                while (p < p_end)
                {
                    const int colorIndex = (int)colorF;
                    const uint8_t* color = (const uint8_t*)(texture + colorIndex);
                    *p++ = *color++;
                    *p++ = *color++;
                    *p++ = *color;
                    colorF += deltaColor;
                }
            }
            else
            {
                // Go to 8-bit pointers
                const uint8_t* const gradient = (const uint8_t*)texture;
                while (p < p_end)
                {
                    const int colorIndex = (int)colorF;

                    const uint8_t* color = gradient + colorIndex * 4;
                    const uint8_t a = color[3];
                    const uint8_t alphaTot = LCD::div255(a * alpha);
                    const uint8_t ialpha = 0xFF - alphaTot;

                    uint8_t pByte = *p;
                    *p++ = LCD::div255(*color++ * alphaTot + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*color++ * alphaTot + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*color++ * alphaTot + pByte * ialpha);

                    colorF += deltaColor;
                }
            }
            x = endx;
        }

        if (x < right) // Second fixed color part
        {
            const uint8_t* const p_end = p + (right - x) * 3;
            const uint32_t colorEnd = texture[deltaColor > 0 ? 1023 : 0];
            const uint8_t b = colorEnd & 0xFF;
            const uint8_t g = (colorEnd >> 8) & 0xFF;
            const uint8_t r = (colorEnd >> 16) & 0xFF;
            if (isSolid && (alpha == 255))
            {
                while (p < p_end)
                {
                    *p++ = b;
                    *p++ = g;
                    *p++ = r;
                }
            }
            else
            {
                const uint8_t a = (colorEnd >> 24) & 0xFF;
                const uint8_t alphaTot = LCD::div255(a * alpha);
                const uint8_t ialpha = 0xFF - alphaTot;
                while (p < p_end)
                {
                    uint8_t pByte = *p;
                    *p++ = LCD::div255(b * alphaTot + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(g * alphaTot + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(r * alphaTot + pByte * ialpha);
                };
            }
        }
    }
}

void PainterRGB888LinearGradient::tearDown() const
{
}
} // namespace touchgfx
