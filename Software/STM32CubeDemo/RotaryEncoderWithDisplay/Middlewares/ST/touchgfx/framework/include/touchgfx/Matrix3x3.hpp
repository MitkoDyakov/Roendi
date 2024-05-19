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
 * @file touchgfx/Matrix3x3.hpp
 *
 * Declares touchgfx::Matrix3x3 class and touchgfx::Matrix3x3::Point struct.
 */
#ifndef TOUCHGFX_MATRIX3X3_HPP
#define TOUCHGFX_MATRIX3X3_HPP

#include <touchgfx/hal/Config.hpp>

namespace touchgfx
{
/**
 * Class representing a 3x3 matrix of floats.
 *
 * The class is used to define affine transformations for 2D graphics.
 */
class Matrix3x3
{
public:
    /** A simple struct containing coordinates. */
    struct Point
    {
        float x; ///< The x coordinate
        float y; ///< The y coordinate
    };

    /**
     * Initialize a new instance of the Matrix3x3 class.
     * The matrix is initialized to the identity matrix.
     */
    Matrix3x3();

    /**
     * Get an element.
     *
     * @param  row    The row (0-2).
     * @param  column The column (0-2).
     *
     * @return The element.
     */
    FORCE_INLINE_FUNCTION float getElement(int row, int column) const
    {
        return elements[row][column];
    }

    /**
     * Get the internal matrix elements.
     *
     * Return value     Internal representation
     * elements[0]  <=  elements[0][0]
     * elements[1]  <=  elements[0][1]
     * elements[2]  <=  elements[0][2]
     * elements[3]  <=  elements[1][0]
     * elements[4]  <=  elements[1][1]
     * elements[5]  <=  elements[1][2]
     * elements[6]  <=  elements[2][0]
     * elements[7]  <=  elements[2][1]
     * elements[8]  <=  elements[2][2]
     *
     * @return A pointer to the internal matrix elements.
     */
    FORCE_INLINE_FUNCTION float* getElements()
    {
        return &elements[0][0];
    }

    /**
     * Set an element.
     *
     * @param  row    The row.
     * @param  column The column.
     * @param  value  The value.
     */
    FORCE_INLINE_FUNCTION void setElement(int row, int column, float value)
    {
        elements[row][column] = value;
    }

    /**
     * Reset to identity matrix
     */
    void reset();

    /**
     * Sets affine transformation from a row-major matrix.
     *
     * @param  transformation The transformation.
     */
    void setAffineTransformation(const float transformation[6])
    {
        elements[0][0] = transformation[0];
        elements[0][1] = transformation[2];
        elements[0][2] = transformation[4];
        elements[1][0] = transformation[1];
        elements[1][1] = transformation[3];
        elements[1][2] = transformation[5];
        elements[2][0] = 0.f;
        elements[2][1] = 0.f;
        elements[2][2] = 1.f;
    }

    /**
     * Apply scale transformation
     *
     * @param scaleX Scale factor in the direction of X
     * @param scaleY Scale factor in the direction of Y
     *
     */
    void scale(float scaleX, float scaleY);

    /**
     * Apply scale transformation
     *
     * @param scaleXY Scale factor in the direction of both X and Y
     *
     */
    void scale(float scaleXY)
    {
        scale(scaleXY, scaleXY);
    }

    /**
     * Apply rotation transformation
     *
     * @param angleInDegrees Angle to rotate in degrees (positive => clockwise, negative => counterclockwise)
     */
    void rotate(float angleInDegrees);

    /**
     * Apply rotation around a pivot point.
     *
     * @param  angleInDegrees Angle to rotate in degrees (positive => clockwise, negative =>
     *                        counterclockwise)
     * @param  x              X coordinate of the pivot point.
     * @param  y              Y coordinate of the pivot point.
     *
     * @note This will overwrite the matrix elements set prior to this call.
     */
    void rotatePivot(float angleInDegrees, float x, float y);

    /**
     * Apply translate transformation
     *
     * @param deltaX Delta distance in the direction of X
     * @param deltaY Delta distance in the direction of Y
     */
    void translate(float deltaX, float deltaY);

    /**
     * Do projective transformation of 2D point
     *
     * Projective transformation does not preserve parallelism, length, and angle.
     * But it still preserves collinearity and incidence.
     *
     * The Matrix includes the projective part (g, h, i), see below.
     *
     * |x'|   |a b c|   |x/w|
     * |y'| = |d e f| * |y/w|
     * |1 |   |g h i|   |1/w|
     *
     * w = g*x + h*y + i
     * x = x coordinate of the 2D point
     * y = y coordinate of the 2D point
     * x' = x coordinate of the transformed 2D point
     * y' = y coordinate of the transformed 2D point
     *
     * @param point 2D Point to transform
     *
     * @return The transformed 2D point
     */
    Matrix3x3::Point projectiveTransform(const Matrix3x3::Point& point) const;

    /**
     * Do affine transformation of 2D point
     *
     * Affine transformation is a special case of the projective transformation,
     * it has the same properties. However unlike projective transformation,
     * it preserves parallelism.
     *
     * The Matrix does not include the projective part (g, h, i), see below.
     *
     * |x'| = |a b c| * |x|
     * |y'|   |d e f|   |y|
     *                  |1|
     *
     * x = x coordinate of the 2D point
     * y = y coordinate of the 2D point
     * x' = x coordinate of the transformed 2D point
     * y' = y coordinate of the transformed 2D point
     *
     * @param point 2D Point to transform
     *
     * @return The transformed 2D point
     */
    Matrix3x3::Point affineTransform(const Matrix3x3::Point& point) const;

    /**
     * Do affine transformation of 2D point
     *
     * Affine transformation is a special case of the projective transformation,
     * it has the same properties. However unlike projective transformation,
     * it preserves parallelism.
     *
     * The Matrix does not include the projective part (g, h, i), see below.
     *
     * |x'| = |a b c| * |x|
     * |y'|   |d e f|   |y|
     *                  |1|
     *
     * x = x coordinate of the 2D point
     * y = y coordinate of the 2D point
     * x' = x coordinate of the transformed 2D point
     * y' = y coordinate of the transformed 2D point
     *
     * @param x X-coordinate of the point to transform
     * @param y Y-coordinate of the point to transform
     *
     * @return The transformed 2D point
     */
    Matrix3x3::Point affineTransform(const float x, const float y) const;

    /**
     * Multiply with another Matrix3x3
     *
     * @param rhs Right hand side Matrix3x3 to multiply with
     *
     * @return The resulting Matrix3x3
     */
    Matrix3x3 multiply(const Matrix3x3& rhs) const;

private:
    float elements[3][3]; ///< The matrix elements
};

} // namespace touchgfx

#endif // TOUCHGFX_MATRIX3X3_HPP
