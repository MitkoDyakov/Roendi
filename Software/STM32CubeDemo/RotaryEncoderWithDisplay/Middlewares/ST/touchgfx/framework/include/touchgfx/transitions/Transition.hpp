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
 * @file touchgfx/transitions/Transition.hpp
 *
 * Declares the touchgfx::Transition class.
 */
#ifndef TOUCHGFX_TRANSITION_HPP
#define TOUCHGFX_TRANSITION_HPP

#include <touchgfx/Application.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
/**
 * The Transition class is the base class for Transitions. Implementations of Transition defines
 * what happens when transitioning between Screens, which typically involves visual
 * effects. An example of a transition implementation can be seen in example
 * custom_transition_example. The most basic transition is the NoTransition class that
 * does a transition without any visual effects.
 *
 * @see NoTransition, SlideTransition
 */
class Transition
{
public:
    /** Initializes a new instance of the Transition class. */
    Transition()
        : screenContainer(0), done(false)
    {
    }

    /** Finalizes an instance of the Transition class. */
    virtual ~Transition()
    {
    }

    /** Called for every tick when transitioning. */
    virtual void handleTickEvent()
    {
    }

    /**
     * Query if the transition is done transitioning. It is the responsibility of the
     * inheriting class to set the underlying done flag once the transition has been
     * completed.
     *
     * @return True if the transition is done, false otherwise.
     */
    bool isDone() const
    {
        return done;
    }

    /**
     * Tears down the Animation. Called before the destructor is called, when the
     * application changes the transition.
     */
    virtual void tearDown()
    {
    }

    /**
     * Initializes the transition. Called after the constructor is called, when the
     * application changes the transition.
     */
    virtual void init()
    {
    }

    /**
     * Invalidates the screen when starting the Transition. Default is
     * to invalidate the whole screen. Subclasses can do partial
     * invalidation.
     */
    virtual void invalidate()
    {
        Application::getInstance()->invalidate();
    }

    /**
     * Sets the Screen Container. Is used by Screen to enable the transition to access the
     * Container.
     *
     * @param [in] cont The Container the transition should have access to.
     */
    virtual void setScreenContainer(Container& cont)
    {
        screenContainer = &cont;
    }

protected:
    /**
     * A Widget that reports solid and but does not draw anything.
     */
    class FullSolidRect : public Widget
    {
    public:
        virtual Rect getSolidRect() const
        {
            return Rect(0, 0, rect.width, rect.height);
        }

        virtual void draw(const Rect& area) const
        {
        }
    };

    Container* screenContainer; ///< The screen Container of the Screen transitioning to.
    bool done;                  ///< Flag that indicates when the transition is done. This should be set by implementing classes.
};

} // namespace touchgfx

#endif // TOUCHGFX_TRANSITION_HPP
