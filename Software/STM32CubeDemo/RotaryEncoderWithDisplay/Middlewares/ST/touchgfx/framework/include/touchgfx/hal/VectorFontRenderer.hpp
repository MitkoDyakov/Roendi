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
 * @file touchgfx/hal/VectorFontRenderer.hpp
 *
 * Defines the touchgfx::VectorFontRenderer class.
 */
#ifndef TOUCHGFX_VECTORFONTRENDERER_HPP
#define TOUCHGFX_VECTORFONTRENDERER_HPP

#include <touchgfx/Font.hpp>
#include <touchgfx/hal/VectorRenderer.hpp>

namespace touchgfx
{
/**
 * Abstract interface for classes implementing vector font rendering.
 */
class VectorFontRenderer
{
public:
    /** Virtual destructor. */
    virtual ~VectorFontRenderer()
    {
    }

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
    virtual void drawGlyph(const Rect& canvasAreaAbs, const Rect& invalidatedAreaRel, const uint16_t* data, const Font* font, colortype color, uint8_t alpha, TextRotation rotation, int x, int y) = 0;
};

} // namespace touchgfx

#endif // TOUCHGFX_VECTORFONTRENDERER_HPP
