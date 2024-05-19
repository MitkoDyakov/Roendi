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
 * @file touchgfx/VectorFontRendererImpl.hpp
 *
 * Defines the touchgfx::VectorFontRendererImpl class, implementing the rendering of vector font glyphs.
 */
#ifndef TOUCHGFX_VECTORFONTRENDERERIMPL_HPP
#define TOUCHGFX_VECTORFONTRENDERERIMPL_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/hal/VectorFontRenderer.hpp>

namespace touchgfx
{
/**
 * Implementation of the VectorFontRenderer interface
 */
class VectorFontRendererImpl : public VectorFontRenderer
{
public:
    VectorFontRendererImpl();

    /**
     * Get buffers for commands and points for the vector font.
     *
     * @param pointArray         Array for the points.
     * @param pointArraySize     Size of the pointArray.
     * @param commandArray       Array for the commands.
     * @param commandArraySize   Size of the commandArray.
     */
    static void getVectorFontBuffers(float*& pointArray, int& pointArraySize, uint8_t*& commandArray, int& commandArraySize);

    /**
     * Draws one glyph from the provided data.
     *
     * @param  canvasAreaAbs         The canvas dimensions in absolute coordinates.
     * @param  invalidatedAreaRel    The area which should be updated in relative coordinates to the canvas area.
     * @param  data                  Data containing tags and points for all contours of the glyph.
     * @param  font                  The font to use.
     * @param  color                 The color of the glyph.
     * @param  alpha                 The transparency of the glyph.
     * @param  rotation              Rotation to do before drawing the glyph.
     * @param  x                     Distance to advance horizontally.
     * @param  y                     Distance to move down.
     */
    virtual void drawGlyph(const Rect& canvasAreaAbs, const Rect& invalidatedAreaRel, const uint16_t* data, const Font* font, colortype color, uint8_t alpha, TextRotation rotation, int x, int y);

private:
    VectorRenderer* renderer;

    uint8_t* cmds; // Buffer of commands for VectorRenderer::drawPath
    float* points; // Buffer of points for VectorRenderer::drawPath
    int commandBufferSize;
    int pointBufferSize;

    uint16_t cmdIndex;
    uint16_t pointsIndex;

    /**
     * Draws a single contour.
     *
     * @param tagCount   The number of tags for the contour to draw.
     * @param tagList    List of tags; one tag-bit for each cmd (segment) of the contour.
     * @param pointList  The points defining the contour, including intermediate control points.
     */
    const int16_t* drawContour(uint16_t tagCount, const uint16_t* const tagList, const int16_t* const pointList);
};

} // namespace touchgfx

#endif // TOUCHGFX_VECTORFONTRENDERERIMPL_HPP
