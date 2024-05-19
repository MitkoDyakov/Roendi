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

#include <touchgfx/Utils.hpp>
#include <touchgfx/hal/Paint.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565LinearGradient.hpp>

namespace touchgfx
{
const uint16_t RMASK = 0xF800; // Mask for red   (1111100000000000)
const uint16_t GMASK = 0x07E0; // Mask for green (0000011111100000)
const uint16_t BMASK = 0x001F; // Mask for blue  (0000000000011111)

FORCE_INLINE_FUNCTION static uint16_t getNativeColor(colortype color)
{
    return ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
}

FORCE_INLINE_FUNCTION static uint16_t getNativeColorFromRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((red << 8) & 0xF800) | ((green << 3) & 0x07E0) | ((blue >> 3) & 0x001F);
}

FORCE_INLINE_FUNCTION static uint16_t alphaBlend(uint16_t R, uint16_t G, uint16_t B, uint16_t bufpix, uint8_t alpha)
{
    const uint8_t ialpha = 0xFF - alpha;
    return (((R * alpha + (bufpix & RMASK) * ialpha) / 255) & RMASK) |
           (((G * alpha + (bufpix & GMASK) * ialpha) / 255) & GMASK) |
           (((B * alpha + (bufpix & BMASK) * ialpha) / 255) & BMASK);
}

static uint16_t random(int x, int y)
{
    const long long int a = 15485863LL * (static_cast<long long int>(x) * static_cast<long long int>(y));
    return (a * a * a % 2038074743LL) & 0xFFFF;
}

static uint16_t applyNoise(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    const uint16_t rounding = random(x, y);
    if (r < 0xF8)
    {
        r += rounding & 7;
    }
    if (g < 0xFC)
    {
        g += (rounding >> 3) & 3;
    }
    if (b < 0xF8)
    {
        b += (rounding >> 5) & 7;
    }
    return getNativeColorFromRGB(r, g, b);
}

void PainterRGB565LinearGradient::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    uint16_t* p = reinterpret_cast<uint16_t*>(destination) + offset;

    if (isVertical)
    {
        // The whole line is the same color
        uint32_t color;
        bool noise = false;
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
            noise = true;
        }

        const uint16_t color565 = getNativeColor(color);
        uint16_t* const p_end = p + count;

        if (isSolid && (alpha == 255))
        {
            if (noise)
            {
                const uint8_t b = color & 0xFF;
                const uint8_t g = (color >> 8) & 0xFF;
                const uint8_t r = (color >> 16) & 0xFF;
                int x = widgetX;
                while (p < p_end)
                {
                    *p++ = applyNoise(x, widgetY, r, g, b);
                    x++;
                }
            }
            else
            {
                while (p < p_end)
                {
                    *p++ = color565;
                }
            }
        }
        else
        {
            const uint8_t a = (color >> 24) & 0xFF;
            const uint8_t alphaTot = LCD::div255(a * alpha);
            const uint16_t r = (color >> 8) & RMASK;
            const uint16_t g = (color >> 5) & GMASK;
            const uint16_t b = (color >> 3) & BMASK;
            while (p < p_end)
            {
                *p = alphaBlend(r, g, b, *p, alphaTot);
                p++;
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
    if (x <= vx0) // All pixels up to first gradient line
    {
        // Starting before gradient
        const int pixels = (int)(MIN(vx0 + 1 - x, count));
        // Start in correct end of palette depending on direction
        const uint32_t colorStart = texture[deltaColor > 0 ? 0 : 1023];
        uint16_t* const p_end = p + pixels;
        if (isSolid && (alpha == 255))
        {
            const uint16_t color565 = getNativeColor(colorStart);
            while (p < p_end)
            {
                *p++ = color565;
            }
        }
        else
        {
            const uint8_t a = (colorStart >> 24) & 0xFF;
            const uint8_t alphaTot = LCD::div255(a * alpha);
            const uint16_t r = (colorStart >> 8) & RMASK;
            const uint16_t g = (colorStart >> 5) & GMASK;
            const uint16_t b = (colorStart >> 3) & BMASK;
            while (p < p_end)
            {
                *p = alphaBlend(r, g, b, *p, alphaTot);
                p++;
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
            const uint16_t* const p_end = p + (endx - x);

            // Start with first or last color depending on direction
            float colorF = deltaColor > 0 ? 0.0000f : 1023.9999f;
            colorF += (x - vx0) * deltaColor;

            if (isSolid && (alpha == 255))
            {
                while (p < p_end)
                {
                    const int colorIndex = (int)colorF;
                    const uint32_t color = texture[colorIndex];
                    const uint8_t b = color & 0xFF;
                    const uint8_t g = (color >> 8) & 0xFF;
                    const uint8_t r = (color >> 16) & 0xFF;
                    *p++ = applyNoise(colorIndex, widgetY, r, g, b);
                    colorF += deltaColor;
                }
            }
            else
            {
                while (p < p_end)
                {
                    const int colorIndex = (int)colorF;

                    const uint32_t color = texture[colorIndex];
                    const uint8_t a = color >> 24;
                    const uint8_t alphaTot = LCD::div255(a * alpha);
                    const uint16_t r = (color >> 8) & RMASK;
                    const uint16_t g = (color >> 5) & GMASK;
                    const uint16_t b = (color >> 3) & BMASK;
                    *p = alphaBlend(r, g, b, *p, alphaTot);
                    p++;
                    colorF += deltaColor;
                }
            }
            x = endx;
        }

        if (x < right) // Second fixed color part
        {
            const uint16_t* const p_end = p + (right - x);
            const uint32_t colorEnd = texture[deltaColor > 0 ? 1023 : 0];
            const uint16_t color565 = getNativeColor(colorEnd);
            if (isSolid && (alpha == 255))
            {
                while (p < p_end)
                {
                    *p++ = color565;
                }
            }
            else
            {
                const uint8_t a = (colorEnd >> 24) & 0xFF;
                const uint8_t alphaTot = LCD::div255(a * alpha);
                const uint16_t r = (colorEnd >> 8) & RMASK;
                const uint16_t g = (colorEnd >> 5) & GMASK;
                const uint16_t b = (colorEnd >> 3) & BMASK;
                while (p < p_end)
                {
                    *p = alphaBlend(r, g, b, *p, alphaTot);
                    p++;
                };
            }
        }
    }
}

void PainterRGB565LinearGradient::tearDown() const
{
}
} // namespace touchgfx
