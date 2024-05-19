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
 * @file touchgfx/hal/PaintImpl.hpp
 *
 * Declares paint functions for widgets
 */
#ifndef TOUCHGFX_PAINTIMPL_HPP
#define TOUCHGFX_PAINTIMPL_HPP

#include <touchgfx/hal/Paint.hpp>

namespace touchgfx
{
namespace paint
{
namespace
{
const uint8_t* blendL8CLUT = 0;
} //namespace

void setL8Palette(const uint8_t* const data)
{
    blendL8CLUT = data;
}

void tearDown()
{
}

} // namespace paint
} // namespace touchgfx

#endif // TOUCHGFX_PAINTIMPL_HPP
