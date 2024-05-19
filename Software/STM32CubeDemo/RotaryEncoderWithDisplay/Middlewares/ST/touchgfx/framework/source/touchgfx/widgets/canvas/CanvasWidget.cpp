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

#include <touchgfx/Utils.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/widgets/canvas/CanvasWidget.hpp>

namespace touchgfx
{
CanvasWidget::CanvasWidget()
    : Widget(),
      alpha(255),
      canvasPainter(0)
{
}

void CanvasWidget::draw(const Rect& invalidatedArea) const
{
    Rect area = invalidatedArea & getMinimalRect();
    if (!canvasPainter || !canvasPainter->setup(getRect()) || area.isEmpty())
    {
        // Painter not ready to paint or nothing to paint => done
        return;
    }

    switch (HAL::DISPLAY_ROTATION)
    {
    case rotate0:
        {
            const int16_t bottom = area.bottom();
            while (area.y < bottom)
            {
                while (!drawCanvasWidget(area))
                {
                    if (area.height == 1)
                    {
                        // Failed on a single line
                        break;
                    }
                    area.height = (area.height + 1) >> 1; // Cannot become 0 as (2+1)>>1=1
#ifdef SIMULATOR
                    if (CanvasWidgetRenderer::getWriteMemoryUsageReport())
                    {
                        if (area.height > 1)
                        {
                            touchgfx_printf("CWR will split draw into multiple draws due to limited memory.\n");
                        }
                        else
                        {
                            touchgfx_printf("CWR was unable to complete a draw operation due to limited memory.\n");
                        }
                    }
#endif
                }
                area.y += area.height;
                if (area.bottom() > bottom)
                {
                    area.height = bottom - area.y;
                }
            }
        }
        break;
    case rotate90:
        {
            const int16_t right = area.right();
            while (area.x < right)
            {
                while (!drawCanvasWidget(area))
                {
                    if (area.width == 1)
                    {
                        break;
                    }
                    area.width = (area.width + 1) >> 1;
#ifdef SIMULATOR
                    if (CanvasWidgetRenderer::getWriteMemoryUsageReport())
                    {
                        if (area.width > 1)
                        {
                            touchgfx_printf("CWR will split draw into multiple draws due to limited memory.\n");
                        }
                        else
                        {
                            touchgfx_printf("CWR was unable to complete a draw operation due to limited memory.\n");
                        }
                    }
#endif
                }
                area.x += area.width;
                if (area.right() > right)
                {
                    area.width = right - area.x;
                }
            }
        }
        break;
    }
}

void CanvasWidget::invalidate() const
{
    Rect minimalRect = getMinimalRect();
    minimalRect &= Rect(0, 0, getWidth(), getHeight());
    invalidateRect(minimalRect);
}

} // namespace touchgfx
