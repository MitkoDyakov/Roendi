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

#include <touchgfx/containers/scrollers/ScrollWheelWithSelectionStyle.hpp>

namespace touchgfx
{
ScrollWheelWithSelectionStyle::ScrollWheelWithSelectionStyle()
    : ScrollWheelBase(),
      drawablesInFirstList(0),
      listCenter(),
      listAfter(),
      extraSizeBeforeSelectedItem(0),
      extraSizeAfterSelectedItem(0),
      marginBeforeSelectedItem(0),
      marginAfterSelectedItem(0),
      drawables(0),
      centerDrawables(0),
      originalUpdateDrawableCallback(0),
      originalUpdateCenterDrawableCallback(0)
{
    ScrollWheelBase::add(listAfter);
    ScrollWheelBase::add(listCenter); // Put center list at top of the first/last list.
}

void ScrollWheelWithSelectionStyle::setWidth(int16_t width)
{
    ScrollWheelBase::setWidth(width);
    if (getHorizontal())
    {
        refreshDrawableListsLayout();
    }
}

void ScrollWheelWithSelectionStyle::setHeight(int16_t height)
{
    ScrollWheelBase::setHeight(height);
    if (!getHorizontal())
    {
        refreshDrawableListsLayout();
    }
}

void ScrollWheelWithSelectionStyle::setHorizontal(bool horizontal)
{
    ScrollWheelBase::setHorizontal(horizontal);
    listCenter.setHorizontal(horizontal);
    listAfter.setHorizontal(horizontal);
    refreshDrawableListsLayout();
}

void ScrollWheelWithSelectionStyle::setCircular(bool circular)
{
    ScrollWheelBase::setCircular(circular);
    listCenter.setCircular(circular);
    listAfter.setCircular(circular);
}

void ScrollWheelWithSelectionStyle::setNumberOfItems(int16_t numberOfItems)
{
    if (numberOfItems != getNumberOfItems())
    {
        ScrollWheelBase::setNumberOfItems(numberOfItems);
        listCenter.setNumberOfItems(numberOfItems);
        listAfter.setNumberOfItems(numberOfItems);
    }
}

void ScrollWheelWithSelectionStyle::setSelectedItemOffset(int16_t offset)
{
    ScrollWheelBase::setSelectedItemOffset(offset);
    refreshDrawableListsLayout();
}

void ScrollWheelWithSelectionStyle::setSelectedItemExtraSize(int16_t extraSizeBefore, int16_t extraSizeAfter)
{
    extraSizeBeforeSelectedItem = extraSizeBefore;
    extraSizeAfterSelectedItem = extraSizeAfter;
    refreshDrawableListsLayout();
}

int16_t ScrollWheelWithSelectionStyle::getSelectedItemExtraSizeBefore() const
{
    return extraSizeBeforeSelectedItem;
}

int16_t ScrollWheelWithSelectionStyle::getSelectedItemExtraSizeAfter() const
{
    return extraSizeAfterSelectedItem;
}

void ScrollWheelWithSelectionStyle::setSelectedItemMargin(int16_t marginBefore, int16_t marginAfter)
{
    marginBeforeSelectedItem = marginBefore;
    marginAfterSelectedItem = marginAfter;
    refreshDrawableListsLayout();
}

int16_t ScrollWheelWithSelectionStyle::getSelectedItemMarginBefore() const
{
    return marginBeforeSelectedItem;
}

int16_t ScrollWheelWithSelectionStyle::getSelectedItemMarginAfter() const
{
    return marginAfterSelectedItem;
}

void ScrollWheelWithSelectionStyle::setSelectedItemPosition(int16_t offset, int16_t extraSizeBefore, int16_t extraSizeAfter, int16_t marginBefore, int16_t marginAfter)
{
    setSelectedItemOffset(offset);
    setSelectedItemExtraSize(extraSizeBefore, extraSizeAfter);
    setSelectedItemMargin(marginBefore, marginAfter);
}

void ScrollWheelWithSelectionStyle::setDrawableSize(int16_t drawableSize, int16_t drawableMargin)
{
    ScrollWheelBase::setDrawableSize(drawableSize, drawableMargin);
    listCenter.setDrawableSize(drawableSize, drawableMargin);
    listAfter.setDrawableSize(drawableSize, drawableMargin);

    // Resize the three lists
    setSelectedItemOffset(distanceBeforeAlignedItem);

    // Changing the drawable size alters number of required drawables, so refresh this
    refreshDrawableListsLayout();
}

void ScrollWheelWithSelectionStyle::setDrawables(DrawableListItemsInterface& drawableListItems, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback,
                                                 DrawableListItemsInterface& centerDrawableListItems, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateCenterDrawableCallback)
{
    drawables = &drawableListItems;
    centerDrawables = &centerDrawableListItems;

    currentAnimationState = NO_ANIMATION;

    originalUpdateDrawableCallback = &updateDrawableCallback;
    originalUpdateCenterDrawableCallback = &updateCenterDrawableCallback;

    refreshDrawableListsLayout();

    setOffset(0);
}

void ScrollWheelWithSelectionStyle::setOffset(int32_t offset)
{
    ScrollWheelBase::setOffset(offset);
    listCenter.setOffset((distanceBeforeAlignedItem - (distanceBeforeAlignedItem - extraSizeBeforeSelectedItem)) + offset);
    listAfter.setOffset((distanceBeforeAlignedItem - (distanceBeforeAlignedItem + itemSize + extraSizeAfterSelectedItem + marginAfterSelectedItem)) + offset);
}

void ScrollWheelWithSelectionStyle::itemChanged(int itemIndex)
{
    ScrollWheelBase::itemChanged(itemIndex);
    listCenter.itemChanged(itemIndex);
    listAfter.itemChanged(itemIndex);
}

void ScrollWheelWithSelectionStyle::refreshDrawableListsLayout()
{
    if (drawables != 0 && centerDrawables != 0)
    {
        const int32_t currentOffset = getOffset();

        const int16_t listCenterPos = distanceBeforeAlignedItem - extraSizeBeforeSelectedItem;
        const int16_t listAfterPos = distanceBeforeAlignedItem + itemSize + (extraSizeAfterSelectedItem + marginAfterSelectedItem);
        const int16_t listBeforeSize = listCenterPos - marginBeforeSelectedItem;
        const int16_t listCenterSize = itemSize + extraSizeBeforeSelectedItem + extraSizeAfterSelectedItem;

        if (getHorizontal())
        {
            const int16_t listAfterSize = getWidth() - listAfterPos;
            list.setPosition(0, 0, listBeforeSize, getHeight());
            listCenter.setPosition(listCenterPos, 0, listCenterSize, getHeight());
            listAfter.setPosition(listAfterPos, 0, listAfterSize, getHeight());
        }
        else
        {
            const int16_t listAfterSize = getHeight() - listAfterPos;
            list.setPosition(0, 0, getWidth(), listBeforeSize);
            listCenter.setPosition(0, listCenterPos, getWidth(), listCenterSize);
            listAfter.setPosition(0, listAfterPos, getWidth(), listAfterSize);
        }

        list.setDrawables(*drawables, 0, *originalUpdateDrawableCallback);
        drawablesInFirstList = list.getNumberOfDrawables();
        listCenter.setDrawables(*centerDrawables, 0, *originalUpdateCenterDrawableCallback);
        listAfter.setDrawables(*drawables, drawablesInFirstList, *originalUpdateDrawableCallback);

        setOffset(keepOffsetInsideLimits(currentOffset, 0));
    }
}
} // namespace touchgfx
