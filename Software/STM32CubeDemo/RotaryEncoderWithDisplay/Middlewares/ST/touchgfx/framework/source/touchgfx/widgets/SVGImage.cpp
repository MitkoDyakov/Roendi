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
#include <touchgfx/hal/VectorRenderer.hpp>
#include <touchgfx/widgets/SVGImage.hpp>

// Declarations, defined in SVGDatabase.cpp
namespace SVGDatabase
{
const touchgfx::VGObject* getInstance();
uint16_t getInstanceSize();
} // namespace SVGDatabase

namespace touchgfx
{
void SVGImage::setSVG(uint16_t id)
{
    if (id >= SVGDatabase::getInstanceSize())
    {
        svgId = SVG_INVALID;
        return;
    }

    svgId = id;
    reset();

    // If the widget is empty, resize it to the size of the SVG
    if (rect.isEmpty())
    {
        resizeToCurrentSVG();
    }
}

void SVGImage::resizeToCurrentSVG()
{
    if (svgId == SVG_INVALID)
    {
        return;
    }
    const VGObject* const table = SVGDatabase::getInstance();
    const VGObject& image = table[svgId];
    const Matrix3x3 matrix = getTransformationMatrix();
    // Transform bounding box corners
    const Matrix3x3::Point p1 = matrix.affineTransform(0.0f, 0.0f);
    const Matrix3x3::Point p2 = matrix.affineTransform(0.0f, image.imageHeight);
    const Matrix3x3::Point p3 = matrix.affineTransform(image.imageWidth, 0.0f);
    const Matrix3x3::Point p4 = matrix.affineTransform(image.imageWidth, image.imageHeight);
    // Find limits of transformed box
    const float x_max = MAX(MAX(p1.x, p2.x), MAX(p3.x, p4.x));
    const float y_max = MAX(MAX(p1.y, p2.y), MAX(p3.y, p4.y));
    setWidthHeight(static_cast<int16_t>(ceilf(x_max)), static_cast<int16_t>(ceilf(y_max)));
}

void SVGImage::reset()
{
    scaleX = 1.0f;
    scaleY = 1.0f;
    rotation = 0.0f;
    rotationCenterX = 0.0f;
    rotationCenterY = 0.0f;
    imagePositionX = 0.0f;
    imagePositionY = 0.0f;
}

float SVGImage::getImageWidth() const
{
    if (svgId == SVG_INVALID)
    {
        return 0.0f;
    }
    const VGObject* const table = SVGDatabase::getInstance();
    const VGObject& image = table[svgId];
    return image.imageWidth;
}

float SVGImage::getImageHeight() const
{
    if (svgId == SVG_INVALID)
    {
        return 0.0f;
    }
    const VGObject* const table = SVGDatabase::getInstance();
    const VGObject& image = table[svgId];
    return image.imageHeight;
}

void SVGImage::draw(const Rect& invalidatedArea) const
{
    if (svgId == SVG_INVALID)
    {
        return;
    }

    const VGObject* const table = SVGDatabase::getInstance();
    const VGObject& image = table[svgId];

    VectorRenderer* renderer = VectorRenderer::getInstance();
    assert(renderer && "No VectorRenderer instance found!");

    renderer->setup(getAbsoluteRect(), invalidatedArea);

    const Matrix3x3 matrix = getTransformationMatrix();
    for (const VGShape* shape = image.shape; shape != 0; shape = shape->next)
    {
        Matrix3x3 shapeMatrix;
        shapeMatrix.setAffineTransformation(shape->transformation);
        shapeMatrix = matrix.multiply(shapeMatrix);

        // Skip entire shape if bounding box does not cover invalidatedArea
        // Find bounding box including stroke width
        const float strokeWidthHalf = shape->strokeWidth / 2.0f;
        const float bb_xMin = shape->boundingbox[0] - strokeWidthHalf;
        const float bb_yMin = shape->boundingbox[1] - strokeWidthHalf;
        const float bb_xMax = shape->boundingbox[2] + strokeWidthHalf;
        const float bb_yMax = shape->boundingbox[3] + strokeWidthHalf;
        // Transform bounding box corners
        const Matrix3x3::Point p1 = shapeMatrix.affineTransform(bb_xMin, bb_yMin);
        const Matrix3x3::Point p2 = shapeMatrix.affineTransform(bb_xMin, bb_yMax);
        const Matrix3x3::Point p3 = shapeMatrix.affineTransform(bb_xMax, bb_yMin);
        const Matrix3x3::Point p4 = shapeMatrix.affineTransform(bb_xMax, bb_yMax);
        // Find limits of transformed box
        const int16_t x_min = static_cast<int16_t>(floorf(MIN(MIN(p1.x, p2.x), MIN(p3.x, p4.x))));
        const int16_t x_max = static_cast<int16_t>(ceilf(MAX(MAX(p1.x, p2.x), MAX(p3.x, p4.x))));
        const int16_t y_min = static_cast<int16_t>(floorf(MIN(MIN(p1.y, p2.y), MIN(p3.y, p4.y))));
        const int16_t y_max = static_cast<int16_t>(ceilf(MAX(MAX(p1.y, p2.y), MAX(p3.y, p4.y))));
        const Rect shapeBox(x_min, y_min, (x_max - x_min) + 1, (y_max - y_min) + 1);
        if (shapeBox.intersect(invalidatedArea))
        {
            renderer->setTransformationMatrix(shapeMatrix);

            // First fill
            if (shape->fillMode != VG_NONE)
            {
                // Set drawing mode and color for this shape
                renderer->setMode((shape->drawingMode == VG_FILL_EVEN_ODD) ? VectorRenderer::FILL_EVEN_ODD : VectorRenderer::FILL_NON_ZERO);
                renderer->setAlpha(shape->fillAlpha);

                // Set the fill color
                switch (shape->fillMode)
                {
                case VG_NONE:
                    break; // Guarded above
                case VG_FIXED_COLOR:
                    {
                        const VGFixedColor* color = reinterpret_cast<const VGFixedColor*>(shape->fillPaint);
                        renderer->setColor(color->argb);
                        break;
                    }
                case VG_LINEAR_GRADIENT:
                    {
                        //Set shape box size on linear (and radial) gradient
                        const float width = shape->boundingbox[2] - shape->boundingbox[0];
                        const float height = shape->boundingbox[3] - shape->boundingbox[1];

                        const VGLinearGradient* linear = reinterpret_cast<const VGLinearGradient*>(shape->fillPaint);
                        renderer->setLinearGradient(linear->x0, linear->y0, linear->x1, linear->y1,
                                                    linear->stops, linear->stopPositions, linear->colors,
                                                    width, height,
                                                    linear->isSolid, linear->palette);
                        break;
                    }
                }

                // Draw this Shape
                renderer->drawPath(shape->commands, shape->numCommands, shape->points, shape->numPoints, shape->boundingbox);
            }

            // Then stroke
            if (shape->strokeMode != VG_NONE)
            {
                // Stroking is nonzero
                renderer->setMode(VectorRenderer::STROKE);
                renderer->setStrokeWidth(shape->strokeWidth);
                renderer->setStrokeLineCap(shape->strokeLineCap);
                renderer->setStrokeLineJoin(shape->strokeLineJoin);
                renderer->setStrokeMiterLimit(shape->strokeMiterLimit);
                renderer->setAlpha(shape->strokeAlpha);

                // Set the fill color
                switch (shape->strokeMode)
                {
                case VG_NONE:
                    break; // Guarded above
                case VG_FIXED_COLOR:
                    {
                        const VGFixedColor* color = reinterpret_cast<const VGFixedColor*>(shape->strokePaint);
                        renderer->setColor(color->argb);
                        break;
                    }
                case VG_LINEAR_GRADIENT:
                    {
                        //Set shape box size on linear (and radial) gradient
                        const float width = shape->boundingbox[2] - shape->boundingbox[0] + shape->strokeWidth;
                        const float height = shape->boundingbox[3] - shape->boundingbox[1] + shape->strokeWidth;

                        const VGLinearGradient* linear = reinterpret_cast<const VGLinearGradient*>(shape->strokePaint);
                        renderer->setLinearGradient(linear->x0, linear->y0, linear->x1, linear->y1,
                                                    linear->stops, linear->stopPositions, linear->colors,
                                                    width, height,
                                                    linear->isSolid, linear->palette);
                        break;
                    }
                }

                // Draw this Shape
                const float boundingbox[4] = { shape->boundingbox[0] - strokeWidthHalf, shape->boundingbox[1] - strokeWidthHalf,
                                               shape->boundingbox[2] + strokeWidthHalf, shape->boundingbox[3] + strokeWidthHalf };
                renderer->drawPath(shape->commands, shape->numCommands, shape->points, shape->numPoints, boundingbox);
            }
        }
    }

    // Finalize rendering
    renderer->tearDown();
}

void SVGImage::invalidateContent() const
{
    if (!visible || svgId == SVG_INVALID)
    {
        return;
    }
    const VGObject* const table = SVGDatabase::getInstance();
    const VGObject& image = table[svgId];
    const Matrix3x3 matrix = getTransformationMatrix();
    // Transform bounding box corners
    const Matrix3x3::Point p1 = matrix.affineTransform(image.x, image.y);
    const Matrix3x3::Point p2 = matrix.affineTransform(image.x, image.y + image.height);
    const Matrix3x3::Point p3 = matrix.affineTransform(image.x + image.width, image.y);
    const Matrix3x3::Point p4 = matrix.affineTransform(image.x + image.width, image.y + image.height);
    // Find limits of transformed box
    const int16_t x_min = static_cast<int16_t>(floorf(MIN(MIN(p1.x, p2.x), MIN(p3.x, p4.x))));
    const int16_t x_max = static_cast<int16_t>(ceilf(MAX(MAX(p1.x, p2.x), MAX(p3.x, p4.x))));
    const int16_t y_min = static_cast<int16_t>(floorf(MIN(MIN(p1.y, p2.y), MIN(p3.y, p4.y))));
    const int16_t y_max = static_cast<int16_t>(ceilf(MAX(MAX(p1.y, p2.y), MAX(p3.y, p4.y))));
    Rect r(x_min, y_min, (x_max - x_min) + 1, (y_max - y_min) + 1);
    invalidateRect(r);
}

touchgfx::Matrix3x3 SVGImage::getTransformationMatrix() const
{
    Matrix3x3 matrix; // Implicit reset to identity matrix
    if (svgId != SVG_INVALID)
    {
        matrix.scale(scaleX, scaleY);
        matrix.translate(imagePositionX - rotationCenterX, imagePositionY - rotationCenterY);
        matrix.rotate(rotation);
        matrix.translate(rotationCenterX, rotationCenterY);
    }
    return matrix;
}

} // namespace touchgfx
