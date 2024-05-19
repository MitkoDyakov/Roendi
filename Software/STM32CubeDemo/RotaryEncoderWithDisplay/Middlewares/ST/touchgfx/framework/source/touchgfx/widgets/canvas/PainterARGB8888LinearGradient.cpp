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
#include <touchgfx/widgets/canvas/PainterARGB8888LinearGradient.hpp>

namespace touchgfx
{
void PainterARGB8888LinearGradient::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t alpha) const
{
    uint32_t* p = reinterpret_cast<uint32_t*>(destination) + offset;

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

        uint32_t* const p_end = p + count;

        if (isSolid && (alpha == 255))
        {
            const int32_t solidColor = (0xFFU << 24) | color;

            while (p < p_end)
            {
                *p++ = solidColor;
            }
        }
        else
        {
            const uint8_t a = (color >> 24) & 0xFF;
            const uint8_t alphaFg = LCD::div255(a * alpha);

            if (alphaFg)
            {
                const uint8_t b = color & 0xFF;
                const uint8_t g = (color >> 8) & 0xFF;
                const uint8_t r = (color >> 16) & 0xFF;

                while (p < p_end)
                {
                    const uint32_t rgbBg = *p;
                    const uint8_t alphaBg = rgbBg >> 24;

                    // DMA2D blending algo
                    const uint8_t alphaMult = LCD::div255(alphaFg * alphaBg);
                    const uint8_t alphaOut = alphaFg + alphaBg - alphaMult;
                    const uint8_t blueOut = (b * alphaFg + (rgbBg & 0xFF) * (alphaBg - alphaMult)) / alphaOut;
                    const uint8_t greenOut = (g * alphaFg + ((rgbBg >> 8) & 0xFF) * (alphaBg - alphaMult)) / alphaOut;
                    const uint8_t redOut = (r * alphaFg + ((rgbBg >> 16) & 0xFF) * (alphaBg - alphaMult)) / alphaOut;

                    *p++ = (alphaOut << 24) | (redOut << 16) | (greenOut << 8) | blueOut;
                }
            };
        }
        return;
    }

    // vx0 is where first color line (y=clSlope*x+clOffset) intersects the horizontal line y=widgetY
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
        uint32_t* const p_end = p + pixels;
        if (isSolid && (alpha == 255))
        {
            const int32_t solidColor = (0xFFU << 24) | colorStart;

            while (p < p_end)
            {
                *p++ = solidColor;
            }
        }
        else
        {
            const uint8_t a = (colorStart >> 24) & 0xFF;
            const uint8_t alphaFg = LCD::div255(a * alpha);

            if (alphaFg)
            {
                const uint8_t b = colorStart & 0xFF;
                const uint8_t g = (colorStart >> 8) & 0xFF;
                const uint8_t r = (colorStart >> 16) & 0xFF;

                while (p < p_end)
                {
                    const uint32_t rgbBg = *p;
                    const uint8_t alphaBg = rgbBg >> 24;

                    // DMA2D blending algo
                    const uint8_t alphaMult = LCD::div255(alphaFg * alphaBg);
                    const uint8_t alphaOut = alphaFg + alphaBg - alphaMult;
                    const uint8_t blueOut = (b * alphaFg + (rgbBg & 0xFF) * (alphaBg - alphaMult)) / alphaOut;
                    const uint8_t greenOut = (g * alphaFg + ((rgbBg >> 8) & 0xFF) * (alphaBg - alphaMult)) / alphaOut;
                    const uint8_t redOut = (r * alphaFg + ((rgbBg >> 16) & 0xFF) * (alphaBg - alphaMult)) / alphaOut;

                    *p++ = (alphaOut << 24) | (redOut << 16) | (greenOut << 8) | blueOut;
                }
            };
        }
        x += pixels;
    }

    const int16_t right = widgetX + count;
    if (x < right) // More to draw
    {
        // vx1 is where y=widgetY intersects the last color line (fixed distance from vx0)
        const float vx1 = vx0 + horizontalDistance;

        if (x < vx1) // Draw gradient part until last color line
        {
            const int endx = (int)(MIN(right, vx1));
            const uint32_t* const p_end = p + (endx - x);

            // Start with first or last color depending on direction
            float colorF = deltaColor > 0 ? 0.0000f : 1023.9999f;
            colorF += (x - vx0) * deltaColor;

            if (isSolid && (alpha == 255))
            {
                while (p < p_end)
                {
                    const int colorIndex = (int)colorF;
                    const uint32_t* color = (const uint32_t*)(texture + colorIndex);

                    *p++ = (0xFFU << 24) | *color;

                    colorF += deltaColor;
                }
            }
            else
            {
                // Go to 8-bit pointers
                const uint32_t* const gradient = (const uint32_t*)texture;
                while (p < p_end)
                {
                    const int colorIndex = (int)colorF;

                    const uint32_t* color = gradient + colorIndex;
                    const uint8_t a = (*color >> 24) & 0xFF;
                    const uint8_t alphaFg = LCD::div255(a * alpha);

                    if (alphaFg)
                    {
                        const uint8_t b = *color & 0xFF;
                        const uint8_t g = (*color >> 8) & 0xFF;
                        const uint8_t r = (*color >> 16) & 0xFF;

                        const uint32_t rgbBg = *p;
                        const uint8_t alphaBg = rgbBg >> 24;

                        // DMA2D blending algo
                        const uint8_t alphaMult = LCD::div255(alphaFg * alphaBg);
                        const uint8_t alphaOut = alphaFg + alphaBg - alphaMult;
                        const uint8_t blueOut = (b * alphaFg + (rgbBg & 0xFF) * (alphaBg - alphaMult)) / alphaOut;
                        const uint8_t greenOut = (g * alphaFg + ((rgbBg >> 8) & 0xFF) * (alphaBg - alphaMult)) / alphaOut;
                        const uint8_t redOut = (r * alphaFg + ((rgbBg >> 16) & 0xFF) * (alphaBg - alphaMult)) / alphaOut;

                        *p++ = (alphaOut << 24) | (redOut << 16) | (greenOut << 8) | blueOut;
                    };

                    colorF += deltaColor;
                }
            }
            x = endx;
        }

        if (x < right) // Second fixed color part
        {
            const uint32_t* const p_end = p + (right - x);
            const uint32_t colorEnd = texture[deltaColor > 0 ? 1023 : 0];
            if (isSolid && (alpha == 255))
            {
                const int32_t solidColor = (0xFFU << 24) | colorEnd;

                while (p < p_end)
                {
                    *p++ = solidColor;
                }
            }
            else
            {
                const uint8_t b = colorEnd & 0xFF;
                const uint8_t g = (colorEnd >> 8) & 0xFF;
                const uint8_t r = (colorEnd >> 16) & 0xFF;

                const uint8_t a = (colorEnd >> 24) & 0xFF;
                const uint8_t alphaFg = LCD::div255(a * alpha);

                while (p < p_end)
                {
                    if (alphaFg)
                    {
                        const uint32_t rgbBg = *p;
                        const uint8_t alphaBg = rgbBg >> 24;

                        // DMA2D blending algo
                        const uint8_t alphaMult = LCD::div255(alphaFg * alphaBg);
                        const uint8_t alphaOut = alphaFg + alphaBg - alphaMult;
                        const uint8_t blueOut = (b * alphaFg + (rgbBg & 0xFF) * (alphaBg - alphaMult)) / alphaOut;
                        const uint8_t greenOut = (g * alphaFg + ((rgbBg >> 8) & 0xFF) * (alphaBg - alphaMult)) / alphaOut;
                        const uint8_t redOut = (r * alphaFg + ((rgbBg >> 16) & 0xFF) * (alphaBg - alphaMult)) / alphaOut;

                        *p++ = (alphaOut << 24) | (redOut << 16) | (greenOut << 8) | blueOut;
                    };
                };
            }
        }
    }
}

void PainterARGB8888LinearGradient::tearDown() const
{
}
} // namespace touchgfx
