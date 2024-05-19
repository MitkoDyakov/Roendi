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
 * @file touchgfx/ConstFont.hpp
 *
 * Declares the touchgfx::ConstFont class.
 */
#ifndef TOUCHGFX_CONSTFONT_HPP
#define TOUCHGFX_CONSTFONT_HPP

#include <touchgfx/Font.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * A ConstFont is a Font implementation that has its contents defined at compile-time and
 * usually placed in read-only memory.
 *
 * @see Font
 *
 * @note Pure virtual class. Create an application-specific implementation of getPixelData().
 */
class ConstFont : public Font
{
public:
    /**
     * Initializes a new instance of the ConstFont class.
     *
     * @param  glyphs         The array of glyphs known to this font.
     * @param  numGlyphs      The number of glyphs in list.
     * @param  height         The height of the font.
     * @param  baseline       The pixel position of the baseline.
     * @param  pixAboveTop    The maximum number of pixels above the top of the text.
     * @param  pixBelowBottom The maximum number of pixels that can be drawn below the baseline in
     *                        this font.
     * @param  bitsPerPixel   The number of bits per pixel in this font.
     * @param  byteAlignRow   The glyphs are saved with each row byte aligned.
     * @param  maxLeft        The maximum a character extends to the left.
     * @param  maxRight       The maximum a character extends to the right.
     * @param  fallbackChar   The fallback character for the typography in case no glyph is available.
     * @param  ellipsisChar The ellipsis character used for truncating long texts.
     */
    ConstFont(const GlyphNode* glyphs, uint16_t numGlyphs, uint16_t height, uint16_t baseline, uint8_t pixAboveTop, uint8_t pixBelowBottom, uint8_t bitsPerPixel, uint8_t byteAlignRow, uint8_t maxLeft, uint8_t maxRight, const Unicode::UnicodeChar fallbackChar, const Unicode::UnicodeChar ellipsisChar);

    using Font::getGlyph;

    virtual const GlyphNode* getGlyph(Unicode::UnicodeChar unicode, const uint8_t*& pixelData, uint8_t& bitsPerPixel) const;

    /**
     * Gets the pixel date associated with this glyph.
     *
     * @param  glyph The glyph to get the pixels data from.
     *
     * @return Pointer to the pixel data of this glyph.
     */
    virtual const uint8_t* getPixelData(const GlyphNode* glyph) const = 0;

    virtual int8_t getKerning(Unicode::UnicodeChar prevChar, const GlyphNode* glyph) const = 0;

    /**
     * Finds the glyph data associated with the specified unicode.
     *
     * @param  unicode The character to look up.
     *
     * @return A pointer to the glyph node or null if the glyph was not found.
     */
    const GlyphNode* find(Unicode::UnicodeChar unicode) const;

protected:
    const GlyphNode* glyphList; ///< The list of glyphs
    uint16_t listSize;          ///< The size of the list of glyphs

private:
    ConstFont()
        : Font(0, 0, 0, 0, 0, 0, 0, 0, 0, 0), glyphList(0), listSize(0)
    {
    }
};

} // namespace touchgfx

#endif // TOUCHGFX_CONSTFONT_HPP
