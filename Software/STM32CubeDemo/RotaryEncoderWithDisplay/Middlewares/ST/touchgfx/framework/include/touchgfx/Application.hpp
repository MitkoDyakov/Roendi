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
 * @file touchgfx/Application.hpp
 *
 * Declares the application class.
 */
#ifndef TOUCHGFX_APPLICATION_HPP
#define TOUCHGFX_APPLICATION_HPP

#include <touchgfx/Drawable.hpp>
#include <touchgfx/UIEventListener.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/hal/Config.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>

namespace touchgfx
{
class Drawable;
class Screen;
class Transition;

/**
 * The Application class is the main interface for manipulating screen contents. It holds a
 * pointer to the currently displayed Screen, and delegates draw requests and events to
 * that Screen. Additionally it contains some global application settings.
 */
class Application : public UIEventListener
{
public:
    /**
     * Gets the single instance application.
     *
     * @return The instance of this application.
     */
    static Application* getInstance();

    /**
     * Gets the current screen.
     *
     * @return The current screen.
     */
    static Screen* getCurrentScreen()
    {
        return currentScreen;
    }

    /**
     * This function can be called to send your application back to
     * the start screen. The simulator will call this function when F5
     * is pressed. To make this work, please implement this function
     * in FrontendApplication.
     *
     * @note The application will not make a complete restart - if
     *       your Model contains data, this will not be reset, unless
     *       this is explicitly done in your
     *       FrontendApplication::changeToStartScreen().
     */
    virtual void changeToStartScreen()
    {
    }

    /**
     * Switch to another Screen. Will call tearDownScreen on current Screen before switching,
     * and subsequently call setupScreen and draw automatically for the new Screen.
     *
     * @param [in] newScreen A pointer to the new screen.
     */
    virtual void switchScreen(Screen* newScreen);

    /**
     * An application specific function for switching screen. Overloading this can provide a
     * means to switch screen from places that does not have access to a pointer to the new
     * screen. Base implementation is empty.
     *
     * @param screenId An id that maps to the desired screen.
     */
    virtual void appSwitchScreen(uint8_t screenId)
    {
        (void)screenId; // Unused variable
    }

    /** An application specific function for requesting redraw of entire screen. */
    virtual void requestRedraw();

    /**
     * An application specific function for requesting redraw of given Rect.
     *
     * @param [in,out] rect The Rect that must be redrawn.
     */
    virtual void requestRedraw(Rect& rect)
    {
        redraw = rect;
    }

    /**
     * Handle a click event. Standard implementation is to delegate the event to the current
     * screen. Called by the framework when a click is detected by some platform specific
     * means.
     *
     * @param event The ClickEvent.
     */
    virtual void handleClickEvent(const ClickEvent& event);

    /**
     * Handle drag events. Called by the framework when a drag is detected by some platform
     * specific means. Standard implementation is to delegate drag event to current screen.
     *
     * @param event The drag event, expressed in absolute coordinates.
     */
    virtual void handleDragEvent(const DragEvent& event);

    /**
     * Handle gestures. Called by the framework when a gesture is detected by some platform
     * specific means. Standard implementation is to delegate drag event to current screen.
     *
     * @param event The gesture event.
     */
    virtual void handleGestureEvent(const GestureEvent& event);

    /**
     * Handle tick. Standard implementation is to delegate tick to the widgets that have
     * registered to receive one. Called by some platform specific means.
     */
    virtual void handleTickEvent();

    /**
     * Handle an incoming character received by the HAL layer. Standard implementation
     * delegates to current screen (which, in turn, does nothing).
     *
     * @param  c The incoming character to handle.
     */
    virtual void handleKeyEvent(uint8_t c);

    /**
     * Evaluates the pending Callback instances. If a callback is valid, it is executed and
     * a Screen transition is executed. This base implementation is empty and does nothing.
     */
    virtual void handlePendingScreenTransition();

    /**
     * Clears the cached areas so coming calls to invalidate are collected for future drawing.
     *
     * @see drawCachedAreas
     */
    virtual void clearCachedAreas();

    /**
     * Draws all cached, invalidated areas on the screen.
     *
     * @see clearCachedAreas
     */
    virtual void drawCachedAreas();

    /**
     * This function copies the parts that were updated in the
     * previous frame (in the TFT buffer) to the active framebuffer
     * (client buffer).
     *
     * This function only copies pixels in double buffering mode.
     */
    void copyInvalidatedAreasFromTFTToClientBuffer();

    /**
     * Adds a widget to the list of widgets receiving ticks every frame (typically
     * 16.67ms)
     *
     * @param [in] w The widget to add.
     *
     * @see unregisterTimerWidget
     *
     * @note The framework keeps track of the number of times a specific widget is registered.
     */
    void registerTimerWidget(Drawable* w)
    {
        timerWidgets.add(w);
    }

    /** Clears all currently registered timer widgets. */

    void clearAllTimerWidgets()
    {
        timerWidgets.clear();
    }

    /**
     * Removes a widget from the list of widgets receiving ticks every frame (typically
     * 16.67ms) milliseconds.
     *
     * @param [in] w The widget to remove.
     *
     * @note If widget has been registered multiple times, an equal number of calls to unregister
     *       are required to stop widget from receiving tick events.
     */
    void unregisterTimerWidget(const Drawable* w)
    {
        timerWidgets.remove(w);
    }

    /**
     * gets the number of timer widgets that are registered and active.
     *
     * @return The size of timerWidgets.
     */
    uint16_t getNumberOfRegisteredTimerWidgets() const
    {
        return timerWidgets.getNumberOfActiveTimerWidgets();
    }

