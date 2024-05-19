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

#include <touchgfx/Application.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/Drawable.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/containers/SwipeContainer.hpp>

namespace touchgfx
{
SwipeContainer::SwipeContainer()
    : Container(),
      currentState(NO_ANIMATION),
      animationCounter(0),
      swipeCutoff(80),
      dragX(0),
      animateDistance(0),
      startX(0),
      endElasticWidth(30),
      animationDuration(20),
      pages(EAST),
      pageIndicator()
{
    Application::getInstance()->registerTimerWidget(this);

    setTouchable(true);

    Container::add(pages);
    Container::add(pageIndicator);
}

SwipeContainer::~SwipeContainer()
{
    Application::getInstance()->unregisterTimerWidget(this);
}

void SwipeContainer::add(Drawable& page)
{
    pages.add(page);

    pageIndicator.setNumberOfPages(getNumberOfPages() + 1);
    setSelectedPage(getSelectedPage());

    setWidthHeight(page);
}

void SwipeContainer::remove(Drawable& page)
{
    Drawable* tmp = pages.getFirstChild();

    if (pageIndicator.getNumberOfPages() == 0)
    {
        return;
    }

    // looks for the child matching page
    // to ensure that the page indicator only counts down if a page is removed
    while (tmp)
    {
        if (tmp == &page)
        {
            pages.remove(page);
            pageIndicator.setNumberOfPages(getNumberOfPages() - 1);

            const uint8_t numPages = getNumberOfPages();
            if (numPages == 0)
            {
                invalidate();
                setWidthHeight(0, 0);
            }
            else
            {
                const uint8_t curPage = getSelectedPage();
                setSelectedPage(MIN(curPage, numPages - 1));
            }
            return;
        }
        else
        {
            tmp = tmp->getNextSibling();
        }
    }
}

void SwipeContainer::setEndSwipeElasticWidth(uint16_t width)
{
    endElasticWidth = width;
}

void SwipeContainer::setSwipeCutoff(uint16_t cutoff)
{
    swipeCutoff = cutoff;
}

void SwipeContainer::setPageIndicatorBitmaps(const Bitmap& normalPage, const Bitmap& highlightedPage)
{
    pageIndicator.setBitmaps(normalPage, highlightedPage);
}

void SwipeContainer::setPageIndicatorXY(int16_t x, int16_t y)
{
    pageIndicator.setXY(x, y);
}

void SwipeContainer::setPageIndicatorXYWithCenteredX(int16_t x, int16_t y)
{
    setPageIndicatorCenteredX(x);
    pageIndicator.setY(y);
}

void SwipeContainer::setPageIndicatorCenteredX()
{
    setPageIndicatorCenteredX(getWidth() / 2);
}

void SwipeContainer::setPageIndicatorCenteredX(int16_t x)
{
    pageIndicator.setX(x - pageIndicator.getWidth() / 2);
}

void SwipeContainer::setSelectedPage(uint8_t pageIndex)
{
    if (pageIndex < getNumberOfPages())
    {
        pageIndicator.setCurrentPage(pageIndex);
        adjustPages();
    }
}

uint8_t SwipeContainer::getSelectedPage() const
{
    return pageIndicator.getCurrentPage();
}

void SwipeContainer::handleTickEvent()
{
    if (currentState == ANIMATE_SWIPE_CANCELLED_LEFT)
    {
        animateSwipeCancelledLeft();
    }
    else if (currentState == ANIMATE_SWIPE_CANCELLED_RIGHT)
    {
        animateSwipeCancelledRight();
    }
    else if (currentState == ANIMATE_LEFT)
    {
        animateLeft();
    }
    else if (currentState == ANIMATE_RIGHT)
    {
        animateRight();
    }
    else if (currentState == ANIMATE_LEFT_WITH_BUTTON)
    {
        animateLeftWithButton();
    }
    else if (currentState == ANIMATE_RIGHT_WITH_BUTTON)
    {
        animateRightWithButton();
    }
}

void SwipeContainer::handleClickEvent(const ClickEvent& event)
{
    // If an animation is already in progress do not
    // react to clicks
    if (currentState != NO_ANIMATION)
    {
        return;
    }

    if (event.getType() == ClickEvent::RELEASED)
    {
        // Save current position for use during animation
        animateDistance = dragX;
        startX = pages.getX();
        const uint8_t curPage = getSelectedPage();
        if (dragX < 0)
        {
            if (curPage == getNumberOfPages() - 1 || dragX > -swipeCutoff)
            {
                currentState = ANIMATE_SWIPE_CANCELLED_LEFT;
            }
            else
            {
                currentState = ANIMATE_LEFT;
            }
        }
        else if (dragX > 0)
        {
            if (curPage == 0 || dragX < swipeCutoff)
            {
                currentState = ANIMATE_SWIPE_CANCELLED_RIGHT;
            }
            else
            {
                currentState = ANIMATE_RIGHT;
            }
        }
    }
}

void SwipeContainer::handleDragEvent(const DragEvent& event)
{
    // If an animation is already in progress do not
    // react to drags
    if (currentState != NO_ANIMATION)
    {
        return;
    }

    dragX += event.getDeltaX();

    // Do not show too much background next to end pages
    const uint8_t curPage = getSelectedPage();
    if (curPage == 0 && dragX > endElasticWidth)
    {
        dragX = static_cast<int16_t>(endElasticWidth);
    }
    else if (curPage == getNumberOfPages() - 1 && dragX < -endElasticWidth)
    {
        dragX = -static_cast<int16_t>(endElasticWidth);
    }

    adjustPages();
}

void SwipeContainer::handleGestureEvent(const GestureEvent& event)
{
    // Do not accept gestures while animating
    if (currentState != NO_ANIMATION)
    {
        return;
    }

    if (event.getType() == GestureEvent::SWIPE_HORIZONTAL)
    {
        // Save current position for use during animation
        animateDistance = dragX;
        startX = pages.getX();

        const uint8_t curPage = getSelectedPage();
        if (event.getVelocity() < 0 && curPage < getNumberOfPages() - 1)
        {
            currentState = ANIMATE_LEFT;
        }
        else if (event.getVelocity() > 0 && curPage > 0)
        {
            currentState = ANIMATE_RIGHT;
        }
    }
}

void SwipeContainer::adjustPages()
{
    pages.moveTo(-static_cast<int16_t>(getSelectedPage() * getWidth()) + dragX, 0);
}

void SwipeContainer::animateSwipeCancelledLeft()
{
    const uint8_t duration = 14;

    if (animationCounter <= duration)
    {
        const int16_t delta = EasingEquations::backEaseOut(animationCounter, 0, -animateDistance, duration);
        dragX = animateDistance + delta;

        adjustPages();
    }
    else
    {
        // Final step: stop the animation
        currentState = NO_ANIMATION;
        animationCounter = 0;
        dragX = 0;
        adjustPages();
    }
    animationCounter++;
}

void SwipeContainer::animateSwipeCancelledRight()
{
    const uint8_t duration = 14;

    if (animationCounter <= duration)
    {
        const int16_t delta = EasingEquations::backEaseOut(animationCounter, 0, animateDistance, duration);
        dragX = animateDistance - delta;

        adjustPages();
    }
    else
    {
        // Final step: stop the animation
        currentState = NO_ANIMATION;
        animationCounter = 0;
        dragX = 0;
        adjustPages();
    }
    animationCounter++;
}

void SwipeContainer::animateLeft()
{
    const uint8_t duration = 10;

    if (animationCounter <= duration)
    {
        const int16_t delta = EasingEquations::cubicEaseOut(animationCounter, 0, getWidth() + animateDistance, duration);
        dragX = animateDistance - delta;
    }
    else
    {
        // Final step: stop the animation
        currentState = NO_ANIMATION;
        animationCounter = 0;
        dragX = 0;
        pageIndicator.goRight();
    }
    adjustPages();
    animationCounter++;
}

void SwipeContainer::animateRight()
{
    const uint8_t duration = 10;

    if (animationCounter <= duration)
    {
        const int16_t delta = EasingEquations::cubicEaseOut(animationCounter, 0, getWidth() - animateDistance, duration);
        dragX = animateDistance + delta;
    }
    else
    {
        // Final step: stop the animation
        currentState = NO_ANIMATION;
        animationCounter = 0;
        dragX = 0;
        pageIndicator.goLeft();
    }
    adjustPages();
    animationCounter++;
}

SwipeContainer::PageIndicator::PageIndicator()
    : Container(),
      unselectedPages(),
      selectedPage(),
      numberOfPages(0),
      currentPage(0)
{
    unselectedPages.setXY(0, 0);
    selectedPage.setXY(0, 0);

    Container::add(unselectedPages);
    Container::add(selectedPage);
}

void SwipeContainer::PageIndicator::setNumberOfPages(uint8_t size)
{
    numberOfPages = size;

    if (unselectedPages.getBitmapId() != BITMAP_INVALID)
    {
        const int dotWidth = Bitmap(unselectedPages.getBitmap()).getWidth();
        unselectedPages.setWidth(dotWidth * size);

        // adjust size of container according to the actual bitmaps
        invalidate();
        setWidthHeight(unselectedPages);
        setCurrentPage(MIN(size, currentPage));
        invalidate();
    }
}

void SwipeContainer::PageIndicator::setBitmaps(const Bitmap& normalPage, const Bitmap& highlightedPage)
{
    selectedPage.setBitmap(highlightedPage);
    unselectedPages.setBitmap(normalPage);
    if (numberOfPages > 0)
    {
        setNumberOfPages(numberOfPages);
    }
}

void SwipeContainer::PageIndicator::goRight()
{
    setCurrentPage((currentPage + 1) % numberOfPages);
}

void SwipeContainer::PageIndicator::goLeft()
{
    setCurrentPage((currentPage + numberOfPages - 1) % numberOfPages);
}

void SwipeContainer::PageIndicator::setCurrentPage(uint8_t page)
{
    if (page < numberOfPages && page != currentPage)
    {
        currentPage = page;
        const int dotWidth = Bitmap(unselectedPages.getBitmap()).getWidth();
        selectedPage.moveTo(page * dotWidth, selectedPage.getY());
    }
}

uint8_t SwipeContainer::PageIndicator::getNumberOfPages() const
{
    return numberOfPages;
}

uint8_t SwipeContainer::PageIndicator::getCurrentPage() const
{
    return currentPage;
}

void SwipeContainer::goNextPage(uint8_t duration)
{
    animationDuration = duration;
    if (getSelectedPage() < getNumberOfPages() - 1)
    {
        currentState = ANIMATE_LEFT_WITH_BUTTON;
    }
}

void SwipeContainer::goPreviousPage(uint8_t duration)
{
    animationDuration = duration;
    if (getSelectedPage() > 0)
    {
        currentState = ANIMATE_RIGHT_WITH_BUTTON;
    }
}

uint8_t SwipeContainer::getAnimationDuration() const
{
    return animationDuration;
}

void SwipeContainer::setAnimationDuration(uint8_t newDuration)
{
    animationDuration = newDuration > 0 ? newDuration : 0;
}

void SwipeContainer::animateLeftWithButton()
{
    if (animationCounter <= animationDuration)
    {
        const int16_t delta = EasingEquations::cubicEaseOut(animationCounter, 0, getWidth() + animateDistance, animationDuration);
        dragX = animateDistance - delta;
    }
    else
    {
        // Final step: stop the animation
        currentState = NO_ANIMATION;
        animationCounter = 0;
        dragX = 0;
        pageIndicator.goRight();
    }
    adjustPages();
    animationCounter++;
}

void SwipeContainer::animateRightWithButton()
{
    if (animationCounter <= animationDuration)
    {
        const int16_t delta = EasingEquations::cubicEaseOut(animationCounter, 0, getWidth() - animateDistance, animationDuration);
        dragX = animateDistance + delta;
    }
    else
    {
        // Final step: stop the animation
        currentState = NO_ANIMATION;
        animationCounter = 0;
        dragX = 0;
        pageIndicator.goLeft();
    }
    adjustPages();
    animationCounter++;
}

} // namespace touchgfx
