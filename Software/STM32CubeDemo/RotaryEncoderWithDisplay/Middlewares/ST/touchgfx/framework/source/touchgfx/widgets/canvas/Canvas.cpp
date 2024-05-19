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

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include <touchgfx/canvas_widget_renderer/Rasterizer.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/Canvas.hpp>

namespace touchgfx
{
Canvas::Canvas(const AbstractPainter* const painter, const Rect& canvasAreaAbs, const Rect& invalidatedAreaRel, uint8_t globalAlpha)
    : canvasPainter(painter),
      canvasAreaWidth(canvasAreaAbs.width),
      canvasAlpha(globalAlpha),
      rasterizer(),
      invalidatedAreaX(0),
      invalidatedAreaY(0),
      invalidatedAreaWidth(0),
      invalidatedAreaHeight(0),
      isPenDown(false),
      wasPenDown(false),
      previousX(0),
      previousY(0),
      previousOutside(0),
      penDownOutside(0),
      initialMoveToX(0),
      initialMoveToY(0)
{
    assert(CanvasWidgetRenderer::hasBuffer() && "No buffer allocated for CanvasWidgetRenderer drawing");
    assert(Rasterizer::POLY_BASE_SHIFT == 5 && "CanvasWidget assumes Q5 but Rasterizer uses a different setting");

    Rect dirtyArea = invalidatedAreaRel;
    dirtyAreaAbsolute = Rect(canvasAreaAbs.x + invalidatedAreaRel.x,
                             canvasAreaAbs.y + invalidatedAreaRel.y,
                             invalidatedAreaRel.width,
                             invalidatedAreaRel.height);

    // Transform rects to match framebuffer coordinates
    // This is needed if the display is rotated compared to the framebuffer
    DisplayTransformation::transformDisplayToFrameBuffer(dirtyArea, canvasAreaAbs);
    DisplayTransformation::transformDisplayToFrameBuffer(dirtyAreaAbsolute);

    // Re-size buffers for optimum memory buffer layout.
    rasterizer.reset(dirtyArea.x, dirtyArea.y);

    invalidatedAreaX = CWRUtil::toQ5<int>(dirtyArea.x);
    invalidatedAreaY = CWRUtil::toQ5<int>(dirtyArea.y);
    invalidatedAreaWidth = CWRUtil::toQ5<int>(dirtyArea.width);
    invalidatedAreaHeight = CWRUtil::toQ5<int>(dirtyArea.height);

    rasterizer.setMaxRender(dirtyAreaAbsolute.width, dirtyAreaAbsolute.height);
}

void Canvas::moveTo(CWRUtil::Q5 x, CWRUtil::Q5 y)
{
    if (isPenDown)
    {
        if (!close())
        {
            return;
        }
    }

    transformFrameBufferToDisplay(x, y);
    x -= invalidatedAreaX;
    y -= invalidatedAreaY;

    const uint8_t outside = isOutside(x, y, invalidatedAreaWidth, invalidatedAreaHeight);

    if (outside)
    {
        isPenDown = false;
    }
    else
    {
        penDownOutside = outside;
        rasterizer.moveTo(x, y);
        isPenDown = true;
        wasPenDown = true;
    }

    initialMoveToX = x;
    initialMoveToY = y;

    previousX = x;
    previousY = y;
    previousOutside = outside;
}

void Canvas::lineTo(CWRUtil::Q5 x, CWRUtil::Q5 y)
{
    transformFrameBufferToDisplay(x, y);
    x -= invalidatedAreaX;
    y -= invalidatedAreaY;

    uint8_t outside = isOutside(x, y, invalidatedAreaWidth, invalidatedAreaHeight);

    if (!previousOutside)
    {
        rasterizer.lineTo(x, y);
    }
    else
    {
        if (!outside || !(previousOutside & outside))
        {
            // x,y is inside, or on another side compared to previous
            if (!isPenDown)
            {
                penDownOutside = previousOutside;
                rasterizer.moveTo(previousX, previousY);
                isPenDown = true;
                wasPenDown = true;
            }
            else
            {
                rasterizer.lineTo(previousX, previousY);
            }
            rasterizer.lineTo(x, y);
        }
        else
        {
            // Restrict "outside" to the same side as previous point.
            outside &= previousOutside;
        }
    }
    previousX = x;
    previousY = y;
    previousOutside = outside;
}

bool Canvas::close()
{
    if (isPenDown)
    {
        if (previousOutside & penDownOutside)
        {
            // We are outside on the same side as we started. No need
            // to close the path, CWR will do this for us.
            //   lineTo(penDownX, penDownY);
        }
        else
        {
            if (previousOutside)
            {
                rasterizer.lineTo(previousX, previousY);
            }
            rasterizer.lineTo(initialMoveToX, initialMoveToY);
        }
    }
    isPenDown = false;
    return !rasterizer.wasOutlineTooComplex();
}

bool Canvas::render(uint8_t customAlpha)
{
    const uint8_t alpha = LCD::div255(canvasAlpha * customAlpha);
    if (alpha == 0 || !wasPenDown)
    {
        return true; // Nothing. Done
    }

    // If the invalidated rect is too wide compared to the allocated buffer for CWR,
    // redrawing will not help. The CanvasWidget needs to know about this situation
    // and maybe try to divide the area vertically instead, but this has not been
    // implemented. And probably should not.
    if (!close())
    {
        return false;
    }

    // Create the rendering buffer
    uint8_t* RESTRICT framebuffer = reinterpret_cast<uint8_t*>(HAL::getInstance()->lockFrameBufferForRenderingMethod(canvasPainter->getRenderingMethod()));
    const int stride = HAL::lcd().framebufferStride();
    uint8_t xAdjust = 0;
    switch (HAL::lcd().framebufferFormat())
    {
    case Bitmap::BW:
        framebuffer += (dirtyAreaAbsolute.x / 8) + dirtyAreaAbsolute.y * stride;
        xAdjust = dirtyAreaAbsolute.x % 8;
        break;
    case Bitmap::GRAY2:
        framebuffer += (dirtyAreaAbsolute.x / 4) + dirtyAreaAbsolute.y * stride;
        xAdjust = dirtyAreaAbsolute.x % 4;
        break;
    case Bitmap::GRAY4:
        framebuffer += (dirtyAreaAbsolute.x / 2) + dirtyAreaAbsolute.y * stride;
        xAdjust = dirtyAreaAbsolute.x % 2;
        break;
    case Bitmap::RGB565:
        framebuffer += dirtyAreaAbsolute.x * 2 + dirtyAreaAbsolute.y * stride;
        break;
    case Bitmap::RGB888:
        framebuffer += dirtyAreaAbsolute.x * 3 + dirtyAreaAbsolute.y * stride;
        break;
    case Bitmap::RGBA2222:
    case Bitmap::BGRA2222:
    case Bitmap::ARGB2222:
    case Bitmap::ABGR2222:
    case Bitmap::L8:
        framebuffer += dirtyAreaAbsolute.x + dirtyAreaAbsolute.y * stride;
        break;
    case Bitmap::ARGB8888:
        framebuffer += dirtyAreaAbsolute.x * 4 + dirtyAreaAbsolute.y * stride;
        break;
    case Bitmap::BW_RLE:
    case Bitmap::A4:
    case Bitmap::CUSTOM:
        assert(false && "Unsupported bit depth");
    }
    const bool result = rasterizer.render(canvasPainter, framebuffer, stride, xAdjust, alpha);
    canvasPainter->tearDown();
    HAL::getInstance()->unlockFrameBuffer();
    return result;
}

void Canvas::transformFrameBufferToDisplay(CWRUtil::Q5& x, CWRUtil::Q5& y) const
{
    if (HAL::DISPLAY_ROTATION == rotate90)
    {
        CWRUtil::Q5 const tmpY = y;
        y = CWRUtil::toQ5<int>(canvasAreaWidth) - x;
        x = tmpY;
    }
}

// Note: if you change these values, check the commented usage below
//#define curve_angle_tolerance_epsilon 0.01f
#define curve_collinearity_epsilon 1e-10f
#define curve_recursion_limit 8

#define m_distance_tolerance 0.25f
#define m_angle_tolerance 0.1f

void Canvas::recursiveQuadraticBezier(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const unsigned level)
{
    if (level > curve_recursion_limit)
    {
        return;
    }

    // Calculate all the mid-points of the line segments
    //----------------------
    const float x12 = (x1 + x2) / 2;
    const float y12 = (y1 + y2) / 2;
    const float x23 = (x2 + x3) / 2;
    const float y23 = (y2 + y3) / 2;
    const float x123 = (x12 + x23) / 2;
    const float y123 = (y12 + y23) / 2;

    float dx = x3 - x1;
    float dy = y3 - y1;
    const float d = abs(((x2 - x3) * dy - (y2 - y3) * dx));

    if (d > curve_collinearity_epsilon)
    {
        // Regular care
        //-----------------
        if (d * d <= m_distance_tolerance * (dx * dx + dy * dy))
        {
            // If the curvature doesn't exceed the distance_tolerance value
            // we tend to finish subdivisions.
            //----------------------
            // FGC: lint does not like this
            // if (m_angle_tolerance < curve_angle_tolerance_epsilon)
            // {
            //     lineTo(x123, y123);
            //     return;
            // }

            // Angle & Cusp Condition
            //----------------------
            float da = abs(atan2f(y3 - y2, x3 - x2) - atan2f(y2 - y1, x2 - x1));
            if (da >= PI)
            {
                da = 2 * PI - da;
            }

            if (da < m_angle_tolerance)
            {
                // Finally we can stop the recursion
                //----------------------
                lineTo(x123, y123);
                return;
            }
        }
    }
    else
    {
        // Collinear case
        //-----------------
        dx = x123 - (x1 + x3) / 2;
        dy = y123 - (y1 + y3) / 2;
        if (dx * dx + dy * dy <= m_distance_tolerance)
        {
            lineTo(x123, y123);
            return;
        }
    }

    // Continue subdivision
    //----------------------
    recursiveQuadraticBezier(x1, y1, x12, y12, x123, y123, level + 1);
    recursiveQuadraticBezier(x123, y123, x23, y23, x3, y3, level + 1);
}

//#define m_cusp_limit 0.0f

void Canvas::recursiveCubicBezier(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4, const unsigned level)
{
    if (level > curve_recursion_limit)
    {
        return;
    }

    // Calculate all the mid-points of the line segments
    //----------------------
    const float x12 = (x1 + x2) / 2;
    const float y12 = (y1 + y2) / 2;
    const float x23 = (x2 + x3) / 2;
    const float y23 = (y2 + y3) / 2;
    const float x34 = (x3 + x4) / 2;
    const float y34 = (y3 + y4) / 2;
    const float x123 = (x12 + x23) / 2;
    const float y123 = (y12 + y23) / 2;
    const float x234 = (x23 + x34) / 2;
    const float y234 = (y23 + y34) / 2;
    const float x1234 = (x123 + x234) / 2;
    const float y1234 = (y123 + y234) / 2;

    if (level > 0) // Enforce subdivision first time
    {
        // Try to approximate the full cubic curve by a single straight line
        //------------------
        float dx = x4 - x1;
        float dy = y4 - y1;

        const float d2 = abs(((x2 - x4) * dy - (y2 - y4) * dx));
        const float d3 = abs(((x3 - x4) * dy - (y3 - y4) * dx));

        float da1, da2;

        if (d2 > curve_collinearity_epsilon && d3 > curve_collinearity_epsilon)
        {
            // Regular care
            //-----------------
            if ((d2 + d3) * (d2 + d3) <= m_distance_tolerance * (dx * dx + dy * dy))
            {
                // If the curvature doesn't exceed the distance_tolerance value
                // we tend to finish subdivisions.
                //----------------------

                // FGC: lint does not like this
                // if (m_angle_tolerance < curve_angle_tolerance_epsilon)
                // {
                //     lineTo(x1234, y1234);
                //     return;
                // }

                // Angle & Cusp Condition
                //----------------------
                const float a23 = atan2f(y3 - y2, x3 - x2);
                da1 = abs(a23 - atan2f(y2 - y1, x2 - x1));
                da2 = abs(atan2f(y4 - y3, x4 - x3) - a23);
                if (da1 >= PI)
                {
                    da1 = 2 * PI - da1;
                }
                if (da2 >= PI)
                {
                    da2 = 2 * PI - da2;
                }

                if (da1 + da2 < m_angle_tolerance)
                {
                    // Finally we can stop the recursion
                    //----------------------
                    lineTo(x1234, y1234);
                    return;
                }

                // FGC: lint does not like this
                // if (m_cusp_limit != 0.0f)
                // {
                //     if (da1 > m_cusp_limit)
                //     {
                //         lineTo(x2, y2);
                //         return;
                //     }

                //     if (da2 > m_cusp_limit)
                //     {
                //         lineTo(x3, y3);
                //         return;
                //     }
                // }
            }
        }
        else
        {
            if (d2 > curve_collinearity_epsilon)
            {
                // p1,p3,p4 are collinear, p2 is considerable
                //----------------------
                if (d2 * d2 <= m_distance_tolerance * (dx * dx + dy * dy))
                {
                    // FGC: lint does not like this
                    // if (m_angle_tolerance < curve_angle_tolerance_epsilon)
                    // {
                    //     lineTo(x1234, y1234);
                    //     return;
                    // }

                    // Angle Condition
                    //----------------------
                    da1 = abs(atan2f(y3 - y2, x3 - x2) - atan2f(y2 - y1, x2 - x1));
                    if (da1 >= PI)
                    {
                        da1 = 2 * PI - da1;
                    }

                    if (da1 < m_angle_tolerance)
                    {
                        lineTo(x2, y2);
                        lineTo(x3, y3);
                        return;
                    }

                    // FGC: lint does not like this
                    // if (m_cusp_limit != 0.0f)
                    // {
                    //     if (da1 > m_cusp_limit)
                    //     {
                    //         lineTo(x2, y2);
                    //         return;
                    //     }
                    // }
                }
            }
            else if (d3 > curve_collinearity_epsilon)
            {
                // p1,p2,p4 are collinear, p3 is considerable
                //----------------------
                if (d3 * d3 <= m_distance_tolerance * (dx * dx + dy * dy))
                {
                    // FGC: lint does not like this
                    // if (m_angle_tolerance < curve_angle_tolerance_epsilon)
                    // {
                    //     lineTo(x1234, y1234);
                    //     return;
                    // }

                    // Angle Condition
                    //----------------------
                    da1 = abs(atan2f(y4 - y3, x4 - x3) - atan2f(y3 - y2, x3 - x2));
                    if (da1 >= PI)
                    {
                        da1 = 2 * PI - da1;
                    }

                    if (da1 < m_angle_tolerance)
                    {
                        lineTo(x2, y2);
                        lineTo(x3, y3);
                        return;
                    }

                    // FGC: lint does not like this
                    // if (m_cusp_limit != 0.0f)
                    // {
                    //     if (da1 > m_cusp_limit)
                    //     {
                    //         lineTo(x3, y3);
                    //         return;
                    //     }
                    // }
                }
            }
            else
            {
                // Collinear case
                //-----------------
                dx = x1234 - (x1 + x4) / 2;
                dy = y1234 - (y1 + y4) / 2;
                if (dx * dx + dy * dy <= m_distance_tolerance)
                {
                    lineTo(x1234, y1234);
                    return;
                }
            }
        }
    }

    // Continue subdivision
    //----------------------
    recursiveCubicBezier(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1);
    recursiveCubicBezier(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1);
}

} // namespace touchgfx
