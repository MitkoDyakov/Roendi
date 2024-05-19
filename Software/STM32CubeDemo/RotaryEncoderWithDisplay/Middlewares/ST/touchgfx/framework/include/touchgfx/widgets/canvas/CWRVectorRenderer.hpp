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
 * @file touchgfx/widgets/canvas/CWRVectorRenderer.hpp
 *
 * Defines the touchgfx::CWRVectorRenderer class.
 */
#ifndef TOUCHGFX_CWRVECTORRENDERER_HPP
#define TOUCHGFX_CWRVECTORRENDERER_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/hal/VectorRenderer.hpp>
#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterColor.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterLinearGradient.hpp>
#include <touchgfx/widgets/canvas/Canvas.hpp>
#include <touchgfx/widgets/canvas/CanvasWidget.hpp>
#include <touchgfx/widgets/canvas/PainterARGB8888.hpp>
#include <touchgfx/widgets/canvas/PainterARGB8888LinearGradient.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565LinearGradient.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888LinearGradient.hpp>

namespace touchgfx
{
/**
 * Implementation of the VectorRendererInterface based on the
 * CanvasWidgetRenderer. The class needs to be subclasses to provide
 * the painter objecs.
 *
 * @see CWRVectorRendererRGB888, CWRVectorRendererRGB565
 */
class CWRVectorRenderer : public VectorRenderer
{
public:
    /**
     * Start a new drawing (invalidated area) within the given canvas area.
     * The canvas area typically corresponds to the area of a Widget and is
     * given in absolute coordinates, whereas the invalidated area is given
     * in relative coordinates.
     * Implementations may lock the framebuffer here.
     *
     * @param  canvasAreaAbs        The canvas dimensions in absolute coordinates.
     * @param  invalidatedAreaRel   The area which should be updated in relative coordinates.
     */
    virtual void setup(const Rect& canvasAreaAbs, const Rect& invalidatedAreaRel);

    /**
     * Set the drawing mode for the VectorRenderer. The mode will be
     * used for all subsequent drawings until setMode is called again.
     *
     * The default mode is non-zero filling.
     *
     * @param mode The filling mode.
     */
    virtual void setMode(DrawMode mode)
    {
        drawMode = mode;
    }

    virtual void setStrokeMiterLimit(float miterLimit)
    {
        strokeMiterLimit = miterLimit;
    }

    virtual void setStrokeLineJoin(VGStrokeLineJoin lineJoin)
    {
        strokeLineJoin = lineJoin;
    }

    virtual void setStrokeLineCap(VGStrokeLineCap lineCap)
    {
        strokeLineCap = lineCap;
    }

    /**
     * Finalizes the current drawing. The implementation may block
     * here until the drawing is done.
     *
     * The implementation is required to unlock the framebuffer when
     * tearDown is called.
     */
    virtual void tearDown();

    /**
     * Draw a Path defined by the given array of primitives
     * (commands) and an array of the corresponding floating point
     * coordinates.
     *
     * @param cmds    An uint8_t array of VectorPrimitives values
     * @param nCmds   The number of commands in cmds
     * @param points  An array of coordinates for the commands
     * @param nPoints The number of values in the points arrays
     * @param bbox    Array describing the bounding box of the path [minx,miny,maxx,maxy] or 0 if unknown
     */
    virtual void drawPath(const uint8_t* cmds, uint32_t nCmds, const float* points, uint32_t nPoints, const float* bbox = 0);

    /**
     * Set the width used for future drawings using the VECTOR_STROKE
     * drawing mode.
     *
     * @param w The width used for stroke drawing.
     */
    virtual void setStrokeWidth(float w);

    /**
     * Set a constant color used for future drawings.
     *
     * @param c An ARGB color value
     */
    virtual void setColor(colortype c);

    /**
     * Set the alpha value (opacity) used for future drawings.
     * The alpha is specified in the interval [0;1].
     *
     * @param a The alpha value.
     */
    virtual void setAlpha(uint8_t a);

