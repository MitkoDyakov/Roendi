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
 * @file touchgfx/transitions/WipeTransition.hpp
 *
 * Declares the touchgfx::WipeTransition class.
 */
#ifndef TOUCHGFX_WIPETRANSITION_HPP
#define TOUCHGFX_WIPETRANSITION_HPP

#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transitions/Transition.hpp>

namespace touchgfx
{
/**
 * A Transition that expands the new screen over the previous from
 * the given direction.  This transition only draws the pixels in the
 * framebuffer once, and never moves any pixels. It is therefore very
 * useful on MCUs with limited performance.
 */
template <Direction templateDirection>
class WipeTransition : public Transition
{
public:
    /**
     * Initializes a new instance of the WipeTransition class.
     *
     * @param  transitionSteps (Optional) Number of steps in the transition animation.
     */
    WipeTransition(const uint8_t transitionSteps = 20)
        : Transition(),
          animationSteps(transitionSteps),
          animationCounter(0),
          calculatedValue(0),
          solid()
    {
        switch (templateDirection)
        {
        case EAST:
        case WEST:
            targetValue = HAL::DISPLAY_WIDTH;
            break;
        case NORTH:
        case SOUTH:
            targetValue = HAL::DISPLAY_HEIGHT;
            break;
        }

        // Ensure that the solid area covers the entire screen
        solid.setPosition(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
    }

    /**
     * Handles the tick event when transitioning. It uncovers and
     * invalidates increasing parts of the new screen elements.
     */
    virtual void handleTickEvent()
    {
        Transition::handleTickEvent();
        animationCounter++;

        // Calculate new position or stop animation
        if (animationCounter > animationSteps)
        {
            // Final step: stop the animation
            done = true;
            animationCounter = 0;
            return;
        }

        // Calculate value in [0;targetValue]
        calculatedValue = EasingEquations::cubicEaseOut(animationCounter, 0, targetValue, animationSteps);

        // Note: Result of "calculatedValue & 1" is compiler dependent for negative values of calculatedValue
        if ((calculatedValue % 2) != 0)
        {
            // Optimization: calculatedValue is odd, add 1/-1 to move drawables modulo 32 bits in framebuffer
            calculatedValue += (calculatedValue > 0 ? 1 : -1);
        }

        // calculatedValue is the width/height of the visible area

        switch (templateDirection)
        {
        case EAST:
            {
                // Cover must have width of remaining part
                const uint16_t prevSolidWidth = solid.getWidth();
                solid.setWidth(HAL::DISPLAY_WIDTH - calculatedValue);

                // Invalidate the uncovered part
                const uint16_t delta = prevSolidWidth - solid.getWidth();
                Rect r(solid.getWidth(), 0, delta, HAL::DISPLAY_HEIGHT);
                screenContainer->invalidateRect(r);
                break;
            }
        case WEST:
            {
                // Cover must have width of remaining part and start after uncovered
                const uint16_t prevSolidPos = solid.getX();
                solid.setWidth(HAL::DISPLAY_WIDTH - calculatedValue);
                solid.setX(calculatedValue);

                // Invalidate the uncovered part
                const uint16_t delta = calculatedValue - prevSolidPos;
                Rect r(prevSolidPos, 0, delta, HAL::DISPLAY_HEIGHT);
                screenContainer->invalidateRect(r);
                break;
            }
        case NORTH:
            {
                // Cover must have height of remaining part and start after uncovered
                const uint16_t prevSolidPos = solid.getY();
                solid.setHeight(HAL::DISPLAY_HEIGHT - calculatedValue);
                solid.setY(calculatedValue);

                // Invalidate the uncovered part
                const uint16_t delta = calculatedValue - prevSolidPos;
                Rect r(0, prevSolidPos, HAL::DISPLAY_WIDTH, delta);
                screenContainer->invalidateRect(r);
                break;
            }
        case SOUTH:
            {
                // Cover must have height of remaining part
                const uint16_t prevSolidHeight = solid.getHeight();
                solid.setHeight(HAL::DISPLAY_HEIGHT - calculatedValue);

                // Invalidate the uncovered part
                const uint16_t delta = prevSolidHeight - solid.getHeight();
                Rect r(0, solid.getHeight(), HAL::DISPLAY_WIDTH, delta);
                screenContainer->invalidateRect(r);
                break;
            }
        }

        // The WipeTransition only draws to parts of the non-TFT
        // framebuffer. To avoid glitches in Double buffering mode
        // both framebuffers must be made identical.
        if (animationCounter == 1 && HAL::USE_DOUBLE_BUFFERING)
        {
            // Synchronize framebuffers
            Application::getInstance()->copyInvalidatedAreasFromTFTToClientBuffer();
        }
    }

    virtual void tearDown()
    {
        screenContainer->remove(solid);
    }

    virtual void init()
    {
        Transition::init();
        // Add the solid (and not-drawing-anything) widget on top to cover the other widgets
        screenContainer->add(solid);
    }

    /**
     * Wipe transition does not require an invalidation. Invalidation
     * is handled by the class. Do no invalidation initially.
     */
    virtual void invalidate()
    {
    }

private:
    const uint8_t animationSteps; ///< Number of steps the transition should move per complete animation.
    uint8_t animationCounter;     ///< Current step in the transition animation.
    int16_t targetValue;          ///< The target value for the transition animation.
    int16_t calculatedValue;      ///< The calculated X or Y value to move the snapshot and the children.
    FullSolidRect solid;          ///< A solid rect that covers the entire screen to avoid copying elements outside
};

} // namespace touchgfx

#endif // TOUCHGFX_WIPETRANSITION_HPP
