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
 * @file touchgfx/transitions/CoverTransition.hpp
 *
 * Declares the touchgfx::CoverTransition class.
 */
#ifndef TOUCHGFX_COVERTRANSITION_HPP
#define TOUCHGFX_COVERTRANSITION_HPP

#include <touchgfx/Callback.hpp>
#include <touchgfx/Drawable.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/transitions/Transition.hpp>

namespace touchgfx
{
/**
 * A Transition that slides the new screen over the previous \e from the given direction.
 */
template <Direction templateDirection>
class CoverTransition : public Transition
{
public:
    /**
     * Initializes a new instance of the CoverTransition class.
     *
     * @param  transitionSteps (Optional) Number of steps in the transition animation.
     */
    CoverTransition(const uint8_t transitionSteps = 20)
        : Transition(),
          animationSteps(transitionSteps),
          animationCounter(0),
          calculatedValue(0),
          solid()
    {
        switch (templateDirection)
        {
        case EAST:
            targetValue = -HAL::DISPLAY_WIDTH;
            break;
        case WEST:
            targetValue = HAL::DISPLAY_WIDTH;
            break;
        case NORTH:
            targetValue = HAL::DISPLAY_HEIGHT;
            break;
        case SOUTH:
            targetValue = -HAL::DISPLAY_HEIGHT;
            break;
        default:
            done = true;
            // Nothing to do here
            break;
        }

        // Ensure that the solid area covers the entire screen
        solid.setPosition(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
    }

    /**
     * Handles the tick event when transitioning. It moves the contents of the Screen's
     * container. The direction of the transition determines the direction the contents of
     * the container moves.
     */
    virtual void handleTickEvent()
    {
        Transition::handleTickEvent();

        // Calculate new position or stop animation
        animationCounter++;
        if (animationCounter > animationSteps)
        {
            // Final step: stop the animation
            done = true;
            animationCounter = 0;
            return;
        }

        const int16_t oldValue = calculatedValue;

        // Calculate value in [0;targetValue]
        calculatedValue = EasingEquations::cubicEaseOut(animationCounter, 0, targetValue, animationSteps);

        // Note: Result of "calculatedValue & 1" is compiler dependent for negative values of calculatedValue
        if ((calculatedValue % 2) != 0)
        {
            // Optimization: calculatedValue is odd, add 1/-1 to move drawables modulo 32 bits in framebuffer
            calculatedValue += (calculatedValue > 0 ? 1 : -1);
        }

        // Convert the calculated value to delta value relative to current moved-to position
        const int16_t relativeValue = calculatedValue - oldValue;

        // The Cover Transition only draws to parts of the non-TFT framebuffer. To avoid glitches
        // In Double buffering mode both framebuffers must be identical.
        if (animationCounter == 1 && HAL::USE_DOUBLE_BUFFERING)
        {
            // Synchronize framebuffers
            Application::getInstance()->copyInvalidatedAreasFromTFTToClientBuffer();
        }

        Rect rect;
        switch (templateDirection)
        {
        case EAST:
            rect.x = HAL::DISPLAY_WIDTH + calculatedValue;
            rect.y = 0;
            rect.width = -calculatedValue;
            rect.height = HAL::DISPLAY_HEIGHT;
            break;
        case WEST:
            rect.x = 0;
            rect.y = 0;
            rect.width = calculatedValue;
            rect.height = HAL::DISPLAY_HEIGHT;
            break;
        case NORTH:
            rect.x = 0;
            rect.y = 0;
            rect.width = HAL::DISPLAY_WIDTH;
            rect.height = calculatedValue;
            break;
        case SOUTH:
            rect.x = 0;
            rect.y = HAL::DISPLAY_HEIGHT + calculatedValue;
            rect.width = HAL::DISPLAY_WIDTH;
            rect.height = -calculatedValue;
            break;
        }
        Application::getInstance()->invalidateArea(rect);

        // Move children with delta value for X or Y
        Drawable* d = screenContainer->getFirstChild();
        while (d)
        {
            switch (templateDirection)
            {
            case EAST:
            case WEST:
                d->setX(d->getX() + relativeValue);
                break;
            case NORTH:
            case SOUTH:
                d->setY(d->getY() + relativeValue);
                break;
            }

            d = d->getNextSibling();
        }
    }

    virtual void tearDown()
    {
        screenContainer->remove(solid);
    }

    virtual void init()
    {
        Transition::init();

        // Move snapshot and its children with delta value for X or Y
        Drawable* d = screenContainer->getFirstChild();
        while (d)
        {
            switch (templateDirection)
            {
            case EAST:
            case WEST:
                d->setX(d->getX() - targetValue);
                break;
            case NORTH:
            case SOUTH:
                d->setY(d->getY() - targetValue);
                break;
            }

            d = d->getNextSibling();
        }

        screenContainer->add(solid);
    }

private:
    const uint8_t animationSteps; ///< Number of steps the transition should move per complete animation.
    uint8_t animationCounter;     ///< Current step in the transition animation.
    int16_t targetValue;          ///< The target value for the transition animation.
    int16_t calculatedValue;      ///< The current X or Y value
    FullSolidRect solid;          ///< A solid rect that covers the entire screen to avoid copying elements outside
};

} // namespace touchgfx

#endif // TOUCHGFX_COVERTRANSITION_HPP