    /**
     * Set a linear gradient used for future drawings.
     * The linear gradient is transformed using the current
     * transformation matrix.
     * Multiple colors (stops) can be specified for the gradient. The
     * stop positions are given in the range [0;1].
     *
     * @param x0            X-coordinate for gradient starting point
     * @param y0            Y-coordinate for gradient starting point
     * @param x1            X-coordinate for gradient end point
     * @param y1            Y-coordinate for gradient end point
     * @param stops         Number of stops
     * @param stopPositions Positions of the stops on the line
     * @param stopColors    Colors of the stops
     * @param width         Width of the box to fill
     * @param height        Height of the box to fill
     * @param solid         True if all colors in the gradient are solid
     * @param palette       The gradient palette
     */
    virtual void setLinearGradient(float x0, float y0, float x1, float y1,
                                   uint32_t stops,
                                   const float* stopPositions,
                                   const uint32_t* stopColors,
                                   float width,
                                   float height,
                                   bool solid,
                                   const uint32_t* palette);

    /**
     * Sets the transformation matrix used for future drawings.
     * The transformation is reset when setup is called.
     *
     * @param m The transformation matrix
     *
     */
    virtual void setTransformationMatrix(const Matrix3x3& m);

private:
    Rect canvasAreaAbsolute;
    Rect dirtyAreaAbsolute;
    uint8_t canvasAlpha;
    const AbstractPainter* canvasPainter;

    DrawMode drawMode;
    uint8_t alpha;
    uint8_t colorAlpha;
    float strokeMiterLimit;
    VGStrokeLineJoin strokeLineJoin;
    VGStrokeLineCap strokeLineCap;

    Rect drawArea;
    float strokeWidth;
    Matrix3x3 matrix;

    class StrokeCanvas;
    virtual AbstractPainterLinearGradient& getLinearPainter() = 0;
    virtual AbstractPainterColor& getColorPainterColor() = 0;
    virtual AbstractPainter& getColorPainter() = 0;
    bool drawPathArea(const uint8_t* cmds, uint32_t nCmds, const float* points, uint32_t nPoints, const Rect& area) const;

    bool drawFill(const uint8_t* cmds, uint32_t nCmds, const float* points, uint32_t nPoints, const Rect& area) const;
    bool drawStroke(const uint8_t* cmds, uint32_t nCmds, const float* points, uint32_t nPoints, const Rect& area) const;
    void drawStrokeBackwards(uint32_t cmdInxPathStart, uint32_t cmdInx, uint32_t pointInx, const uint8_t* cmds, const float* points, bool drawLineCaps, StrokeCanvas& canvas) const;
    void getPreviousDestination(float& positionX, float& positionY, uint32_t cmdInx, uint32_t pointInx, const uint8_t* cmds, const float* points) const;

    class CanvasWidgetProxy : public CanvasWidget
    {
        virtual bool drawCanvasWidget(const Rect& invalidatedArea) const
        {
            return true;
        }

    public:
        void setParent(Drawable* p)
        {
            parent = p;
        }
    } proxyWidget;

    class StrokeCanvas : private Canvas
    {
    public:
        StrokeCanvas(const AbstractPainter* const painter, const Rect& canvasAreaAbs, const Rect& invalidatedAreaRel, uint8_t globalAlpha, const Matrix3x3& m)
            : Canvas(painter, canvasAreaAbs, invalidatedAreaRel, globalAlpha),
              matrix(m),
              firstLineTo(false), noLineHasBeenDrawn(true), drawLineCapInsteadOfLineJoin(false),
              strokeWidthHalf(1.0f), miterLimitSquared(0.0f), strokeLineJoin(VG_STROKE_LINEJOIN_BEVEL), strokeLineCap(VG_STROKE_LINECAP_BUTT), strokeAlpha(255),
              lastX(0.0f), lastY(0.0f),
              initialStrokeStartX(0.0f), initialStrokeStartY(0.0f), initialStrokeEndX(0.0f), initialStrokeEndY(0.0f),
              lastStrokeStartX(0.0f), lastStrokeStartY(0.0f), lastStrokeEndX(0.0f), lastStrokeEndY(0.0f)
        {
            setFillingRule(Rasterizer::FILL_NON_ZERO);
        }

        void strokeMoveTo(float x, float y);
        void strokeLineTo(float x, float y);
        bool strokeClose(float x, float y);
        bool strokeDrawDoubleLineCap();

        void strokeBezierQuad(float x0, float y0, const float x1, const float y1, const float x, const float y)
        {
            Canvas::quadraticBezierTo(x0, y0, x1, y1, x, y);
        }

