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
 * Defines the touchgfx::VGShape, and the touchgfx::VGObject structs.
 *
 */
#ifndef TOUCHGFX_VGDATA_HPP
#define TOUCHGFX_VGDATA_HPP

#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{

/**
 * A list of vector graphics polygon filling modes.
 */
enum VGFillMode
{
    VG_FILL_EVEN_ODD, ///< Fill the paths using the even-odd filling rule
    VG_FILL_NON_ZERO  ///< Fill the paths using the non-zero filling rule
};

/**
 * A list of coloring modes. Each shape can have 2 coloring modes. One
 * for filling and one for stroking.
 */
enum VGColorMode
{
    VG_NONE,           ///< No coloring
    VG_FIXED_COLOR,    ///< Color with a fixed color in the whole shape
    VG_LINEAR_GRADIENT ///< Color using a linear gradient
};

/**
 * Values that represent the different ways strokes lines can be joined. Bevel is a simple cut off,
 * Miter extends into a spike (possibly falling back to Bevel if the miter limit for the stroke
 * is exceeded) and Round is a circle arc.
 */
enum VGStrokeLineJoin
{
    VG_STROKE_LINEJOIN_BEVEL,
    VG_STROKE_LINEJOIN_MITER,
    VG_STROKE_LINEJOIN_ROUND
};

/**
 * Values that represent the different ways stroke lines can end. Butt is a simpe cut off, Round
 * is a half circle and Square extends the line by half stroke width.
 */
enum VGStrokeLineCap
{
    VG_STROKE_LINECAP_BUTT,
    VG_STROKE_LINECAP_ROUND,
    VG_STROKE_LINECAP_SQUARE
};

/**
 * Fixed color paint definition.
 */
struct VGFixedColor
{
    uint32_t argb; ///< The color in ARGB8888 format
};

/**
 * Linear gradient paint definition.
 */
struct VGLinearGradient
{
    const uint32_t* palette;    ///< 1024 colors in ARGB888
    uint32_t stops;             ///< Number of stops (colors)
    const float* stopPositions; ///< Stop positions on the vector
    const uint32_t* colors;     ///< Stop colors
    float x0;                   ///< The gradient line start x
    float y0;                   ///< The gradient line start y
    float x1;                   ///< The gradient line end x
    float y1;                   ///< The gradient line end y
    bool isSolid;               ///< True if all colors are solid
};

/**
 * Data structure for a vector shape (polygon, circle, path, etc.).
 *
 * Shapes can be linked in a list using the next member. Shapes are linked to by a VGObject.
 * Which thereby consists of a number of Shapes. Each shape has attributes, e.g. strokeWidth,
 * that is used when drawing the paths in the Shape.
 *
 * All coordinates and sizes (e.g. strokeWidth) are not translated by any transform given in the
 * svg. This has to be done when drawing.
 */
struct VGShape
{
    uint16_t numCommands;            ///< Number of commands in the paths
    uint16_t numPoints;              ///< Number of coordinate floats in the paths
    const uint8_t* commands;         ///< The paths commands
    const float* points;             ///< The paths coordinates
    VGFillMode drawingMode;          ///< Polygon fill mode for this shape
    VGColorMode fillMode;            ///< Fill coloring mode for this shape
    VGColorMode strokeMode;          ///< Stroke coloring mode for this shape
    const void* fillPaint;           ///< Filling painter object or null if not filled
    const void* strokePaint;         ///< Stroking painter object or null if not stroked
    float strokeWidth;               ///< Stroke width to use when drawing this shape
    VGStrokeLineJoin strokeLineJoin; ///< The stroke line join
    VGStrokeLineCap strokeLineCap;   ///< The stroke line cap visual
    float strokeMiterLimit;          ///< The stroke miter limit
    uint8_t fillAlpha;               ///< Alpha of this shape's fill in the range [0;255]
    uint8_t strokeAlpha;             ///< Alpha of this shape's stroke in the range [0;255]
    float boundingbox[4];            ///< Un-transformed bounding box for this Shape [xmin, ymin, xmax, ymax] excluding strokeWidth
    float transformation[6];         ///< The transformation matrix to apply before drawing
    const VGShape* next;             ///< Pointer to the next Shape
};

/**
 * Data structure for a vector drawing.
 *
 * VGObject drawings consists of a linked list of VGShapes.
 */
struct VGObject
{
    float x;              ///< The x coordinate
    float y;              ///< The y coordinate
    float width;          ///< The width including strokeWidth
    float height;         ///< The height including strokeWidth
    float imageWidth;     ///< Width of the image (either calculated or from SVG width attribute)
    float imageHeight;    ///< Height of the image  (either calculated or from SVG height attribute)
    const VGShape* shape; ///< First shape in this drawing
};

} // namespace touchgfx

#endif // TOUCHGFX_VGDATA_HPP
