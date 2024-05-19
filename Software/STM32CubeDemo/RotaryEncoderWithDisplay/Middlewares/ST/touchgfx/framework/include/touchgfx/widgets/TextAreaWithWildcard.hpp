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
 * @file touchgfx/widgets/TextAreaWithWildcard.hpp
 *
 * Declares the touchgfx::TextAreaWithOneWildcard and touchgfx::TextAreaWithTwoWildcards classes.
 */
#ifndef TOUCHGFX_TEXTAREAWITHWILDCARD_HPP
#define TOUCHGFX_TEXTAREAWITHWILDCARD_HPP

#include <touchgfx/Unicode.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/TextArea.hpp>

namespace touchgfx
{
/**
 * Base class for TextArea with one or two wildcards.
 *
 * @see TextAreaWithOneWildcard, TextAreaWithTwoWildcards
 *
 */
class TextAreaWithWildcardBase : public TextArea
{
public:
    TextAreaWithWildcardBase()
        : TextArea()
    {
    }

    virtual void draw(const Rect& area) const;

    virtual void invalidateContent() const
    {
        Widget::invalidateContent();
    }

protected:
    virtual TextArea::BoundingArea calculateBoundingArea() const
    {
        return TextArea::BoundingArea();
    }
};

/**
 * TextArea with one wildcard. The format string (i.e. the TypedText set in setTypedText()) is
 * expected to contain a wildcard &lt;placeholder> from the text.
 *
 * @note the text converter tool converts the <...> to ascii value 2 which is then being
 *       replaced by a wildcard text.
 */
class TextAreaWithOneWildcard : public TextAreaWithWildcardBase
{
public:
    TextAreaWithOneWildcard()
        : TextAreaWithWildcardBase(), wc1(0)
    {
    }

    /**
     * Sets the wildcard used in the TypedText where &lt;placeholder> is placed. Wildcard
     * string must be a null-terminated UnicodeChar array.
     *
     * @param  value A pointer to the UnicodeChar to set the wildcard to.
     *
     * @note The pointer passed is saved, and must be accessible whenever TextAreaWithOneWildcard
     *       may need it.
     */
    void setWildcard1(const Unicode::UnicodeChar* value)
    {
        wc1 = value;
    }

    virtual const Unicode::UnicodeChar* getWildcard1() const
    {
        return wc1;
    }

    /**
     * Sets the wildcard used in the TypedText where &lt;placeholder> is placed. Wildcard
     * string must be a null-terminated UnicodeChar array.
     *
     * @param  value A pointer to the UnicodeChar to set the wildcard to.
     *
     * @note The pointer passed is saved, and must be accessible whenever TextAreaWithOneWildcard
     *       may need it.
     */
    void setWildcard(const Unicode::UnicodeChar* value)
    {
        setWildcard1(value);
    }

    /**
     * Gets the wildcard used in the TypedText as previously set using setWildcard().
     *
     * @return The wildcard used in the text.
     */
    const Unicode::UnicodeChar* getWildcard() const
    {
        return getWildcard1();
    }

protected:
    const Unicode::UnicodeChar* wc1; ///< Pointer to the wildcard string. Must be null-terminated.
};

/**
 * TextArea with two wildcards. The format string (i.e. the TypedText set in setTypedText()) is
 * expected to contain two wildcards &lt;placeholders> from the text.
 *
 *
 * @note the text converter tool converts the <...> to ascii value 2 which is what is
 *       being replaced by a wildcard text.
 */
class TextAreaWithTwoWildcards : public TextAreaWithWildcardBase
{
public:
    TextAreaWithTwoWildcards()
        : TextAreaWithWildcardBase(), wc1(0), wc2(0)
    {
    }

    /**
     * Sets the wildcard used in the TypedText where first &lt;placeholder> is placed.
     * Wildcard string must be a null-terminated UnicodeChar array.
     *
     * @param  value A pointer to the UnicodeChar to set the wildcard to.
     *
     * @note The pointer passed is saved, and must be accessible whenever TextAreaWithTwoWildcard
     *       may need it.
     */
    void setWildcard1(const Unicode::UnicodeChar* value)
    {
        wc1 = value;
    }

    virtual const Unicode::UnicodeChar* getWildcard1() const
    {
        return wc1;
    }

    /**
     * Sets the wildcard used in the TypedText where second &lt;placeholder> is placed.
     * Wildcard string must be a null-terminated UnicodeChar array.
     *
     * @param  value A pointer to the UnicodeChar to set the wildcard to.
     *
     * @note The pointer passed is saved, and must be accessible whenever TextAreaWithTwoWildcard
     *       may need it.
     */
    void setWildcard2(const Unicode::UnicodeChar* value)
    {
        wc2 = value;
    }

    virtual const Unicode::UnicodeChar* getWildcard2() const
    {
        return wc2;
    }

protected:
    const Unicode::UnicodeChar* wc1; ///< Pointer to the first wildcard string. Must be null-terminated.
    const Unicode::UnicodeChar* wc2; ///< Pointer to the second wildcard string. Must be null-terminated.
};

} // namespace touchgfx

#endif // TOUCHGFX_TEXTAREAWITHWILDCARD_HPP
