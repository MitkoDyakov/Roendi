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
 * @file touchgfx/hal/VectorRenderer.hpp
 *
 * Defines the touchgfx::VectorRenderer class.
 */
#ifndef TOUCHGFX_VECTORRENDERER_HPP
#define TOUCHGFX_VECTORRENDERER_HPP

#include <touchgfx/Matrix3x3.hpp>
#include <touchgfx/hal/VGData.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{

/**
 * Abstract interface for classes implementing general 2D vector
 * drawing.
 *
 * Users of this interface are Widgets drawing vector graphics like
 * SVG images.
 *
 * Implementations draw the path given (in drawPath) using the last
 * color or gradient supplied with setColor or setLinearGradient.
 */
class VectorRenderer
{
public:
    /** Virtual destructor. */
    virtual ~VectorRenderer()
    {
    }

    /**
     * Gets the VectorRenderer instance.
     *
     * @return The VectorRenderer instance.
     */
    static VectorRenderer* getInstance();

    /**
     * Start a new drawing (invalidated area) within the given canvas area.
     * The canvas area typically corresponds to the area of a Widget and is
     * given in absolute coordinates, whereas the invalidated area is given
     * in relative coordinates.
     * Implementations may lock the framebuffer here.
     *
     * @param  canvasAreaAbs        The canvas dimensions in absolute coordinates.
     * @param  invalidatedAreaRel   The area which should be updated in relative coordinates to the canvas area.
     */
    virtual void setup(const Rect& canvasAreaAbs, const Rect& invalidatedAreaRel) = 0;

    /**
     * A list of vector graphics drawing modes.
     *
     */
    enum DrawMode
    {
        STROKE,        ///< Draw the outline of the paths
        FILL_EVEN_ODD, ///< Fill the paths using the even-odd filling rule
        FILL_NON_ZERO  ///< Fill the paths using the non-zero filling rule
    };

    /**
     * Set the drawing mode for the VectorRenderer. The mode will be
     * used for all subsequent drawings until setMode is called again.
     *
     * The default mode is non-zero filling.
     *
     * @param mode The filling mode.
     */
    virtual void setMode(DrawMode mode) = 0;

    /**
     * Sets stroke miter limit. This the max distance from the Miter point on the stroke to the
     * stroke point that will be used. If the miter limit is exceeded for a stroke join, it will not
     * be drawn as a Miter but as a Bevel.
     *
     * @param  miterLimit The miter limit.
     */
    virtual void setStrokeMiterLimit(float miterLimit) = 0;

    /**
     * Sets stroke line join type.
     *
     * @param  lineJoin The line join type.
     */
    virtual void setStrokeLineJoin(VGStrokeLineJoin lineJoin) = 0;

    /**
     * Sets stroke line cap type.
     *
     * @param  lineCap The line cap type.
     */
    virtual void setStrokeLineCap(VGStrokeLineCap lineCap) = 0;

    /**
     * Finalizes the current drawing. The implementation may block
     * here until the drawing is done.
     *
     * The implementation is required to unlock the framebuffer when
     * tearDown is called.
     */
    virtual void tearDown() = 0;

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
    virtual void drawPath(const uint8_t* cmds, uint32_t nCmds, const float* points, uint32_t nPoints, const float* bbox = 0) = 0;

    /**
     * Set the width used for future drawings using the VECTOR_STROKE
     * drawing mode.
     *
     * @param w The width used for stroke drawing.
     */
    virtual void setStrokeWidth(float w) = 0;

    /**
     * Set a constant color used for future drawings.
     *
     * @param c An ARGB color value
     */
    virtual void setColor(colortype c) = 0;

    /**
     * Set the alpha value (opacity) used for future drawings.
     * The alpha is specified in the interval [0;1].
     *
     * @param a The alpha value.
     */
    virtual void setAlpha(uint8_t a) = 0;

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
                                   const uint32_t* palette) = 0;

    /**
     * Sets the transformation matrix used for future drawings.
     * The transformation is reset when setup is called.
     *
     * @param m The transformation matrix
     *
     */
    virtual void setTransformationMatrix(const Matrix3x3& m) = 0;
};

} // namespace touchgfx

#endif // TOUCHGFX_VECTORRENDERER_HPP
