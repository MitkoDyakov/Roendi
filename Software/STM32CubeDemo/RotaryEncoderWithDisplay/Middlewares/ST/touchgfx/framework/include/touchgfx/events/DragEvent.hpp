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
 * @file touchgfx/events/DragEvent.hpp
 *
 * Declares the touchgfx::DragEvent class.
 */
#ifndef TOUCHGFX_DRAGEVENT_HPP
#define TOUCHGFX_DRAGEVENT_HPP

#include <touchgfx/Event.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * A drag event. The only drag event currently supported is DRAGGED,
 * which will be issued every time the input system detects a drag.
 *
 * @see Event
 */
class DragEvent : public Event
{
public:
    /** Values that represent drag event types. */
    enum DragEventType
    {
        DRAGGED ///< An enum constant representing the dragged option
    };

    /**
     * Initializes a new instance of the DragEvent class.
     *
     * @param  type The type of the drag event.
     * @param  oldX The x coordinate of the drag start position (dragged from)
     * @param  oldY The y coordinate of the drag start position (dragged from)
     * @param  newX The x coordinate of the new position (dragged to)
     * @param  newY The y coordinate of the new position (dragged to)
     */
    DragEvent(DragEventType type, int16_t oldX, int16_t oldY, int16_t newX, int16_t newY)
        : dragEventType(type), dragOldX(oldX), dragOldY(oldY), dragNewX(newX), dragNewY(newY)
    {
    }

    /**
     * Copy constructor.
     *
     * @param  dragEvent The drag event.
     */
    DragEvent(const DragEvent& dragEvent)
    {
        *this = dragEvent;
    }

    /**
     * Gets the old x coordinate, i.e. where the drag operation was
     * started (dragged from).
     *
     * @return The old x coordinate, i.e. where the drag operation was started (dragged from).
     */
    int16_t getOldX() const
    {
        return dragOldX;
    }

    /**
     * Gets the old y coordinate, i.e. where the drag operation was
     * started (dragged from).
     *
     * @return The old y coordinate, i.e. where the drag operation was started (dragged from).
     */
    int16_t getOldY() const
    {
        return dragOldY;
    }

    /**
     * Gets the new x coordinate (dragged to).
     *
     * @return The new x coordinate (dragged to).
     */
    int16_t getNewX() const
    {
        return dragNewX;
    }

    /**
     * Gets the new x coordinate (dragged to).
     *
     * @return The new y coordinate (dragged to).
     */
    int16_t getNewY() const
    {
        return dragNewY;
    }

    /**
     * Gets the type of this drag event.
     *
     * @return The type of this drag event.
     */
    DragEventType getType() const
    {
        return dragEventType;
    }

    /**
     * Gets the distance in x coordinates (how long was the drag).
     *
     * @return The distance of this drag event.
     */
    int16_t getDeltaX() const
    {
        return dragNewX - dragOldX;
    }

    /**
     * Gets the distance in y coordinates (how long was the drag).
     *
     * @return The distance of this drag event.
     */
    int16_t getDeltaY() const
    {
        return dragNewY - dragOldY;
    }

    /**
     * Gets event type.
     *
     * @return The type of this event.
     */
    virtual Event::EventType getEventType() const
    {
        return Event::EVENT_DRAG;
    }

    /**
     * Assignment operator.
     *
     * @param  dragEvent The drag event.
     *
     * @return A shallow copy of this object.
     */
    const DragEvent& operator=(const DragEvent& dragEvent)
    {
        dragEventType = dragEvent.dragEventType;
        dragOldX = dragEvent.dragOldX;
        dragOldY = dragEvent.dragOldY;
        dragNewX = dragEvent.dragNewX;
        dragNewY = dragEvent.dragNewY;
        return *this;
    }

private:
    DragEventType dragEventType;
    int16_t dragOldX;
    int16_t dragOldY;
    int16_t dragNewX;
    int16_t dragNewY;
};

} // namespace touchgfx

#endif // TOUCHGFX_DRAGEVENT_HPP
