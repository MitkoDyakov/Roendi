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

#include <touchgfx/containers/progress_indicators/AbstractDirectionProgress.hpp>

namespace touchgfx
{
AbstractDirectionProgress::AbstractDirectionProgress()
    : AbstractProgressIndicator(), progressDirection(RIGHT)
{
    AbstractDirectionProgress::setDirection(RIGHT);
}

void AbstractDirectionProgress::setDirection(DirectionType direction)
{
    if (direction != progressDirection)
    {
        progressDirection = direction;
        progressIndicatorContainer.invalidate();
        setValue(getValue());
    }
}

AbstractDirectionProgress::DirectionType AbstractDirectionProgress::getDirection() const
{
    return progressDirection;
}
} // namespace touchgfx
