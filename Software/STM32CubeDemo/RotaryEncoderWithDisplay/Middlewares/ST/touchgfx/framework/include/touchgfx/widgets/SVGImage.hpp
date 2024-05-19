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
 * @file touchgfx/widgets/SVGImage.hpp
 *
 * Defines the touchgfx::SVGImage class.
 */
#ifndef TOUCHGFX_SVGIMAGE_HPP
#define TOUCHGFX_SVGIMAGE_HPP

#include <touchgfx/Matrix3x3.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/hal/VGData.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
const uint16_t SVG_INVALID = 0xFFFFU; ///< Define the svgId of an invalid SVG

/**
 * Simple widget capable of showing a SVG image on the display.
 *
 * @see Image
 */
class SVGImage : public Widget
{
public:
    /**
     * Constructs a new Image with a default SVG (undefined) if none is specified.
     *
     * @see setSVG
     */
    SVGImage()
        : Widget(), svgId(SVG_INVALID),
          scaleX(1.0f), scaleY(1.0f), rotation(0.0f), rotationCenterX(0.0f), rotationCenterY(0.0f),
          imagePositionX(0.0f), imagePositionY(0.0f)
    {
    }

    /**
     * Sets the SVG for this SVGImage and updates the width and height of this widget (if the widget
     * has not already been assigned a width and height) to match those of the SVG drawing. See the
     * file generated/images/include/images/SVGDatabase.hpp.
     *
     * @param  id The SVG ID.
     *
     * @see resizeToCurrentSVG
     *
     * @note The image is not resized and invalidated after setting the id.
     */
    virtual void setSVG(uint16_t id);

    /** Resize the Widget to match the size of the SVG drawing. */
    void resizeToCurrentSVG();

    /** Resets all adjustments made to the svg proportions. */
    void reset();

    /**
     * Gets SVG width as given in the SVG image file. This is the width before applying any rotation
     * and scaling.
     *
     * @return The SVG width.
     */
    float getImageWidth() const;

    /**
     * Gets SVG height as given in the SVG image file. This is the width before applying any
     * rotation and scaling.
     *
     * @return The SVG height.
     */
    float getImageHeight() const;

    /**
     * Sets SVG scale for both width and height.
     *
     * @param  scale The scale to apply to width and height.
     */
    void setScale(float scale)
    {
        setScale(scale, scale);
    }

    /**
     * Sets SVG scale for width and for height.
     *
     * @param  xScale The scale for width.
     * @param  yScale The scale for height.
     *
     * @see setScale(float), getScaleX, getScaleY
     */
    void setScale(float xScale, float yScale)
    {
        setScaleX(xScale);
        setScaleY(yScale);
    }

    /**
     * Sets SVG scale for width.
     *
     * @param  scale The scale for width.
     *
     * @see setScale(float), setScale(float,float), getScaleX
     */
    void setScaleX(float scale)
    {
        scaleX = scale;
    }

    /**
     * Gets SVG scale for width.
     *
     * @return The SVG scale for width.
     *
     * @see setScale, setScaleX
     */
    float getScaleX() const
    {
        return scaleX;
    }

    /**
     * Sets SVG height scale.
     *
     * @param  scale The scale for height.
     *
     * @see setScale(float), setScale(float,float), getScaleY
     */
    void setScaleY(float scale)
    {
        scaleY = scale;
    }

    /**
     * Gets SVG height scale.
     *
     * @return The SVG scale for height.
     *
     * @see setScale, setScaleY
     */
    float getScaleY() const
    {
        return scaleY;
    }

    /**
     * Sets SVG rotation center. The position is relative to the topleft corner of the SVGImage
     * widget, not relative to the SVG. The center is the position before applying any scaling.
     *
     * @param  centerX The center x coordinate.
     * @param  centerY The center y coordinate.
     *
     * @see getRotationCenterX, getRotationCenterY, setImagePosition
     */
    void setRotationCenter(float centerX, float centerY)
    {
        rotationCenterX = centerX;
        rotationCenterY = centerY;
    }

    /**
     * Gets rotation center x coordinate.
     *
     * @return The rotation center x coordinate.
     */
    float getRotationCenterX() const
    {
        return rotationCenterX;
    }

    /**
     * Gets rotation center y coordinate.
     *
     * @return The rotation center y coordinate.
     */
    float getRotationCenterY() const
    {
        return rotationCenterY;
    }

    /**
     * Sets SVG rotation. The rotation is done around the position set in the previously set
     * rotation center.
     *
     * @param  angle The rotation angle.
     *
     * @see setRotationCenter, getRotation
     */
    void setRotation(float angle)
    {
        rotation = angle;
    }

    /**
     * Gets SVG rotation.
     *
     * @return The SVG rotation.
     *
     * @see setRotation
     */
    float getRotation() const
    {
        return rotation;
    }

    /**
     * Sets image position. This is used to move the SVG on the screen. Negative values move the SVG
     * to the left and up.
     *
     * @param  imageX The image x coordinate.
     * @param  imageY The image y coordinate.
     *
     * @see getImagePositionX, getImagePositionY
     */
    void setImagePosition(float imageX, float imageY)
    {
        imagePositionX = imageX;
        imagePositionY = imageY;
    }

    /**
     * Gets image position x coordinate.
     *
     * @return The image position x coordinate.
     *
     * @see setImagePosition
     */
    float getImagePositionX() const
    {
        return imagePositionX;
    }

    /**
     * Gets image position y coordinate.
     *
     * @return The image position y coordinate.
     *
     * @see setImagePosition
     */
    float getImagePositionY() const
    {
        return imagePositionY;
    }

    virtual void draw(const Rect& invalidatedArea) const;

    virtual void invalidateContent() const;

    virtual Rect getSolidRect() const
    {
        return Rect();
    }

protected:
    /**
     * Calculate the transformation matrix used when drawing the SVG.
     *
     * @return The transformation matrix.
     */
    Matrix3x3 getTransformationMatrix() const;

    uint16_t svgId;        ///< The SVG to display.
    float scaleX;          ///< The scale for width
    float scaleY;          ///< The scale for height
    float rotation;        ///< The rotation
    float rotationCenterX; ///< The rotation center x coordinate
    float rotationCenterY; ///< The rotation center y coordinate
    float imagePositionX;  ///< The SVG image position x coordinate
    float imagePositionY;  ///< The SVG image position y coordinate
};

} // namespace touchgfx

#endif // TOUCHGFX_SVGIMAGE_HPP
