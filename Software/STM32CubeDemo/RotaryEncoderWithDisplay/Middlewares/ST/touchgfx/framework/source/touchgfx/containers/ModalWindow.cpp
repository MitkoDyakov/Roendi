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

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Drawable.hpp>
#include <touchgfx/containers/ModalWindow.hpp>
#include <touchgfx/hal/HAL.hpp>

namespace touchgfx
{
ModalWindow::ModalWindow()
    : Container(),
      backgroundShade(),
      windowContainer(),
      windowBackground()
{
    ModalWindow::setWidthHeight(HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);

    const int defaultShadeAlpha = 96;
    const colortype defaultShadeColor = Color::getColorFromRGB(0x0, 0x0, 0x0);

    backgroundShade.setPosition(0, 0, getWidth(), getHeight());
    backgroundShade.setColor(defaultShadeColor);
    backgroundShade.setTouchable(true);
    ModalWindow::setShadeAlpha(defaultShadeAlpha);
    Container::add(backgroundShade);

    Container::add(windowContainer);
    windowContainer.add(windowBackground);
}

void ModalWindow::setBackground(const BitmapId& bmpId)
{
    windowBackground.setBitmap(Bitmap(bmpId));
    windowBackground.setXY(0, 0);

    windowContainer.setPosition((getWidth() - windowBackground.getWidth()) / 2, (getHeight() - windowBackground.getHeight()) / 2, windowBackground.getWidth(), windowBackground.getHeight());
    invalidate();
}

void ModalWindow::setBackground(const BitmapId& bmpId, int16_t backgroundX, int16_t backgroundY)
{
    setBackground(bmpId);
    windowContainer.setXY(backgroundX, backgroundY);
}

uint16_t ModalWindow::getBackgroundWidth() const
{
    return windowBackground.getWidth();
}

uint16_t ModalWindow::getBackgroundHeight() const
{
    return windowBackground.getHeight();
}

void ModalWindow::add(Drawable& d)
{
    windowContainer.add(d);
}

void ModalWindow::remove(Drawable& d)
{
    windowContainer.remove(d);
}

void ModalWindow::setShadeAlpha(uint8_t alpha)
{
    backgroundShade.setAlpha(alpha);
    backgroundShade.invalidate();
}

uint8_t ModalWindow::getShadeAlpha() const
{
    return backgroundShade.getAlpha();
}

void ModalWindow::setShadeColor(colortype color)
{
    backgroundShade.setColor(color);
    backgroundShade.invalidate();
}

colortype ModalWindow::getShadeColor() const
{
    return backgroundShade.getColor();
}

void ModalWindow::show()
{
    setVisible(true);
    invalidate();
}

void ModalWindow::hide()
{
    invalidate();
    setVisible(false);
}
} // namespace touchgfx
