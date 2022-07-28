#include <gui/screen_screen/screenView.hpp>
#include "CST816S.h"

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

uint16_t x = 0;
uint16_t y = 0;
extern data_struct data;

void screenView::handleTickEvent()
{
    if(x != data.x || y != data.y)
    {
        x = data.x;
        y = data.y;
        circle2.setPosition(x-20,  y-20, 40, 40);
        circle2.invalidate();
        circle1.invalidate();
    }

}