    /**
     * Gets the number of timer events registered to a widget, i.e. how many times a
     * drawable must be unregistered until it no longer receives timer ticks.
     *
     * @param  w The widget to to get count from.
     *
     * @return 0 if the drawable is not registered as a timer widget, otherwise returns how
     *         many times the drawable is currently registered.
     */
    uint16_t getTimerWidgetCountForDrawable(const Drawable* w) const
    {
        return timerWidgets.getCounter(w);
    }

    static const uint8_t MAX_TIMER_WIDGETS = 32; ///< Maximum number of widgets receiving ticks. @remarks Memory impact: x * (sizeof(Drawable*)+1)
    static const uint16_t TICK_INTERVAL_MS = 10; ///< Deprecated, do not use this constant. Tick interval depends on VSYNC of your target platform.

    /**
     * Sets the DebugPrinter object to be used by the application to print debug messages.
     *
     * @param [in] printer The debug printer to configure.
     */
    static void setDebugPrinter(DebugPrinter* printer)
    {
        debugPrinter = printer;
    }

    /**
     * Returns the DebugPrinter object associated with the application.
     *
     * @return DebugPrinter The DebugPrinter object.
     */
    static DebugPrinter* getDebugPrinter()
    {
        return debugPrinter;
    }

    /**
     * Sets the debug string to be displayed onscreen on top of the framebuffer.
     */
    static void invalidateDebugRegion()
    {
        if (debugPrinter)
        {
            debugRegionInvalidRect.expandToFit(debugPrinter->getRegion());
        }
    }

    /**
     * Sets the debug string to be displayed onscreen on top of the framebuffer.
     *
     * @param [in] string The debug string to display onscreen.
     */
    static void setDebugString(const char* string)
    {
        if (debugPrinter)
        {
            debugPrinter->setString(string);
            invalidateDebugRegion();
        }
    }

    /**
     * Invalidates the entire screen.
     */
    virtual void invalidate();

    /**
     * Invalidates the given area.
     *
     * @param  area The area to invalidate.
     */
    virtual void invalidateArea(Rect area);

    /**
     * Get list of the invalidated areas in the current frame.
     * The list is cleared in the beginning of HAL::tick.
     *
     * The list can be usefull on some platforms where the information
     * can be used to reduce the amount of pixels sent to the display.
     *
     * @return Const reference to the list.
     */
    const Vector<Rect, 8>& getInvalidatedAreas()
    {
        return cachedDirtyAreas;
    }

protected:
    /** Protected constructor. */
    Application();

    /**
     * Initiate a draw operation of the entire screen. Standard implementation is to
     * delegate draw request to the current Screen.
     */
    virtual void draw();

    /**
     * Initiate a draw operation of the specified region of the screen. Standard
     * implementation is to delegate draw request to the current Screen.
     *
     * @param [in] rect The area to draw.
     *
     * @note Unlike Widget::draw this is safe to call from user code as it will properly traverse
     *       widgets in z-order.
     * @note The coordinates given must be absolute coordinates.
     */
    virtual void draw(Rect& rect);

    typedef Vector<Rect, 8> RectVector_t; ///< Type to ensure the same number of rects are in the Vector

    /** A class to handle and manage timer widgets. */
    class TimerWidgets
    {
    public:
        /**
         * Tick all active timer widgets. Adding and removing timer widgets during ticks is supported.
         */
        void tickActive();

        /**
         * Adds timer widget. If a timer widget is added from a handleTickEvent(), the timer will not be
         * ticket until the next tick (unless that same widget is already registered, and has not yet
         * been ticked in this tick).
         *
         * @param [in] w The timer widget to add.
         */
        void add(Drawable* w);

        /**
         * Removes the given timer widget.
         *
         * @param  w The timer widget to remove.
         */
        void remove(const Drawable* w);

        /** Clears all timer widgets. */
        void clear();

        /**
         * Gets a counter for a given timer widget. Normally the counter is 1 unless a timer widget has
         * been registered several times.
         *
         * @param  w The timer widget to get the counter for.
         *
         * @return The counter.
         */
        uint16_t getCounter(const Drawable* w) const;

        /**
         * Gets number of active timer widgets.
         *
         * @return The number of active timer widgets.
         */
        uint16_t getNumberOfActiveTimerWidgets() const;

        /**
         * Compacts the timer widgets. All widgets with a counter of 0 will be removed from the list,
         * and the list will be compacted. Compacting the timer widgets during traversal (in
         * tickActive()) is handled properly.
         */
        void compact();

    private:
        Vector<Drawable*, MAX_TIMER_WIDGETS> widgets; ///< List of widgets that receive timer ticks.
        uint8_t counters[MAX_TIMER_WIDGETS];          ///< A counter for each potentially registered timer widget. Increase when registering for timer events, decrease when unregistering.
        uint16_t currentWidget;                       ///< The current widget, used in tickActive().
        uint16_t lastWidget;                          ///< The last widget, used in tickActive().
    } timerWidgets;                                   ///< The timer widgets

    RectVector_t cachedDirtyAreas;        ///< When draw caching is enabled, these rects keeps track of the dirty screen area.
    RectVector_t lastRects;               ///< The dirty areas from last frame that needs to be redrawn because we have swapped frame buffers.
    Rect redraw;                          ///< Rect describing application requested invalidate area
    bool transitionHandled;               ///< True if the transition is done and Screen::afterTransition has been called.
    static Screen* currentScreen;         ///< Pointer to currently displayed Screen.
    static Transition* currentTransition; ///< Pointer to current transition.
    static Application* instance;         ///< Pointer to the instance of the Application-derived subclass. @note Must be set by subclass constructor!
    static DebugPrinter* debugPrinter;    ///< Pointer to the DebugPrinter instance.
    static Rect debugRegionInvalidRect;   ///< Invalidated Debug Region
};

} // namespace touchgfx

#endif // TOUCHGFX_APPLICATION_HPP
