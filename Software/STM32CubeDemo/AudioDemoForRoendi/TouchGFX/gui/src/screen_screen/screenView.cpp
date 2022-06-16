#include <gui/screen_screen/screenView.hpp>
#include <touchgfx/Color.hpp>
#include <BitmapDatabase.hpp>
#include "math.h"

uint8_t oldProgressValue = 0;
extern volatile uint8_t progress;

screenView::screenView()
{

}

void screenView::setupScreen()
{
    screenViewBase::setupScreen();
}

void screenView::tearDownScreen()
{
    screenViewBase::tearDownScreen();
}

void screenView::handleTickEvent()
{
    float len = 0;

    if(oldProgressValue != progress)
    {
        len = 60 + round(((float)130)*(((float)progress)/100.0));
        line2.setEnd(len, 25.0f);
        line2.invalidate();
        line1.invalidate();
        oldProgressValue = progress;
    }
}