        void strokeBezierCubic(float x0, float y0, float x1, float y1, float x2, float y2, float x, float y)
        {
            Canvas::cubicBezierTo(x0, y0, x1, y1, x2, y2, x, y);
        }

        bool strokeRender()
        {
            return Canvas::render(strokeAlpha);
        }

        void setStroke(float width, float miterLimit, VGStrokeLineJoin lineJoin, VGStrokeLineCap lineCap, uint8_t alpha)
        {
            strokeWidthHalf = abs(width) / 2.0f;
            miterLimitSquared = miterLimit * strokeWidthHalf * miterLimit * strokeWidthHalf;
            strokeLineJoin = lineJoin;
            strokeLineCap = lineCap;
            strokeAlpha = alpha;
        }

        void lineCapInsteadOfLineJoin(bool drawLineCaps);

        void drawFinalLineCap();

    private:
        const Matrix3x3& matrix;
        bool firstLineTo;
        bool noLineHasBeenDrawn;
        bool drawLineCapInsteadOfLineJoin;
        float strokeWidthHalf;
        float miterLimitSquared;
        VGStrokeLineJoin strokeLineJoin;
        VGStrokeLineCap strokeLineCap;
        uint8_t strokeAlpha;
        float lastX;
        float lastY;
        float initialStrokeStartX;
        float initialStrokeStartY;
        float initialStrokeEndX;
        float initialStrokeEndY;
        float lastStrokeStartX;
        float lastStrokeStartY;
        float lastStrokeEndX;
        float lastStrokeEndY;

        void strokeDrawLineJoin(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        void strokeDrawLineJoinRound(float x, float y, float x2, float y2, float x3, float y3, int level);
        void strokeDrawLineCap(float x2, float y2, float x3, float y3);

        virtual void lineTo(CWRUtil::Q5 x, CWRUtil::Q5 y)
        {
            // This function is called from the Canvas Bezier curve functions, so we need to call our own function to draw the stroke.
            strokeLineTo(x.to<float>(), y.to<float>());
        }

        void moveToWithTransform(float x, float y)
        {
            Matrix3x3::Point point = matrix.affineTransform(x, y);
            Canvas::moveTo(CWRUtil::toQ5<float>(point.x), CWRUtil::toQ5<float>(point.y));
        }

        void lineToWithTransform(float x, float y)
        {
            Matrix3x3::Point point = matrix.affineTransform(x, y);
            Canvas::lineTo(CWRUtil::toQ5<float>(point.x), CWRUtil::toQ5<float>(point.y));
        }
    };
};

/**
 * Implementation of the CWRVectorRendererInterface
 * for the RGB565 framebuffer format.
 */
class CWRVectorRendererRGB565 : public CWRVectorRenderer
{
    virtual AbstractPainterLinearGradient& getLinearPainter()
    {
        return linearPainter;
    }

    virtual AbstractPainterColor& getColorPainterColor()
    {
        return colorPainter;
    }

    virtual AbstractPainter& getColorPainter()
    {
        return colorPainter;
    }

private:
    PainterRGB565 colorPainter;
    PainterRGB565LinearGradient linearPainter;
};

/**
 * Implementation of the CWRVectorRendererInterface
 * for the RGB888 framebuffer format.
 */
class CWRVectorRendererRGB888 : public CWRVectorRenderer
{
    virtual AbstractPainterLinearGradient& getLinearPainter()
    {
        return linearPainter;
    }

    virtual AbstractPainterColor& getColorPainterColor()
    {
        return colorPainter;
    }

    virtual AbstractPainter& getColorPainter()
    {
        return colorPainter;
    }

private:
    PainterRGB888 colorPainter;
    PainterRGB888LinearGradient linearPainter;
};

/**
 * Implementation of the CWRVectorRendererInterface
 * for the ARGB8888 framebuffer format.
 */
class CWRVectorRendererARGB8888 : public CWRVectorRenderer
{
    virtual AbstractPainterLinearGradient& getLinearPainter()
    {
        return linearPainter;
    }

    virtual AbstractPainterColor& getColorPainterColor()
    {
        return colorPainter;
    }

    virtual AbstractPainter& getColorPainter()
    {
        return colorPainter;
    }

private:
    PainterARGB8888 colorPainter;
    PainterARGB8888LinearGradient linearPainter;
};

} // namespace touchgfx

#endif // TOUCHGFX_CWRVECTORRENDERER_HPP
