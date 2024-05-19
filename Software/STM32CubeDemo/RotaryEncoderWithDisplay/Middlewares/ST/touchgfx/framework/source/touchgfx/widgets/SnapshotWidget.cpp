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

#include <touchgfx/Drawable.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/SnapshotWidget.hpp>

namespace touchgfx
{
SnapshotWidget::SnapshotWidget()
    : Widget(), bitmapId(BITMAP_INVALID), alpha(255)
{
}

void SnapshotWidget::draw(const Rect& invalidatedArea) const
{
    if (alpha == 0 || bitmapId == BITMAP_INVALID)
    {
        return;
    }

    Rect absRect(0, 0, Bitmap(bitmapId).getWidth(), rect.height);
    translateRectToAbsolute(absRect);
    HAL::lcd().blitCopy((const uint16_t*)Bitmap(bitmapId).getData(), absRect, invalidatedArea, alpha, false);
}

Rect SnapshotWidget::getSolidRect() const
{
    if (alpha < 255 || bitmapId == BITMAP_INVALID)
    {
        return Rect();
    }
    return Rect(0, 0, getWidth(), getHeight());
}

void SnapshotWidget::makeSnapshot()
{
    makeSnapshot(BITMAP_ANIMATION_STORAGE);
}

void SnapshotWidget::makeSnapshot(const BitmapId bmp)
{
    Rect visRect(0, 0, rect.width, rect.height);
    getVisibleRect(visRect);
    const Rect absRect = getAbsoluteRect();
    bitmapId = (HAL::lcd().copyFrameBufferRegionToMemory(visRect, absRect, bmp)) ? bmp : BITMAP_INVALID;
}
} // namespace touchgfx
