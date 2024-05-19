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
 * @file touchgfx/transitions/SlideTransition.hpp
 *
 * Declares the touchgfx::SlideTransition class.
 */
#ifndef TOUCHGFX_SLIDETRANSITION_HPP
#define TOUCHGFX_SLIDETRANSITION_HPP

#include <touchgfx/Drawable.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/transitions/Transition.hpp>
#include <touchgfx/widgets/SnapshotWidget.hpp>

namespace touchgfx
{
/**
 * A Transition that slides from one screen to the next. It does so by moving a SnapShotWidget
 * with a snapshot of the Screen transitioning away from, and by moving the contents of
 * Screen transitioning to.
 *
 * @see Transition
 */
template <Direction templateDirection>
class SlideTransition : public Transition
{
public:
    /**
     * Initializes a new instance of the SlideTransition class.
     *
     * @param  transitionSteps (Optional) Number of steps (ticks) in the transition animation, default is 20.
     */
    SlideTransition(const uint8_t transitionSteps = 20)
        : Transition(),
          snapshot(),
          animationSteps(transitionSteps),
          animationCounter(0),
          relativeValue(0)
    {
        if (!HAL::USE_ANIMATION_STORAGE)
        {
            // No animation storage, don't slide
            done = true;
            return;
        }

        snapshot.setPosition(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
        snapshot.makeSnapshot();

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
        }
    }

    /**
     * Handles the tick event when transitioning. It moves the contents of the Screen's
     * container and a SnapshotWidget with a snapshot of the previous Screen. The direction
     * of the transition determines the direction the contents of the container and the
     * SnapshotWidget moves.
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

        // Calculate value in [0;targetValue]
        int16_t calculatedValue = EasingEquations::cubicEaseOut(animationCounter, 0, targetValue, animationSteps);

        // Note: Result of "calculatedValue & 1" is compiler dependent for negative values of calculatedValue
        if ((calculatedValue % 2) != 0)
        {
            // Optimization: calculatedValue is odd, add 1/-1 to move drawables modulo 32 bits in framebuffer
            calculatedValue += (calculatedValue > 0 ? 1 : -1);
        }

        // Move snapshot
        switch (templateDirection)
        {
        case EAST:
        case WEST:
            relativeValue = calculatedValue - snapshot.getX();
            break;
        case NORTH:
        case SOUTH:
            relativeValue = calculatedValue - snapshot.getY();
            break;
        }

        // Move snapshot and its children with delta value for X or Y
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

        // Entire screen has changed, redraw
        screenContainer->invalidate();
    }

    virtual void tearDown()
    {
        if (HAL::USE_ANIMATION_STORAGE && screenContainer)
        {
            screenContainer->remove(snapshot);
        }
    }

    virtual void init()
    {
        Transition::init();

        if (done)
        {
            return;
        }

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

        screenContainer->add(snapshot);
    }

protected:
    SnapshotWidget snapshot; ///< The SnapshotWidget that is moved when transitioning.

private:
    const uint8_t animationSteps; ///< Number of steps the transition should move per complete animation.
    uint8_t animationCounter;     ///< Current step in the transition animation.
    int16_t targetValue;          ///< The target value for the transition animation.
    int16_t relativeValue;        ///< The relative X or Y value for the snapshot and the children.
};

} // namespace touchgfx

#endif // TOUCHGFX_SLIDETRANSITION_HPP
